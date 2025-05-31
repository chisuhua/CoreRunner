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

#ifndef __SYSTEC_TLM_WRAPPER
#define __SYSTEC_TLM_WRAPPER

#include <tlm_utils/simple_target_socket.h>

#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "base/trace.hh"
#include "systemc/ext/core/sc_module_name.hh"
#include "systemc/tlm_port_wrapper.hh"

#include "systemc/ext/tlm_utils/simple_target_socket.h"
#include "systemc/ext/tlm_utils/simple_initiator_socket.h"
#include "systemc/ext/systemc"
#include "systemc/ext/tlm"
#include "ScNode/ScNodeWrapper.h"

using namespace std;
using namespace sc_core;
using namespace gem5;

//class ScNodeWrapper;
//using transType = std::pair<tlm::tlm_generic_payload*, tlm::tlm_phase>;
//using transFunc = std::function<void(transType)>;
//typedef ScNodeWrapper* (*create_scnode_t)(uint64_t, sc_core::sc_time, const std::string&, transFunc);
//typedef void (*scnode_start_t)(ScNodeWrapper* obj, sc_core::sc_time t);
//typedef void (*scnode_fwhandler_t)(ScNodeWrapper* obj, transType t);


SC_MODULE(TlmWrapper)
{
  public:
    SC_HAS_PROCESS(TlmWrapper);
    TlmWrapper(sc_core::sc_module_name name, const std::string& scname, AddrRange range);
    const std::string& scnode_module_name;

    tlm_utils::simple_target_socket<TlmWrapper> tSocket;
    tlm_utils::simple_initiator_socket<TlmWrapper> iSocket;

    sc_time cycleTime;
    sc_gem5::TlmTargetWrapper<32> tsck_wrapper;
    sc_gem5::TlmInitiatorWrapper<32> isck_wrapper;

    std::queue<transType> gem5ToTlmFwQueue;
    std::queue<transType> gem5ToTlmBwQueue;
    std::queue<transType> tlmToGem5BwQueue;
    std::queue<transType> tlmToGem5FwQueue;
    sc_event fwEvent;
    sc_event bwEvent;


  private:
    void bwClock();
    void fwClock();

    gem5::Port &gem5_getPort(const std::string &if_name, int idx=-1) override;
    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &payload,
                                       tlm::tlm_phase &phase,
                                       sc_core::sc_time &fwDelay);


    tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &payload,
                                       tlm::tlm_phase &phase,
                                       sc_core::sc_time &bwDelay);
/*
    void b_transport(tlm::tlm_generic_payload& trans,
                             sc_time& delay);

    unsigned int transport_dbg(tlm::tlm_generic_payload &trans);
*/

    ScNodeWrapper* scnode;
    DebugManager* scnode_debugger;
    scnode_start_t scnode_start;
    scnode_fwhandler_t scnode_fwhandler;
    scnode_bwhandler_t scnode_bwhandler;
    
    AddrRange range;

    //void executeTransaction(tlm::tlm_generic_payload& trans);
};

#endif // __SYSTEC_TLM_WRAPPER
