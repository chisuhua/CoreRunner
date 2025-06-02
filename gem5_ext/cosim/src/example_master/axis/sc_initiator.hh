#ifndef __TRAFFIC_GENERATOR_HH__
#define __TRAFFIC_GENERATOR_HH__

// #include <tlm_utils/peq_with_cb_and_phase.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_get.h>

#include <systemc>
#include <tlm>

#include "sc_mm.hh"
#include "top.hh"

class Initiator : public sc_core::sc_module
{

    typedef tlm_utils::simple_initiator_socket<Initiator> InitiatorSocketType;
    typedef tlm_utils::simple_target_socket<Initiator> TargetSocketType;

    struct ConnectionInfo {
        TargetSocketType* from;
        InitiatorSocketType* to;
    };

    typedef std::map<tlm::tlm_generic_payload*, ConnectionInfo> PendingTransType;
    typedef typename PendingTransType::iterator PendingTransIterator;
    typedef typename PendingTransType::const_iterator PendingTransConstIterator;

  private:
    // Gem5SystemC::MemoryManager mm;

    PendingTransType  pendingTrans;

    sc_core::sc_event beginRequestEvent;
    sc_core::sc_event endRequestEvent;

    sc_core::sc_event beginResponseEvent;
    sc_core::sc_event endResponseEvent;

    tlm_utils::peq_with_get<tlm::tlm_generic_payload> request_peq;
    tlm_utils::peq_with_get<tlm::tlm_generic_payload> response_peq;

    void addPendingTransaction(tlm::tlm_generic_payload& trans) {
        const ConnectionInfo info = { &target_socket, &initiator_socket};
        assert(pendingTrans.find(&trans) == pendingTrans.end());
        pendingTrans[&trans] = info;
    }

  public:
    axis_bridge &bridge;
    InitiatorSocketType initiator_socket;
    TargetSocketType target_socket;

    SC_HAS_PROCESS(Initiator);


    Initiator(sc_core::sc_module_name name, axis_bridge &bridge);

    void request_process();
    void response_process();


    void checkTransaction(tlm::tlm_generic_payload& trans);

    tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans,
                                               tlm::tlm_phase& phase,
                                               sc_core::sc_time& delay);

    tlm::tlm_sync_enum b_transport(tlm::tlm_generic_payload& trans,
                                               tlm::tlm_phase& phase,
                                               sc_core::sc_time& delay);


    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans,
                                           tlm::tlm_phase& phase,
                                           sc_core::sc_time& delay);
};

#endif
