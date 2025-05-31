#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <systemc>
#include <tlm>
#include <functional>
#include <queue>

//#include <tlm_utils/multi_passthrough_initiator_socket.h>
//#include <tlm_utils/multi_passthrough_target_socket.h>

#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

//#include "systemc/ext/tlm_utils/simple_target_socket.h"
//#include "systemc/ext/tlm_utils/simple_initiator_socket.h"

using transType = std::pair<tlm::tlm_generic_payload*, tlm::tlm_phase>;
using transFunc = std::function<void(transType)>;

class ScNodeTarget;
class DebugManager;

typedef ScNodeTarget* (*create_sctarget_t)(uint64_t, sc_core::sc_time, const std::string&, transFunc);
typedef void (*sctarget_start_t)(ScNodeTarget* obj, sc_core::sc_time t);
typedef void (*sctarget_fwhandler_t)(ScNodeTarget* obj, transType t);
typedef void (*sctarget_bwhandler_t)(ScNodeTarget* obj, transType t);
typedef DebugManager* (*sctarget_debugger_t)(ScNodeTarget* obj);


inline sc_core::sc_time alignAtNext(sc_core::sc_time time, sc_core::sc_time alignment)
{
    return std::ceil(time / alignment) * alignment - time;
}
inline bool isFullCycle(sc_core::sc_time time, sc_core::sc_time cycleTime)
{
    return alignAtNext(time, cycleTime) == sc_core::SC_ZERO_TIME;
}




template <typename... Args>
inline std::string GetFormattedString(const std::string& fmt, Args&&... args) {
    size_t size = snprintf(nullptr, 0, fmt.c_str(), std::forward<Args>(args)...) + 1;
    std::vector<char> buf(size);
    snprintf(buf.data(), size, fmt.c_str(), std::forward<Args>(args)...);
    return std::string(buf.data(), buf.data()+size-1);
}

class ScNodeTarget : public sc_core::sc_module
{
  public:
    tlm_utils::simple_initiator_socket<ScNodeTarget> iSocket;

    //SC_HAS_PROCESS(ScNodeTarget);
    explicit ScNodeTarget(sc_core::sc_module_name name,
                  const char* scnode_name,
                  sc_core::sc_time cycle_time,
                  transFunc bw_handler,
                  uint64_t simctx_id,
                  bool recordable);

    void gem5ToTlmBwHandler(transType);
    void gem5ToTlmFwHandler(transType);
    transFunc tlmToGem5BwHandler;

    sc_core::sc_event fwEvent;
    sc_core::sc_event bwEvent;

    std::queue<transType> gem5ToTlmFwQueue;

    std::queue<transType> tlmToGem5BwQueue;

    void bwClock();
    void fwClock();
    void start(sc_core::sc_time);
    DebugManager* setupDebugger(bool _debugEnabled, bool _writeToConsole, bool _writeToFile);
    //virtual ~ScNodeTarget() noexcept;
  private:
    // static void logo();
    //std::unique_ptr<Passthrough> createScNode(bool);
    //void end_of_simulation() override;

    //std::unique_ptr<Passthrough> passthrough;
    // Transaction Recorders (one per channel).
    // They generate the output databases.
    // std::vector<TlmRecorder> tlmRecorders;


    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &payload,
                                       tlm::tlm_phase &phase,
                                       sc_core::sc_time &fwDelay);

    tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &payload,
                                       tlm::tlm_phase &phase,
                                       sc_core::sc_time &bwDelay);
/*
    void b_transport(tlm::tlm_generic_payload &payload,
                     sc_core::sc_time &delay);

    unsigned int transport_dbg(tlm::tlm_generic_payload &trans);
*/
    const char* scnode_name;
    DebugManager* debugger;
    // sc_module* scnode_instance;
    sc_core::sc_time   cycleTime;
    uint64_t simctx_id;
};

extern "C" {
  __attribute__((visibility("default"))) ScNodeTarget* make_sctarget(uint64_t, sc_core::sc_time, const std::string&, transFunc);
  __attribute__((visibility("default"))) void sctarget_start(ScNodeTarget* obj, sc_core::sc_time t);
  __attribute__((visibility("default"))) void sctarget_fwhandler(ScNodeTarget* obj, transType t);
  __attribute__((visibility("default"))) void sctarget_bwhandler(ScNodeTarget* obj, transType t);
  __attribute__((visibility("default"))) DebugManager* sctarget_debugger(ScNodeTarget* obj);
}


