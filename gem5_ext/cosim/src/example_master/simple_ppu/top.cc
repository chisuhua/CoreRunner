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

// using namespace utils;

#define CONNECT_DUT(DUT, SIGS, SIGNAME) DUT.s00_axi_ ## SIGNAME(SIGS.SIGNAME)


AXILitePCConfig checker_config()
{
        AXILitePCConfig cfg;

        cfg.enable_all_checks();

        return cfg;
}

axi_bridge::axi_bridge(sc_module_name name) :
            clk("clk"),
            rst_n("rst_n"),
            slave_signals("slave_signals"),
            master_signals("master_signals"),
            slave_bridge("slave_bridge", false),    // disable aligner
            master_bridge("master_bridge"),
            master_checker("master_checker", checker_config()),
            slave_checker("slave_checker", checker_config()),
            dut("dut")
{
        // Wire up the clock and reset signals.
        slave_bridge.clk(clk);
        slave_bridge.resetn(rst_n);

        master_bridge.clk(clk);
        master_bridge.resetn(rst_n);

        slave_checker.clk(clk);
        slave_checker.resetn(rst_n);

        master_checker.clk(clk);
        master_checker.resetn(rst_n);

        dut.s00_axi_aclk(clk);
        dut.s00_axi_aresetn(rst_n);

        // dut.m00_axi_aclk(clk);
        // dut.m00_axi_aresetn(rst_n);

        // Wire-up the slave_bridge and checker.
        slave_signals.connect(slave_bridge);
        slave_signals.connect(slave_checker);

        // Wire-up the master_bridge and checker.
        master_signals.connect(master_bridge);
        master_signals.connect(master_checker);


        //
        // Since the AXILite Dut doesn't use the same naming
        // conventions as AXILiteSignals, we need to manually connect
        // everything.
        //
        dut.s00_axi_awvalid(slave_signals.awvalid);
        dut.s00_axi_awready(slave_signals.awready);
        dut.s00_axi_awaddr(slave_signals.awaddr);
        dut.s00_axi_awprot(slave_signals.awprot);

        dut.s00_axi_wvalid(slave_signals.wvalid);
        dut.s00_axi_wready(slave_signals.wready);
        dut.s00_axi_wdata(slave_signals.wdata);
        dut.s00_axi_wstrb(slave_signals.wstrb);

        dut.s00_axi_bvalid(slave_signals.bvalid);
        dut.s00_axi_bready(slave_signals.bready);
        dut.s00_axi_bresp(slave_signals.bresp);


        dut.s00_axi_arvalid(slave_signals.arvalid);
        dut.s00_axi_arready(slave_signals.arready);
        dut.s00_axi_araddr(slave_signals.araddr);
        dut.s00_axi_arprot(slave_signals.arprot);

        dut.s00_axi_rvalid(slave_signals.rvalid);
        dut.s00_axi_rready(slave_signals.rready);
        dut.s00_axi_rdata(slave_signals.rdata);
        dut.s00_axi_rresp(slave_signals.rresp);

        dut.m00_axi_awvalid(master_signals.awvalid);
        dut.m00_axi_awready(master_signals.awready);
        dut.m00_axi_awaddr(master_signals.awaddr);
        dut.m00_axi_awprot(master_signals.awprot);

        dut.m00_axi_wvalid(master_signals.wvalid);
        dut.m00_axi_wready(master_signals.wready);
        dut.m00_axi_wdata(master_signals.wdata);
        dut.m00_axi_wstrb(master_signals.wstrb);

        dut.m00_axi_bvalid(master_signals.bvalid);
        dut.m00_axi_bready(master_signals.bready);
        dut.m00_axi_bresp(master_signals.bresp);


        dut.m00_axi_arvalid(master_signals.arvalid);
        dut.m00_axi_arready(master_signals.arready);
        dut.m00_axi_araddr(master_signals.araddr);
        dut.m00_axi_arprot(master_signals.arprot);

        dut.m00_axi_rvalid(master_signals.rvalid);
        dut.m00_axi_rready(master_signals.rready);
        dut.m00_axi_rdata(master_signals.rdata);
        dut.m00_axi_rresp(master_signals.rresp);
}

Top::Top(sc_module_name name, int bridge_num) :
        clk("clk", sc_time(1, SC_NS)),
        rst_n("rst_n")
{
    for (int i=0; i< bridge_num; i++) {
        bridge.push_back(new axi_bridge("bridge" + i));
        bridge[i]->clk(clk);
        bridge[i]->rst_n(rst_n);
    }
}
