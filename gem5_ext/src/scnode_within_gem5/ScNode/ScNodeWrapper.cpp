#include "ScNodeWrapper.h"
#include <functional>
#include "Passthrough.h"
#include "DebugManager.h"

ScNodeWrapper::ScNodeWrapper(
    sc_core::sc_module_name name,
    const char* scnode_name,
    sc_core::sc_time cycle_time,
    transFunc fw_handler,
    transFunc bw_handler,
    uint64_t simctx_id,
    bool recordable) :
    sc_core::sc_module(name),
    iSocket("initiator"),
    tSocket("target"),
    scnode_name(scnode_name),
    cycleTime(cycle_time),
    tlmToGem5FwHandler(fw_handler),
    tlmToGem5BwHandler(bw_handler),
    simctx_id(simctx_id)
{

    if (strcmp(scnode_name, "Passthrough")==0) {
      auto instance = new Passthrough(scnode_name);
      iSocket.bind(instance->tSocket);
      instance->iSocket.bind(tSocket);
    } else {
      assert(false);
      //throw std::runtime_error("scnode_name is not supported");
    }

    SC_METHOD(fwClock);
    sensitive << fwEvent;
    dont_initialize();

    SC_METHOD(bwClock);
    sensitive << bwEvent;
    dont_initialize();

    iSocket.register_nb_transport_bw(this, &ScNodeWrapper::nb_transport_bw);
    tSocket.register_nb_transport_fw(this, &ScNodeWrapper::nb_transport_fw);
}

  /*
std::unique_ptr<sc_module> ScNodeWrapper::createScNode(
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
void ScNodeWrapper::gem5ToTlmFwHandler(transType trans) {
    DBGTRANS(__FUNCTION__,  trans.first, trans.second);
    gem5ToTlmFwQueue.push(trans);
};

void ScNodeWrapper::gem5ToTlmBwHandler(transType trans) {
    DBGTRANS(__FUNCTION__,  trans.first, trans.second);
    gem5ToTlmBwQueue.push(trans);
};

void ScNodeWrapper::fwClock() {
    if (isFullCycle(sc_time_stamp(), cycleTime)) {
        while( !gem5ToTlmFwQueue.empty()) {
            auto it = gem5ToTlmFwQueue.front();
            tlm::tlm_generic_payload* payload = it.first;
            tlm::tlm_phase phase = it.second;
            sc_core::sc_time t = SC_ZERO_TIME;
            DBGTRANS("ScNodeWrapper Gem5ToTlmFw", it.first, it.second);
            iSocket->nb_transport_fw(*payload, phase, t);
            assert(ret = tlm::TLM_ACCEPTED);
            gem5ToTlmFwQueue.pop();
        }
        while( !gem5ToTlmBwQueue.empty()) {
            auto it = gem5ToTlmBwQueue.front();
            tlm::tlm_generic_payload* payload = it.first;
            tlm::tlm_phase phase = it.second;
            sc_core::sc_time t = SC_ZERO_TIME;
            DBGTRANS("ScNodeWrapper Gem5ToTlmBw", it.first, it.second);
            tSocket->nb_transport_bw(*payload, phase, t);
            assert(ret = tlm::TLM_ACCEPTED);
            gem5ToTlmBwQueue.pop();
        }
    };
    if (!gem5ToTlmFwQueue.empty() || !gem5ToTlmBwQueue.empty()) {
        fwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    }
}

// from sc to gem5
void ScNodeWrapper::bwClock() {
    if (isFullCycle(sc_time_stamp(), cycleTime)) {
        while(!tlmToGem5FwQueue.empty()) {
            auto it = tlmToGem5FwQueue.front();
            DBGTRANS("ScNodeWrapper TlmToGem5Fw Clock", it.first, it.second);
            tlmToGem5FwHandler(it);
            tlmToGem5FwQueue.pop();
        }
        while(!tlmToGem5BwQueue.empty()) {
            auto it = tlmToGem5BwQueue.front();
            DBGTRANS("ScNodeWrapper TlmToGem5Bw Clock", it.first, it.second);
            tlmToGem5BwHandler(it);
            tlmToGem5BwQueue.pop();
        }
    }
    if (!tlmToGem5FwQueue.empty() || !tlmToGem5BwQueue.empty()) {
        bwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    }
}
/*
void ScNodeWrapper::b_transport(
    tlm::tlm_generic_payload &payload,
    sc_core::sc_time &delay)
{
    iSocket->b_transport(payload, delay);
}
*/

// from sc to gem5 bw
tlm::tlm_sync_enum ScNodeWrapper::nb_transport_bw(
    tlm::tlm_generic_payload &payload,
    tlm::tlm_phase &phase,
    sc_core::sc_time &bwDelay)
{
    DBGTRANS("ScNodeWrapper isck ScToGem5 bw", &payload, phase);
    tlmToGem5BwQueue.push({&payload, phase});
    bwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    return tlm::TLM_ACCEPTED;
}

// from sc to gem5 fw
tlm::tlm_sync_enum ScNodeWrapper::nb_transport_fw(
    tlm::tlm_generic_payload &payload,
    tlm::tlm_phase &phase,
    sc_core::sc_time &bwDelay)
{
    DBGTRANS("ScNodeWrapper tsck ScToGem5 fw", &payload, phase);
    tlmToGem5FwQueue.push({&payload, phase});
    bwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    return tlm::TLM_ACCEPTED;
}

void ScNodeWrapper::start(sc_time time) {
    sc_set_curr_simcontext(simctx_id);
    fwEvent.notify(alignAtNext(sc_time_stamp(), cycleTime));
    sc_time delta_time = time - sc_time_stamp();
    if (delta_time > SC_ZERO_TIME)  {
        sc_start(time - sc_time_stamp());
    }
}

DebugManager* ScNodeWrapper::setupDebugger(bool enabled = true, bool write_to_console = true, bool write_to_file = false) {
    DebugManager* dbg = &DebugManager::getInstance();
    dbg->setup(enabled, write_to_console, write_to_file);
    return dbg;
}

extern "C" {
  ScNodeWrapper* make_scnode(uint64_t id, sc_time cycleTime, const std::string& name, transFunc fw_handler, transFunc bw_handler) {
    sc_set_curr_simcontext(id); 
    return new ScNodeWrapper("ScNodeWrapper", name.c_str(), cycleTime, fw_handler, bw_handler, id, false);
  }

  void scnode_start(ScNodeWrapper* obj, sc_time t) {
    obj->start(t);
  }
  void scnode_fwhandler(ScNodeWrapper* obj, transType t) {
    obj->gem5ToTlmFwHandler(t);
  }
  void scnode_bwhandler(ScNodeWrapper* obj, transType t) {
    obj->gem5ToTlmBwHandler(t);
  }
  DebugManager* scnode_debugger(ScNodeWrapper* obj) {
    return obj->setupDebugger();
  }
}

int sc_main(int argc, char* argv[]) {
    //ScNodeWrapper scnose("scnose", false);
    sc_core::sc_start();
    return 0;
}
