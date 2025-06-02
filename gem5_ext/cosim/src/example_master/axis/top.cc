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

#include "top.hh"


axis_bridge::axis_bridge(sc_module_name name) :
            clk("clk"),
            rst_n("rst_n"),
            slave_signals("slave_signals"),
            master_signals("master_signals"),
            slave_bridge("slave_bridge"),
            master_bridge("master_bridge"),
            dut("dut")
{
        // Wire up the clock and reset signals.
        slave_bridge.clk(clk);
        slave_bridge.resetn(rst_n);

        master_bridge.clk(clk);
        master_bridge.resetn(rst_n);
        // Wire-up the slave_bridge and checker.
        slave_signals.connect(slave_bridge);
        // Wire-up the master_bridge and checker.
        master_signals.connect(master_bridge);

        //
        dut.s_tvalid(slave_signals.tvalid);
        dut.s_tready(slave_signals.tready);
        dut.s_tdata(slave_signals.tdata);
        dut.s_tstrb(slave_signals.tstrb);
        dut.s_tlast(slave_signals.tlast);

        dut.m_tvalid(master_signals.tvalid);
        dut.m_tready(master_signals.tready);
        dut.m_tdata(master_signals.tdata);
        dut.m_tstrb(master_signals.tstrb);
        dut.m_tlast(master_signals.tlast);
}

Top::Top(sc_module_name name, int bridge_num) :
        clk("clk", sc_time(1, SC_NS)),
        rst_n("rst_n")
{
    std::stringstream bridge_name;
    for (int i=0; i< bridge_num; i++) {
        bridge_name.str("");
        bridge_name << "bridge" << i;
        bridge.push_back(new axis_bridge(bridge_name.str().c_str()));
        bridge[i]->clk(clk);
        bridge[i]->rst_n(rst_n);
    }
}
