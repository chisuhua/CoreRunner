#pragma once
#include <iomanip>
#include <iostream>
#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>

using namespace tlm;
using namespace std;

class ScMemory : public sc_module {
public:
    tlm_utils::simple_target_socket<ScMemory> tSocket;

    unsigned char *mem;

    SC_CTOR(ScMemory) : tSocket("socket") {
        tSocket.register_b_transport(this, &ScMemory::b_transport);
        mem = (unsigned char *)malloc(16*1024*1024);
        for (int i = 0; i < 256; ++i) {
            mem[i] = 0;
        }
    }
/*
    void blocking_transport(tlm::tlm_generic_payload& payload, sc_time& t) {
        unsigned char* data_ptr = payload.get_data_ptr();
        sc_dt::uint64 addr = payload.get_address();
        unsigned int len = payload.get_data_length();

        if (payload.is_write()) {
            for (unsigned int i = 0; i < len; ++i) {
                memory[addr + i] = data_ptr[i];
            }
        } else if (payload.is_read()) {
            for (unsigned int i = 0; i < len; ++i) {
                data_ptr[i] = memory[addr + i];
            }
        }

        payload.set_response_status(TLM_OK_RESPONSE);
    }
*/
    void b_transport(tlm::tlm_generic_payload& trans, sc_time& t)
    {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    adr = trans.get_address();
        unsigned char*   ptr = trans.get_data_ptr();
        unsigned int     len = trans.get_data_length();
        unsigned char*   byt = trans.get_byte_enable_ptr();

        if (trans.get_address() >= 16*1024*1024) {
            cout << "\033[1;31m("
                << "Address Error"
                << "\033[0m" << endl;
            trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
            return;
        }
        if (byt != 0) {
            cout << "\033[1;31m("
                << "Byte Enable Error"
                << "\033[0m" << endl;
            trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE );
            return;
        }
        if (len < 1 || len > 64) {
            cout << "\033[1;31m("
                << "Burst Error"
                << "\033[0m" << endl;
            trans.set_response_status( tlm::TLM_BURST_ERROR_RESPONSE );
            return;
        }

        if (cmd == tlm::TLM_READ_COMMAND)
        {
            memcpy(mem+trans.get_address(), // destination
                    trans.get_data_ptr(),      // source
                    trans.get_data_length());  // size
        }
        else if (cmd == tlm::TLM_WRITE_COMMAND)
        {
            memcpy(trans.get_data_ptr(),      // destination
                    mem + trans.get_address(), // source
                    trans.get_data_length());  // size
        }

        cout << "\033[1;32m("
                << name()
                << ")@"  << setfill(' ') << setw(12) << sc_time_stamp()
                << ": " << setw(12) << (cmd ? "Exec. Write " : "Exec. Read ")
                << "Addr = " << setfill('0') << setw(8) << hex << adr
                << " Data = " << "0x" << setfill('0') << setw(8) << hex
                << *reinterpret_cast<int*>(ptr)
                << "\033[0m" << endl;

        trans.set_response_status( tlm::TLM_OK_RESPONSE );
    }

};
