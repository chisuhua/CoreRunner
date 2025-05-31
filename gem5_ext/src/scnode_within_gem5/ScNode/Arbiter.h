#ifndef ARBITER_H
#define ARBITER_H

//#include "DRAMSys/common/dramExtensions.h"

#include <iostream>
#include <queue>
#include <set>
#include <systemc>
#include <tlm>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>
#include <vector>

DECLARE_EXTENDED_PHASE(REQ_ARBITRATION);
DECLARE_EXTENDED_PHASE(RESP_ARBITRATION);

class Arbiter : public sc_core::sc_module
{
public:
    tlm_utils::multi_passthrough_initiator_socket<Arbiter> iSocket;
    tlm_utils::multi_passthrough_target_socket<Arbiter> tSocket;

protected:
    Arbiter(const sc_core::sc_module_name& name);
    SC_HAS_PROCESS(Arbiter);

    void end_of_elaboration() override;

    tlm_utils::peq_with_cb_and_phase<Arbiter> payloadEventQueue;
    virtual void peqCallback(tlm::tlm_generic_payload& payload, const tlm::tlm_phase& phase) = 0;

    ControllerVector<Thread, bool> threadIsBusy;
    ControllerVector<Channel, bool> channelIsBusy;

    ControllerVector<Channel, std::queue<tlm::tlm_generic_payload*>> pendingRequestsOnChannel;

    ControllerVector<Thread, std::uint64_t> nextThreadPayloadIDToAppend;
    ControllerVector<Channel, std::uint64_t> nextChannelPayloadIDToAppend;

    tlm::tlm_sync_enum nb_transport_fw(int id,
                                       tlm::tlm_generic_payload& trans,
                                       tlm::tlm_phase& phase,
                                       sc_core::sc_time& fwDelay);
    tlm::tlm_sync_enum nb_transport_bw(int id,
                                       tlm::tlm_generic_payload& payload,
                                       tlm::tlm_phase& phase,
                                       sc_core::sc_time& bwDelay);
    void b_transport(int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);
    unsigned int transport_dbg(int id, tlm::tlm_generic_payload& trans);

    const sc_core::sc_time tCK;
    const sc_core::sc_time arbitrationDelayFw;
    const sc_core::sc_time arbitrationDelayBw;

    const unsigned bytesPerBeat;
    const uint64_t addressOffset;
};

class ArbiterSimple final : public Arbiter
{
public:
    ArbiterSimple(const sc_core::sc_module_name& name);
    SC_HAS_PROCESS(ArbiterSimple);

private:
    void end_of_elaboration() override;
    void peqCallback(tlm::tlm_generic_payload& cbTrans, const tlm::tlm_phase& phase) override;

    ControllerVector<Thread, std::queue<tlm::tlm_generic_payload*>> pendingResponsesOnThread;
};

/*
class ArbiterFifo final : public Arbiter
{
public:
    ArbiterFifo(const sc_core::sc_module_name& name);
    SC_HAS_PROCESS(ArbiterFifo);

private:
    void end_of_elaboration() override;
    void peqCallback(tlm::tlm_generic_payload& cbTrans, const tlm::tlm_phase& phase) override;

    ControllerVector<Thread, unsigned int> activeTransactionsOnThread;
    const unsigned maxActiveTransactionsPerThread;

    ControllerVector<Thread, tlm::tlm_generic_payload*> outstandingEndReqOnThread;
    ControllerVector<Thread, std::queue<tlm::tlm_generic_payload*>> pendingResponsesOnThread;

    ControllerVector<Channel, sc_core::sc_time> lastEndReqOnChannel;
    ControllerVector<Thread, sc_core::sc_time> lastEndRespOnThread;
};

class ArbiterReorder final : public Arbiter
{
public:
    ArbiterReorder(const sc_core::sc_module_name& name);
    SC_HAS_PROCESS(ArbiterReorder);

private:
    void end_of_elaboration() override;
    void peqCallback(tlm::tlm_generic_payload& cbTrans, const tlm::tlm_phase& phase) override;

    ControllerVector<Thread, unsigned int> activeTransactionsOnThread;
    const unsigned maxActiveTransactions;

    struct ThreadPayloadIDCompare
    {
        bool operator()(const tlm::tlm_generic_payload* lhs,
                        const tlm::tlm_generic_payload* rhs) const
        {
            return ArbiterExtension::getThreadPayloadID(*lhs) <
                   ArbiterExtension::getThreadPayloadID(*rhs);
        }
    };

    ControllerVector<Thread, tlm::tlm_generic_payload*> outstandingEndReqOnThread;
    ControllerVector<Thread, std::set<tlm::tlm_generic_payload*, ThreadPayloadIDCompare>>
        pendingResponsesOnThread;

    ControllerVector<Channel, sc_core::sc_time> lastEndReqOnChannel;
    ControllerVector<Thread, sc_core::sc_time> lastEndRespOnThread;

    ControllerVector<Thread, std::uint64_t> nextThreadPayloadIDToReturn;
};
*/


#endif // ARBITER_H
