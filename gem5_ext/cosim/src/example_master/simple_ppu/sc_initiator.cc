// use tlm2.0 example SimpleBusAT.h as reference

#include "sc_initiator.hh"
#include "top.hh"

Initiator::Initiator(sc_core::sc_module_name name, axi_bridge &bridge)
  : sc_core::sc_module(name),
    bridge(bridge),
    target_socket("target_socket"),
    initiator_socket("initiator_socket"),
    request_peq("request_peq"),
    response_peq("response_peq")
{
    initiator_socket.register_nb_transport_bw(this, &Initiator::nb_transport_bw);
    // initiator_socket.register_b_transport(this, &Initiator::b_transport);

    // target_socket.bind(top.master_bridge.socket);
    // top.bridge.socket.bind(target_socket);
    bridge.master_bridge.socket.bind(target_socket);
    target_socket.register_nb_transport_fw(this, &Initiator::nb_transport_fw);

    SC_THREAD(request_process);
    SC_THREAD(response_process);
}

/* TLM-2 non-blocking transport method */
tlm::tlm_sync_enum Initiator::nb_transport_fw(tlm::tlm_generic_payload& trans,
                                           tlm::tlm_phase& phase,
                                           sc_time& delay)
{
    if (phase == tlm::BEGIN_REQ) {
        trans.acquire();
        addPendingTransaction(trans);

        request_peq.notify(trans, delay);
    } else if (phase == tlm::END_RESP) {
        endResponseEvent.notify(delay);
        checkTransaction(trans);

        SC_REPORT_INFO("sc_initiator", "fw request completed");
        return tlm::TLM_COMPLETED;
    } else {
        std::stringstream ss;
        ss  << "ERROR: " << name()
                  << ":Illegal phase received from iniitiator."
                  << std::endl;
        SC_REPORT_INFO("Initiator fw ", ss.str().c_str());
        assert(false); exit(1);
    }

    std::stringstream ss;
    ss  << "Send "                // << cmdStr << " request @0x" << std::hex
        << trans.get_address();
    SC_REPORT_INFO("Initiator fw ", ss.str().c_str());
}
tlm::tlm_sync_enum
Initiator::b_transport(tlm::tlm_generic_payload& trans,
                                  tlm::tlm_phase& phase,
                                  sc_core::sc_time& delay)
{
}

tlm::tlm_sync_enum
Initiator::nb_transport_bw(tlm::tlm_generic_payload& trans,
                                  tlm::tlm_phase& phase,
                                  sc_core::sc_time& delay)
{
    if (phase != tlm::END_REQ && phase != tlm::BEGIN_RESP) {
          std::cout << "ERROR: '" << name()
               << "': Illegal phase received from target." << std::endl;
          assert(false); exit(1);
    }

    endRequestEvent.notify(delay);
    if (phase == tlm::BEGIN_RESP) {
        response_peq.notify(trans, delay);
    }

    return tlm::TLM_ACCEPTED;
}


void Initiator::request_process()
{
    while (true) {
        wait(request_peq.get_event());

        tlm::tlm_generic_payload* trans;

        while ((trans = request_peq.get_next_transaction()) != 0) {
            // Fill in the destionation port
            PendingTransIterator it = pendingTrans.find(trans);
            assert(it != pendingTrans.end());
            it->second.to = &initiator_socket;

            tlm::tlm_phase phase = tlm::BEGIN_REQ;
            auto delay = sc_core::SC_ZERO_TIME;

            auto status = initiator_socket->nb_transport_fw(*trans, phase, delay);

            switch (status) {
                case tlm::TLM_ACCEPTED:
                case tlm::TLM_UPDATED:
                    // Transcation not yet finished
                    if (phase == tlm::BEGIN_REQ) {
                        // request phase not yet finished
                        wait(endRequestEvent);
                    } else if (phase == tlm::END_REQ) {
                        // request phase finished , but response phase not yet started
                        wait(delay);
                    } else if (phase == tlm::BEGIN_RESP) {
                        checkTransaction(*trans);
                        SC_REPORT_INFO("SC_initiator", "received response");
                        response_peq.notify(*trans, delay);
                        // Not needed to send END_REQ to initiator
                        continue;
                    } else { // END_RESP
                        assert(0); exit(1);
                    }

                    // only send END_REQ to initiator if BEGIN_kk:w
                    if (it->second.from) {
                        phase = tlm::END_REQ;
                        (*it->second.from)->nb_transport_bw(*trans, phase, delay);
                    }
                    break;

                case tlm::TLM_COMPLETED:
                    // transcation finished
                    response_peq.notify(*trans, delay);

                    // reset the destination port(we must not send END_RESP to target
                    it->second.to = 0;

                    wait(delay);
                    break;

                default:
                    assert(0); exit(1);
            };
        }
    }
}

void Initiator::response_process()
{
    while (true) {
        wait(response_peq.get_event());

        tlm::tlm_generic_payload* trans;

        while ((trans = response_peq.get_next_transaction()) != 0) {
            PendingTransIterator it = pendingTrans.find(trans);
            assert(it != pendingTrans.end());

            tlm::tlm_phase phase = tlm::BEGIN_RESP;
            auto delay = sc_core::SC_ZERO_TIME;

            TargetSocketType* target_socket = it->second.from;
            // if BEGIN_RESP is end first we don't have to send END_REQ anymore
            it->second.from = 0;

            // BEGIN_RESP
            auto status = (*target_socket)->nb_transport_bw(*trans, phase, delay);

            switch (status) {
                case tlm::TLM_COMPLETED:
                    // transcation finished
                    wait(delay);
                    break;

                case tlm::TLM_ACCEPTED:
                case tlm::TLM_UPDATED:
                    // Transcation not yet finished
                    wait(endResponseEvent);
                    break;

                default:
                    assert(0); exit(1);
            };

            // foware END_RESP to target
            if (it->second.to) {
                phase = tlm::END_RESP;
                tlm::tlm_sync_enum r = (*it->second.to)->nb_transport_fw(*trans, phase, delay);
                assert(r == tlm::TLM_COMPLETED);
                (void)r;
            }

            pendingTrans.erase(it);
            trans->release();
        }
    }
}



void
Initiator::checkTransaction(tlm::tlm_generic_payload& trans)
{
    if (trans.is_response_error()) {
        std::stringstream ss;
        ss << "Transaction returned with error, response status = %s"
           << trans.get_response_string();
        SC_REPORT_ERROR("TLM-2", ss.str().c_str());
    }
}


