/*
 * Copyright 2022 Fraunhofer IESE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <tlm_utils/simple_target_socket.h>

#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "base/trace.hh"
#include "debug/ScNode.hh"
#include "params/TLM_Wrapper.hh"
#include "sc_tlm_wrapper.hh"
#include "ScNode/ScNodeWrapper.h"
#include "ScNode/DebugManager.h"

#include "systemc/ext/systemc"
#include "systemc/ext/tlm"
#include <dlfcn.h>

using namespace std;
using namespace sc_core;
using namespace gem5;

TlmWrapper::TlmWrapper(
    sc_module_name name,
    const std::string& scname,
    AddrRange range) :
    sc_module(name),
    scnode_module_name(scname),
    tSocket("tSocket"),
    iSocket("iSocket"),
    cycleTime(sc_time(10,SC_NS)),
    tsck_wrapper(tSocket, std::string(name) + ".tsck", InvalidPortID),
    isck_wrapper(iSocket, std::string(name) + ".isck", InvalidPortID),
    range(range)
{
    SC_METHOD(fwClock);
    sensitive << fwEvent;
    dont_initialize();

    SC_METHOD(bwClock);
    sensitive << bwEvent;
    dont_initialize();

    void* handle = dlopen("./libscnode.so", RTLD_LAZY | RTLD_DEEPBIND);
    if (!handle) {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
        throw std::runtime_error("error");
    }

    // 获取工厂函数
    create_scnode_t make_scnode = (create_scnode_t)dlsym(handle, "make_scnode");
    if (!make_scnode) {
        std::cerr << "Failed to find make_scnode function: " << dlerror() << std::endl;
        dlclose(handle);
        throw std::runtime_error("error");
    }
    scnode_start = (scnode_start_t)dlsym(handle, "scnode_start"); 
    if (!scnode_start) {
        std::cerr << "Failed to find scnode_start function: " << dlerror() << std::endl;
        dlclose(handle);
        throw std::runtime_error("error");
    }
    scnode_fwhandler = (scnode_fwhandler_t)dlsym(handle, "scnode_fwhandler"); 
    if (!scnode_fwhandler) {
        std::cerr << "Failed to find scnode_fwhandler function: " << dlerror() << std::endl;
        dlclose(handle);
        throw std::runtime_error("error");
    }

    scnode_bwhandler = (scnode_bwhandler_t)dlsym(handle, "scnode_bwhandler"); 
    if (!scnode_bwhandler) {
        std::cerr << "Failed to find scnode_bwhandler function: " << dlerror() << std::endl;
        dlclose(handle);
        throw std::runtime_error("error");
    }


    scnode_debugger_t make_debugger = (scnode_debugger_t)dlsym(handle, "scnode_debugger"); 
    if (!make_debugger) {
        std::cerr << "Failed to find scnode_debugger function: " << dlerror() << std::endl;
        dlclose(handle);
        throw std::runtime_error("error");
    }


    scnode = make_scnode((uint64_t)this, 
       cycleTime,
       scnode_module_name,
       [&](transType trans)
       {
          // sc to gem5 fw transport
          //std::cout << "TLM TlmWrapper callback receive fw trans " << trans.first << std::endl;
          tlmToGem5FwQueue.push(trans);
          bwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
       },
       [&](transType trans)
       {
          // sc to gem5 bw transport
          //std::cout << "TLM TlmWrapper callback receive bw trans " << trans.first << std::endl;
          tlmToGem5BwQueue.push(trans);
          bwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
       });

    scnode_debugger = make_debugger(scnode);

    //tSocket.register_b_transport(this, &TlmWrapper::b_transport);
    tSocket.register_nb_transport_fw(this, &TlmWrapper::nb_transport_fw);
    iSocket.register_nb_transport_bw(this, &TlmWrapper::nb_transport_bw);

    std::cout << "TLM TlmWrapper Online" << std::endl;
}

/*
void TlmWrapper::peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase) {
    if (phase == tlm::BEGIN_REQ) {
        scnode_fwhandler(scnode, it);
        scnode_start(scnode, sc_time_stamp());

}
*/

void TlmWrapper::fwClock() {
    if (isFullCycle(sc_time_stamp(), cycleTime)) {
        while(!gem5ToTlmFwQueue.empty()) {
            auto it = gem5ToTlmFwQueue.front();
            //std::cout << "TLM TlmWrapper gem5ToTlm fw Clock @" << sc_time_stamp() << " payload=" <<  it.first << " phase=" << it.second << std::endl;
            scnode_fwhandler(scnode, it);
            gem5ToTlmFwQueue.pop();
        }
        while(!gem5ToTlmBwQueue.empty()) {
            auto it = gem5ToTlmBwQueue.front();
            //std::cout << "TLM TlmWrapper gem5ToTlm bw Clock @" << sc_time_stamp() << " payload=" <<  it.first << " phase=" << it.second << std::endl;
            scnode_bwhandler(scnode, it);
            gem5ToTlmBwQueue.pop();
        }
        sc_time ts = sc_time_stamp();
        scnode_start(scnode, sc_time_stamp());
        fwEvent.notify(cycleTime);
    } else {
        sc_time ts = sc_time_stamp();
        scnode_start(scnode, ts);
    //if (!fwQueue.empty()) {
        sc_time delta_time = alignAtNext(ts, cycleTime);
        //std::cout << "TLM TlmWrapper start to ts=" << ts << "delta=" << delta_time << "cycleTime" << cycleTime << std::endl;
        std::cout << "TLM TlmWrapper gem5ToSc Clock @" << ts << " delta_time=" << delta_time << "cycle_time=" << cycleTime << std::endl;
        fwEvent.notify(delta_time);
    }
}

void TlmWrapper::bwClock() {
    if (isFullCycle(sc_time_stamp(), cycleTime)) {
        while( !tlmToGem5FwQueue.empty()) {
            auto it = tlmToGem5FwQueue.front();
            tlm::tlm_generic_payload* payload = it.first;
            tlm::tlm_phase phase = it.second;
            sc_core::sc_time t = SC_ZERO_TIME;
            //std::cout << "TLM TlmWrapper scToGem5 fw Clock @" << sc_time_stamp() << " payload=" <<  it.first << " phase=" << it.second << std::endl;
            auto ret = iSocket->nb_transport_fw(*payload, phase, t);
            assert(ret == tlm::TLM_ACCEPTED);
            tlmToGem5FwQueue.pop();
        }
        while( !tlmToGem5BwQueue.empty()) {
            auto it = tlmToGem5BwQueue.front();
            tlm::tlm_generic_payload* payload = it.first;
            tlm::tlm_phase phase = it.second;
            sc_core::sc_time t = SC_ZERO_TIME;
            //std::cout << "TLM TlmWrapper scToGem5 bw Clock @" << sc_time_stamp() << " payload=" <<  it.first << " phase=" << it.second << std::endl;
            auto ret = tSocket->nb_transport_bw(*payload, phase, t);
            tlmToGem5BwQueue.pop();

        }
    };
    if (!tlmToGem5FwQueue.empty() || !tlmToGem5BwQueue.empty()) {
        bwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    }
}

/*
void TlmWrapper::b_transport(
    tlm::tlm_generic_payload &payload,
    sc_core::sc_time &delay)
{
    // Subtract base address offset
    payload.set_address(payload.get_address() - range.start());
    scnode->fwTransaction({payload, delay});
}
*/

// gem5 to sc fw
tlm::tlm_sync_enum TlmWrapper::nb_transport_fw(
    tlm::tlm_generic_payload &payload,
    tlm::tlm_phase &phase,
    sc_core::sc_time &fwDelay)
{
    // Subtract base address offset
    payload.set_address(payload.get_address() - range.start());

    gem5ToTlmFwQueue.push({&payload, phase});
    sc_time ts = sc_time_stamp();
    sc_time delta_time = alignAtNext(ts, cycleTime);

    //std::cout << "TLM TlmWrapper gem5Tsc fw @" << ts << " payload " << &payload << " delta=" << delta_time << std::endl;
    fwEvent.notify(delta_time);
    return tlm::TLM_ACCEPTED;
}


// gem5 to sc fw
tlm::tlm_sync_enum TlmWrapper::nb_transport_bw(
    tlm::tlm_generic_payload &payload,
    tlm::tlm_phase &phase,
    sc_core::sc_time &bwDelay)
{
    gem5ToTlmBwQueue.push({&payload, phase});
    sc_time ts = sc_time_stamp();
    sc_time delta_time = alignAtNext(ts, cycleTime);
    //std::cout << "TLM TlmWrapper gem5Tosc bw @" << ts << " payload " << &payload << " delta=" << delta_time << std::endl;
    fwEvent.notify(delta_time);
    return tlm::TLM_ACCEPTED;
    //return tSocket->nb_transport_bw(payload, phase, bwDelay);
}
/*

unsigned int TlmWrapper::transport_dbg(tlm::tlm_generic_payload &trans)
{
    // Subtract base address offset
    trans.set_address(trans.get_address() - range.start());

    return iSocket->transport_dbg(trans);
}
*/

// This "create" method bridges the python configuration and the systemc
// objects. It instantiates the Printer object and sets it up using the
// parameter values from the config, just like it would for a SimObject. The
// systemc object could accept those parameters however it likes, for instance
// through its constructor or by assigning them to a member variable.
TlmWrapper *
gem5::TLM_WrapperParams::create() const
{
    TlmWrapper *wrapper = new TlmWrapper(name.c_str(), scname.c_str(), range);
    return wrapper;
}

gem5::Port
&TlmWrapper::gem5_getPort(const std::string &if_name, int idx)
{
    panic_if(idx != InvalidPortID, "This object doesn't support vector ports");
    DPRINTF(ScNode, "TlmTlmWrapper Get Port name %s, %d\n", if_name, idx);
    if (if_name == "tsck") {
      return tsck_wrapper;
    } else if (if_name == "isck") {
      return isck_wrapper;
    } else {
      panic("sc_tlm_wrapper should use tsck or isck port name");
    }
}
