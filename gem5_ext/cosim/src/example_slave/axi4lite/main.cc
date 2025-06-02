#include <systemc>
#include <tlm>

#include "cli_parser.hh"
#include "report_handler.hh"
#include "sc_target.hh"
#include "sim_control.hh"
#include "slave_transactor.hh"
#include "stats.hh"

int
sc_main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    sc_trace_file *trace_fp = sc_create_vcd_trace_file(argv[0]);
    CliParser parser;
    parser.parse(argc, argv);

    sc_core::sc_report_handler::set_handler(reportHandler);

    Gem5SystemC::Gem5SimControl sim_control("gem5",
                                           parser.getConfigFile(),
                                           parser.getSimulationEnd(),
                                           parser.getDebugFlags());

    // unsigned long long int memorySize = 512*1024*1024ULL;
    unsigned long long int memorySize = 16ULL;

    Gem5SystemC::Gem5SlaveTransactor transactor("transactor", "transactor");
    Target memory("memory",
                  parser.getVerboseFlag(),
                  memorySize,
                  parser.getMemoryOffset());

    memory.socket.bind(transactor.socket);
    transactor.sim_control.bind(sim_control);

    memory.top.signals.Trace(trace_fp);
    sc_trace(trace_fp, memory.top.rst_n, "rst_n");
    sc_trace(trace_fp, memory.top.clk, "clk");
    sc_trace(trace_fp, memory.top.bridge.resetn, "bridge.resetn");
    sc_trace(trace_fp, memory.top.bridge.clk, "bridge.clk");

    // Reset is active low. Emit a reset cycle.
    memory.top.rst_n.write(false);
    sc_start(10, SC_NS);
    memory.top.rst_n.write(true);

    SC_REPORT_INFO("sc_main", "Start of Simulation");

    sc_core::sc_start();
    // sc_core::sc_start(140, SC_US);

    SC_REPORT_INFO("sc_main", "End of Simulation");

    CxxConfig::statsDump();

    sc_stop();

    if (trace_fp) {
        sc_close_vcd_trace_file(trace_fp);
    }
    return EXIT_SUCCESS;
}
