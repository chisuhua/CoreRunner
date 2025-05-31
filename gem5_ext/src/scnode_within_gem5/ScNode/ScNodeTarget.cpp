#include "ScNodeTarget.h"
#include <functional>
#include "Passthrough.h"
#include "ScMemory.h"
#include "DebugManager.h"

ScNodeTarget::ScNodeTarget(
    sc_core::sc_module_name name,
    const char* scnode_name,
    sc_core::sc_time cycle_time,
    transFunc bw_handler,
    uint64_t simctx_id,
    bool recordable) :
    sc_core::sc_module(name),
    iSocket("initiator"),
    scnode_name(scnode_name),
    cycleTime(cycle_time),
    tlmToGem5BwHandler(bw_handler),
    simctx_id(simctx_id)
{
    if (strcmp(scnode_name, "ScMemory")==0) {
      auto instance = new ScMemory(scnode_name);
      iSocket.bind(instance->tSocket);
    } else {
      auto instance = new ScMemory(scnode_name);
      iSocket.bind(instance->tSocket);
      //throw std::runtime_error("scnode_name is not supported");
    }

    SC_METHOD(fwClock);
    sensitive << fwEvent;
    dont_initialize();

    SC_METHOD(bwClock);
    sensitive << bwEvent;
    dont_initialize();

    iSocket.register_nb_transport_bw(this, &ScNodeTarget::nb_transport_bw);
}

  /*
std::unique_ptr<sc_module> ScNodeTarget::createScNode(
    sc_module_name name,
    bool recordable)
{
    return std::make_unique<Passthrough>("ScMemory");
    return recordable
        ? std::make_shared<MemoryRecordable>("DRAMSys", config)
        : std::make_shared<Memory>("DRAMSys", config);
}
  */

// from gem5 to sc
void ScNodeTarget::gem5ToTlmFwHandler(transType trans) {
    DBGTRANS(__FUNCTION__,  trans.first, trans.second);
    gem5ToTlmFwQueue.push(trans);
};

void ScNodeTarget::fwClock() {
    if (isFullCycle(sc_time_stamp(), cycleTime)) {
        while( !gem5ToTlmFwQueue.empty()) {
            auto it = gem5ToTlmFwQueue.front();
            tlm::tlm_generic_payload* payload = it.first;
            tlm::tlm_phase phase = it.second;
            sc_core::sc_time t = SC_ZERO_TIME;
            DBGTRANS("ScNodeTarget Gem5ToTlmFw", it.first, it.second);
            iSocket->nb_transport_fw(*payload, phase, t);
            assert(ret = tlm::TLM_ACCEPTED);
            gem5ToTlmFwQueue.pop();
        }
    };
    if (!gem5ToTlmFwQueue.empty()) {
        fwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    }
}

// from sc to gem5
void ScNodeTarget::bwClock() {
    if (isFullCycle(sc_time_stamp(), cycleTime)) {
        while(!tlmToGem5BwQueue.empty()) {
            auto it = tlmToGem5BwQueue.front();
            DBGTRANS("ScNodeTarget TlmToGem5Bw Clock", it.first, it.second);
            tlmToGem5BwHandler(it);
            tlmToGem5BwQueue.pop();
        }
    }
    if (!tlmToGem5BwQueue.empty()) {
        bwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    }
}
/*
void ScNodeTarget::b_transport(
    tlm::tlm_generic_payload &payload,
    sc_core::sc_time &delay)
{
    iSocket->b_transport(payload, delay);
}
*/

// from sc to gem5 bw
tlm::tlm_sync_enum ScNodeTarget::nb_transport_bw(
    tlm::tlm_generic_payload &payload,
    tlm::tlm_phase &phase,
    sc_core::sc_time &bwDelay)
{
    DBGTRANS("ScNodeTarget isck ScToGem5 bw", &payload, phase);
    tlmToGem5BwQueue.push({&payload, phase});
    bwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    return tlm::TLM_ACCEPTED;
}


void ScNodeTarget::start(sc_time time) {
    sc_set_curr_simcontext(simctx_id);
    fwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    sc_time delta_time = time - sc_time_stamp();
    if (delta_time > SC_ZERO_TIME)  {
        sc_start(time - sc_time_stamp());
    }
}

DebugManager* ScNodeTarget::setupDebugger(bool enabled = true, bool write_to_console = true, bool write_to_file = false) {
    DebugManager* dbg = &DebugManager::getInstance();
    dbg->setup(enabled, write_to_console, write_to_file);
    return dbg;
}

extern "C" {
  ScNodeTarget* make_sctarget(uint64_t id, sc_time cycleTime, const std::string& name, transFunc bw_handler) {
    sc_set_curr_simcontext(id); 
    return new ScNodeTarget("ScNodeTarget", name.c_str(), cycleTime, bw_handler, id, false);
  }

  void sctarget_start(ScNodeTarget* obj, sc_time t) {
    obj->start(t);
  }
  void sctarget_fwhandler(ScNodeTarget* obj, transType t) {
    obj->gem5ToTlmFwHandler(t);
  }
  void sctarget_bwhandler(ScNodeTarget* obj, transType t) {
    obj->gem5ToTlmBwHandler(t);
  }
  DebugManager* sctarget_debugger(ScNodeTarget* obj) {
    return obj->setupDebugger();
  }
}

