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

class ScNodeWrapper;
class DebugManager;

typedef ScNodeWrapper* (*create_scnode_t)(uint64_t, sc_core::sc_time, const std::string&, transFunc, transFunc);
typedef void (*scnode_start_t)(ScNodeWrapper* obj, sc_core::sc_time t);
typedef void (*scnode_fwhandler_t)(ScNodeWrapper* obj, transType t);
typedef void (*scnode_bwhandler_t)(ScNodeWrapper* obj, transType t);
typedef DebugManager* (*scnode_debugger_t)(ScNodeWrapper* obj);


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

class ScNodeWrapper : public sc_core::sc_module
{
  public:
    tlm_utils::simple_initiator_socket<ScNodeWrapper> iSocket;
    tlm_utils::simple_target_socket<ScNodeWrapper> tSocket;

    //SC_HAS_PROCESS(ScNodeWrapper);
    explicit ScNodeWrapper(sc_core::sc_module_name name,
                  const char* scnode_name,
                  sc_core::sc_time cycle_time,
                  transFunc fw_handler,
                  transFunc bw_handler,
                  uint64_t simctx_id,
                  bool recordable);

    void gem5ToTlmBwHandler(transType);
    void gem5ToTlmFwHandler(transType);
    transFunc tlmToGem5FwHandler;
    transFunc tlmToGem5BwHandler;

    sc_core::sc_event fwEvent;
    sc_core::sc_event bwEvent;

    std::queue<transType> gem5ToTlmFwQueue;
    std::queue<transType> gem5ToTlmBwQueue;

    std::queue<transType> tlmToGem5FwQueue;
    std::queue<transType> tlmToGem5BwQueue;

    void bwClock();
    void fwClock();
    void start(sc_core::sc_time);
    DebugManager* setupDebugger(bool _debugEnabled, bool _writeToConsole, bool _writeToFile);
    //virtual ~ScNodeWrapper() noexcept;
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
  __attribute__((visibility("default"))) ScNodeWrapper* make_scnode(uint64_t, sc_core::sc_time, const std::string&, transFunc, transFunc);
  __attribute__((visibility("default"))) void scnode_start(ScNodeWrapper* obj, sc_core::sc_time t);
  __attribute__((visibility("default"))) void scnode_fwhandler(ScNodeWrapper* obj, transType t);
  __attribute__((visibility("default"))) void scnode_bwhandler(ScNodeWrapper* obj, transType t);
  __attribute__((visibility("default"))) DebugManager* scnode_debugger(ScNodeWrapper* obj);
}


