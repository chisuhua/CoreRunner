#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "DebugManager.h"

using namespace sc_core;
using namespace tlm;
using namespace std;

// === Passthrough 模块 ===
class Passthrough : public sc_module {
public:
    tlm_utils::simple_target_socket<Passthrough> tSocket;
    tlm_utils::simple_initiator_socket<Passthrough> iSocket;

    SC_CTOR(Passthrough) : tSocket("target"), iSocket("initiator") {
        tSocket.register_nb_transport_fw(this, &Passthrough::nb_transport_fw);
        iSocket.register_nb_transport_bw(this, &Passthrough::nb_transport_bw);
    }

    // Non-blocking transport handler (forward)
    tlm_sync_enum nb_transport_fw(tlm_generic_payload& trans, tlm_phase& phase, sc_time& delay) {
        DBGTRANS("Passthrough begin fw", &trans, phase);

        // 将当前 phase 和事务转发给下游模块
        tlm_phase fw_phase = phase;
        tlm_sync_enum status = iSocket->nb_transport_fw(trans, fw_phase, delay);

        DBGTRANS("Passthrough done fw", &trans, phase);

        if (status == TLM_UPDATED) {
            phase = fw_phase;
        }

        return status;
    }

    // Backward transport: 从 Target 到 Initiator
    tlm_sync_enum nb_transport_bw(tlm_generic_payload& trans, tlm_phase& phase, sc_time& delay) {
        DBGTRANS("Passthrough begin bw", &trans, phase);

        tlm_phase bw_phase = phase;
        tlm_sync_enum status = tSocket->nb_transport_bw(trans, bw_phase, delay);

        DBGTRANS("Passthrough done bw", &trans, phase);

        if (status == TLM_UPDATED)
            phase = bw_phase;

        return status;
    }
};

/*
// === Target 模块 ===
class Target : public sc_module {
public:
    tlm_utils::simple_tSocket<Target> tSocket;

    SC_CTOR(Target) : tSocket("target") {
        tSocket.register_nb_transport_bw(this, &Target::nb_transport_bw);
    }

    // Non-blocking transport handler (backward)
    tlm_sync_enum nb_transport_bw(tlm_generic_payload& trans, tlm_phase& phase, sc_time& delay) {
        cout << "Target: Received transaction at " << sc_time_stamp() << endl;

        if (phase == BEGIN_REQ) {
            // 模拟处理延迟
            wait(delay);

            // 设置响应状态
            trans.set_response_status(TLM_OK_RESPONSE);

            // 返回 END_RESP 相位
            phase = END_RESP;
            return TLM_UPDATED;
        }

        return TLM_COMPLETED;
    }
};

// === Initiator 模块 ===
class Initiator : public sc_module {
public:
    tlm_utils::simple_iSocket<Initiator> iSocket;
    sc_event end_simulation_event;

    SC_CTOR(Initiator) : iSocket("initiator") {
        SC_THREAD(main_thread);
    }

    void main_thread() {
        for (int i = 0; i < 5; ++i) {
            tlm_generic_payload trans;
            sc_time delay = sc_time(10, SC_NS);
            tlm_phase phase = BEGIN_REQ;

            trans.set_command(TLM_READ_COMMAND);
            trans.set_address(i * 4);
            trans.set_data_ptr(new uint8_t[4]{0});
            trans.set_data_length(4);
            trans.set_streaming_width(4);
            trans.set_byte_enable_ptr(0);
            trans.set_dmi_allowed(false);
            trans.set_response_status(TLM_INCOMPLETE_RESPONSE);

            // 发送事务
            tlm_sync_enum status = iSocket->nb_transport(trans, phase, delay);

            if (status == TLM_UPDATED) {
                // 等待响应
                while (phase != END_RESP) {
                    wait(1, SC_NS);
                    status = iSocket->nb_transport(trans, phase, delay);
                }

                if (trans.is_response_error()) {
                    cout << "Error in transaction at " << sc_time_stamp() << endl;
                } else {
                    cout << "Transaction successful at " << sc_time_stamp() << endl;
                }

                delete[] trans.get_data_ptr();
            }

            wait(10, SC_NS);
        }

        end_simulation_event.notify();  // 结束模拟
    }
};

// === 测试平台 ===
int sc_main(int argc, char* argv[]) {
    Initiator initiator("initiator");
    Passthrough passthrough("passthrough");
    Target target("target");

    initiator.iSocket.bind(passthrough.tSocket);
    passthrough.iSocket.bind(target.tSocket);

    sc_start(100, SC_NS);  // 运行 100ns

    return 0;
}
*/
