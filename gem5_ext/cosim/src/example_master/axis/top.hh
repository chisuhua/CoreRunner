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

// #include "checkers/pc-axilite.h"
#include "test-modules/signals-axis.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm-bridges/tlm2axis-bridge.h"
#include "tlm-bridges/axis2tlm-bridge.h"

// #include "test-modules/utils.h"

#include "Vaxis_join.h"

// using namespace utils;
#define AXI_DATA_WIDTH 32


// Top simulation module.
SC_MODULE(axis_bridge)
{
public:
    sc_in<bool> clk;
    sc_in<bool> rst_n; // Active low.

    AXISSignals<AXI_DATA_WIDTH > slave_signals;
    tlm2axis_bridge<AXI_DATA_WIDTH > slave_bridge;

    AXISSignals<AXI_DATA_WIDTH > master_signals;
    axis2tlm_bridge<AXI_DATA_WIDTH > master_bridge;

public:
    // dut is the RTL AXI4Lite device we're testing.
    Vaxis_join dut;
    axis_bridge(sc_module_name name);
};

SC_MODULE(Top)
{
public:
    sc_clock clk;
    sc_signal<bool> rst_n; // Active low.


    // dut is the RTL AXI4Lite device we're testing.
    std::vector<axis_bridge*> bridge;

    Top(sc_module_name name, int bridge_num);
};

