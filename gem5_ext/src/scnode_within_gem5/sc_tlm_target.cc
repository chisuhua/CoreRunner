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
#include "params/TLM_Target.hh"
#include "sc_tlm_target.hh"
#include "ScNode/ScNodeTarget.h"
#include "ScNode/DebugManager.h"

#include "systemc/ext/systemc"
#include "systemc/ext/tlm"
#include <dlfcn.h>

using namespace std;
using namespace sc_core;
using namespace gem5;

Target::Target(
    sc_module_name name,
    const std::string& scname,
    AddrRange range) :
    sc_module(name),
    scnode_module_name(scname),
    tSocket("tSocket"),
    cycleTime(sc_time(10,SC_NS)),
    tsck_wrapper(tSocket, std::string(name) + ".tsck", InvalidPortID),
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
    create_sctarget_t make_sctarget = (create_sctarget_t)dlsym(handle, "make_sctarget");
    if (!make_sctarget) {
        std::cerr << "Failed to find make_scnode function: " << dlerror() << std::endl;
        dlclose(handle);
        throw std::runtime_error("error");
    }
    sctarget_start = (sctarget_start_t)dlsym(handle, "sctarget_start"); 
    if (!sctarget_start) {
        std::cerr << "Failed to find scnode_start function: " << dlerror() << std::endl;
        dlclose(handle);
        throw std::runtime_error("error");
    }
    sctarget_fwhandler = (sctarget_fwhandler_t)dlsym(handle, "sctarget_fwhandler"); 
    if (!sctarget_fwhandler) {
        std::cerr << "Failed to find scnode_fwhandler function: " << dlerror() << std::endl;
        dlclose(handle);
        throw std::runtime_error("error");
    }

    sctarget_bwhandler = (sctarget_bwhandler_t)dlsym(handle, "sctarget_bwhandler"); 
    if (!sctarget_bwhandler) {
        std::cerr << "Failed to find sctarget_bwhandler function: " << dlerror() << std::endl;
        dlclose(handle);
        throw std::runtime_error("error");
    }

    sctarget_debugger_t make_debugger = (sctarget_debugger_t)dlsym(handle, "sctarget_debugger"); 
    if (!make_debugger) {
        std::cerr << "Failed to find sctarget_debugger function: " << dlerror() << std::endl;
        dlclose(handle);
        throw std::runtime_error("error");
    }


    scnode = make_sctarget((uint64_t)this, 
       cycleTime,
       scnode_module_name,
       [&](transType trans)
       {
          // sc to gem5 bw transport
          //std::cout << "TLM Target callback receive bw trans " << trans.first << std::endl;
          tlmToGem5BwQueue.push(trans);
          bwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
       });

    sctarget_debugger = make_debugger(scnode);

    //tSocket.register_b_transport(this, &Target::b_transport);
    tSocket.register_nb_transport_fw(this, &Target::nb_transport_fw);

    std::cout << "TLM Target Online" << std::endl;
}

/*
void Target::peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase) {
    if (phase == tlm::BEGIN_REQ) {
        scnode_fwhandler(scnode, it);
        scnode_start(scnode, sc_time_stamp());

}
*/

void Target::fwClock() {
    if (isFullCycle(sc_time_stamp(), cycleTime)) {
        while(!gem5ToTlmFwQueue.empty()) {
            auto it = gem5ToTlmFwQueue.front();
            std::cout << "TLM Target gem5ToTlm fw Clock @" << sc_time_stamp() << " payload=" <<  it.first << " phase=" << it.second << std::endl;
            sctarget_fwhandler(scnode, it);
            gem5ToTlmFwQueue.pop();
        }
        sc_time ts = sc_time_stamp();
        sctarget_start(scnode, sc_time_stamp());
        fwEvent.notify(cycleTime);
    } else {
        sc_time ts = sc_time_stamp();
        sctarget_start(scnode, ts);
    //if (!fwQueue.empty()) {
        sc_time delta_time = alignAtNext(ts, cycleTime);
        //std::cout << "TLM Target start to ts=" << ts << "delta=" << delta_time << "cycleTime" << cycleTime << std::endl;
        std::cout << "TLM Target gem5ToSc Clock @" << ts << " delta_time=" << delta_time << "cycle_time=" << cycleTime << std::endl;
        fwEvent.notify(delta_time);
    }
}

void Target::bwClock() {
    if (isFullCycle(sc_time_stamp(), cycleTime)) {
        while( !tlmToGem5BwQueue.empty()) {
            auto it = tlmToGem5BwQueue.front();
            tlm::tlm_generic_payload* payload = it.first;
            tlm::tlm_phase phase = it.second;
            sc_core::sc_time t = SC_ZERO_TIME;
            //std::cout << "TLM Target scToGem5 bw Clock @" << sc_time_stamp() << " payload=" <<  it.first << " phase=" << it.second << std::endl;
            auto ret = tSocket->nb_transport_bw(*payload, phase, t);
            tlmToGem5BwQueue.pop();

        }
    };
    if (!tlmToGem5BwQueue.empty()) {
        bwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    }
}

/*
void Target::b_transport(
    tlm::tlm_generic_payload &payload,
    sc_core::sc_time &delay)
{
    // Subtract base address offset
    payload.set_address(payload.get_address() - range.start());
    scnode->fwTransaction({payload, delay});
}
*/

// gem5 to sc fw
tlm::tlm_sync_enum Target::nb_transport_fw(
    tlm::tlm_generic_payload &payload,
    tlm::tlm_phase &phase,
    sc_core::sc_time &fwDelay)
{
    // Subtract base address offset
    payload.set_address(payload.get_address() - range.start());

    gem5ToTlmFwQueue.push({&payload, phase});
    sc_time ts = sc_time_stamp();
    sc_time delta_time = alignAtNext(ts, cycleTime);

    std::cout << "TLM Target gem5ToTlm fw @" << ts << " payload " << &payload << " delta=" << delta_time << std::endl;
    fwEvent.notify(delta_time);
    return tlm::TLM_ACCEPTED;
}


/*

unsigned int Target::transport_dbg(tlm::tlm_generic_payload &trans)
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
Target *
gem5::TLM_TargetParams::create() const
{
    Target *target = new Target(name.c_str(), scname.c_str(), range);
    return target;
}

gem5::Port
&Target::gem5_getPort(const std::string &if_name, int idx)
{
    panic_if(idx != InvalidPortID, "This object doesn't support vector ports");
    DPRINTF(ScNode, "TlmTarget Get Port name %s, %d\n", if_name, idx);
    if (if_name == "tsck") {
      return tsck_wrapper;
    } else {
      panic("sc_tlm_target should use tsck or isck port name");
    }
}
