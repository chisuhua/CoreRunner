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

#include "target_to_axi4lite.hh"

// using namespace utils;

#define CONNECT_DUT(DUT, SIGS, SIGNAME) DUT.s00_axi_ ## SIGNAME(SIGS.SIGNAME)


AXILitePCConfig checker_config()
{
        AXILitePCConfig cfg;

        cfg.enable_all_checks();

        return cfg;
}

Top::Top(sc_module_name name) :
            clk("clk", sc_time(1, SC_NS)),
            rst_n("rst_n"),
            signals("signals"),
            bridge("bridge"),
            checker("checker", checker_config()),
            dut("dut")
{

        // Wire up the clock and reset signals.
        bridge.clk(clk);
        bridge.resetn(rst_n);
        checker.clk(clk);
        checker.resetn(rst_n);
        dut.s00_axi_aclk(clk);
        dut.s00_axi_aresetn(rst_n);

        // Wire-up the bridge and checker.
        signals.connect(bridge);
        signals.connect(checker);

        //
        // Since the AXILite Dut doesn't use the same naming
        // conventions as AXILiteSignals, we need to manually connect
        // everything.
        //
        dut.s00_axi_awvalid(signals.awvalid);
        dut.s00_axi_awready(signals.awready);
        dut.s00_axi_awaddr(signals.awaddr);
        dut.s00_axi_awprot(signals.awprot);

        dut.s00_axi_arvalid(signals.arvalid);
        dut.s00_axi_arready(signals.arready);
        dut.s00_axi_araddr(signals.araddr);
        dut.s00_axi_arprot(signals.arprot);

        dut.s00_axi_wvalid(signals.wvalid);
        dut.s00_axi_wready(signals.wready);
        dut.s00_axi_wdata(signals.wdata);
        dut.s00_axi_wstrb(signals.wstrb);

        dut.s00_axi_bvalid(signals.bvalid);
        dut.s00_axi_bready(signals.bready);
        dut.s00_axi_bresp(signals.bresp);

        dut.s00_axi_rvalid(signals.rvalid);
        dut.s00_axi_rready(signals.rready);
        dut.s00_axi_rdata(signals.rdata);
        dut.s00_axi_rresp(signals.rresp);
}

