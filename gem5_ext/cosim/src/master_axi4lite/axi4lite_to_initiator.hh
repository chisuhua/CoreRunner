/*
 * This is a small example showing howto connect an RTL AXI4Lite Device
 * to a SystemC/TLM simulation using the TLM-2-AXI4Lite bridge.
 *
 * Copyright (c) 2018 Xilinx Inc.
 * Written by Edgar E. Iglesias
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once
#include <sstream>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "checkers/pc-axilite.h"
#include "test-modules/signals-axilite.h"
#include "tlm-bridges/tlm2axilite-bridge.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

// #include "test-modules/utils.h"

#include "Vaxilite_dev.h"

// using namespace utils;



// Top simulation module.
SC_MODULE(Top)
{
        sc_clock clk;
        sc_signal<bool> rst_n; // Active low.

        AXILiteSignals<4, 32 > signals;
        tlm2axilite_bridge<4, 32 > bridge;
    AXILiteProtocolChecker<4, 32 > checker;
        // dut is the RTL AXI4Lite device we're testing.
        Vaxilite_dev dut;

        Top(sc_module_name name);
};
/*
int sc_main(int argc, char *argv[])
{
        Verilated::commandArgs(argc, argv);
        Top top("Top");

        sc_trace_file *trace_fp = sc_create_vcd_trace_file(argv[0]);

        top.signals.Trace(trace_fp);

        // Reset is active low. Emit a reset cycle.
        top.rst_n.write(false);
        sc_start(4, SC_US);
        top.rst_n.write(true);

        sc_start(140, SC_US);
        sc_stop();

        if (trace_fp) {
                sc_close_vcd_trace_file(trace_fp);
        }
        return 0;
}
*/
