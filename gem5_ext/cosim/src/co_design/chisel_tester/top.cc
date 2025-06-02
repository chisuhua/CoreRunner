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

#define CONNECT(SIGNAME) dut.s_axi_ ## SIGNAME(slave_signals.SIGNAME); \
   dut.m_axi_ ## SIGNAME(master_signals.SIGNAME);

/*
AXILitePCConfig checker_config()
{
        AXILitePCConfig cfg;

        cfg.enable_all_checks();

        return cfg;
}
*/

axi_bridge::axi_bridge(sc_module_name name) :
            clk("clk"),
            rst_n("rst_n"),
            slave_signals("slave_signals"),
            slave_bridge("slave_bridge", V_AXI4, false), // no aligner
            master_signals("master_signals"),
            master_bridge("master_bridge", V_AXI4),
            dut("dut")
            // master_checker("master_checker", checker_config()),
            // slave_checker("slave_checker", checker_config()),
{
        // Wire up the clock and reset signals.
        slave_bridge.clk(clk);
        slave_bridge.resetn(rst_n);

        master_bridge.clk(clk);
        master_bridge.resetn(rst_n);

        // slave_checker.clk(clk);
        // slave_checker.resetn(rst_n);

        // master_checker.clk(clk);
        // master_checker.resetn(rst_n);

        dut.axi_aclk(clk);
        dut.axi_aresetn(rst_n);

        // dut.m00_axi_aclk(clk);
        // dut.m00_axi_aresetn(rst_n);

        // Wire-up the slave_bridge and checker.
        slave_signals.connect(slave_bridge);
        // slave_signals.connect(slave_checker);

        // Wire-up the master_bridge and checker.
        master_signals.connect(master_bridge);
        // master_signals.connect(master_checker);

        // slave_signals.connect(dut, "s_axi_");
        // master_signals.connect(dut, "m_axi_");
        CONNECT(awvalid)
        CONNECT(awready)
        CONNECT(awaddr)
        CONNECT(awprot)
        CONNECT(awid)
        CONNECT(awlen)
        CONNECT(awsize)
        CONNECT(awburst)
        CONNECT(awlock)
        CONNECT(awcache)
        CONNECT(awqos)
        CONNECT(awregion)
        CONNECT(awuser)

        CONNECT(wlast)
        CONNECT(wvalid)
        CONNECT(wready)
        CONNECT(wdata)
        CONNECT(wstrb)
        // CONNECT(wid)
        CONNECT(wuser)
        CONNECT(bvalid)
        CONNECT(bready)
        CONNECT(bid)
        CONNECT(bresp)
        CONNECT(buser)

    CONNECT(arvalid)
    CONNECT(arready)
    CONNECT(arid)
    CONNECT(araddr)
    CONNECT(arlen)
    CONNECT(arsize)
    CONNECT(arburst)
    CONNECT(arlock)
    CONNECT(arcache)
    CONNECT(arprot)
    CONNECT(arqos)
    CONNECT(arregion)
    CONNECT(aruser)

    CONNECT(rlast)
    CONNECT(rvalid)
    CONNECT(rready)
    CONNECT(rid)
    CONNECT(rdata)
    CONNECT(rresp)
    CONNECT(ruser)
}

Top::Top(sc_module_name name, int bridge_num) :
        clk("clk", sc_time(1, SC_NS)),
        rst_n("rst_n")
{
    std::stringstream bridge_name;
    for (int i=0; i< bridge_num; i++) {
        bridge_name.str("");
        bridge_name << "bridge" << i;
        bridge.push_back(new axi_bridge(bridge_name.str().c_str()));
        bridge[i]->clk(clk);
        bridge[i]->rst_n(rst_n);
    }
}
