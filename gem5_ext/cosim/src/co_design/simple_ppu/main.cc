#include <systemc>
#include <tlm>

#include "base/addr_range.hh"
#include "cli_parser.hh"
#include "params/ExternalMaster.hh"
#include "params/ExternalSlave.hh"
#include "master_transactor.hh"
#include "report_handler.hh"
#include "sc_initiator.hh"
#include "sc_target.hh"
#include "sim_control.hh"
#include "slave_transactor.hh"
#include "stats.hh"
#include "ini_loader.h"
#include <cstdio>
#include <list>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

static std::thread* gem5_thread = nullptr;
static std::mutex gem5_lock;
static std::condition_variable gem5_cv;
static bool gem5_ready = false;

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

    std::map<std::string,int> tlmObjectByName;
    std::map<std::string,std::string> tlmMasterPortName;
    std::map<std::string,std::string> tlmSlavePortName;
    std::map<std::string,gem5::AddrRange> tlmSlaveAddrRange;
    std::vector<std::string> tlmName;

    gem5::CxxConfigManager *config_manager = sim_control.config_manager;
    std::list<gem5::SimObject *> objectsInOrder = config_manager->objectsInOrder;

    for (auto i = objectsInOrder.begin(); i != objectsInOrder.end(); ++i)
    {
        gem5::SimObject *object = *i;
        const std::string &instance_name = object->name();
        std::string object_name = config_manager->unRename(instance_name);

        std::string object_type;
        const gem5::CxxConfigDirectoryEntry &entry = config_manager->findObjectType(object_name, object_type);
        gem5::CxxConfigParams* params = config_manager->findObjectParams(object_name);

        if (object_type == "ExternalMaster")
        {
            std::size_t pos = object_name.find("master");
            object_name.replace(pos, 6,"");
            tlmMasterPortName[object_name] = ((gem5::ExternalMasterParams*)params)->port_data;
            if (tlmObjectByName.find(object_name) != tlmObjectByName.end())
            {
                tlmObjectByName[object_name] |= 0x1;
            } else {
                tlmObjectByName[object_name] = 1;
            }
        }

        if (object_type == "ExternalSlave")
        {
            std::size_t pos = object_name.find("slave");
            object_name.replace(pos, 5,"");
            tlmSlavePortName[object_name] = ((gem5::ExternalSlaveParams*)params)->port_data;
            tlmSlaveAddrRange[object_name] = ((gem5::ExternalSlaveParams*)params)->addr_ranges;
            if (tlmObjectByName.find(object_name) != tlmObjectByName.end())
            {
                tlmObjectByName[object_name] |= 0x2;
            } else {
                tlmObjectByName[object_name] = 0x2;
            }
        }

        // printf(object_type.c_str());
    }

    int num_of_bridge = 0;
    for (auto i = tlmObjectByName.begin(); i != tlmObjectByName.end(); ++i)
    {
        std::string tlm_name = (*i).first;
        tlmName.push_back(tlm_name);
        int tlm_type = (*i).second;
        if (tlm_type == 3) {
            num_of_bridge++;
        } else {
            assert("only support bridge now");
        }
    }

    Top top("top", num_of_bridge);
    std::vector<Gem5SystemC::Gem5MasterTransactor*> master_transactor;
    std::vector<Gem5SystemC::Gem5SlaveTransactor*> slave_transactor;

    std::vector<Initiator*> initiator;
    std::vector<Target*> target;

    for (int i=0; i< num_of_bridge; i++) {
        std::string object_name = tlmName[i];
        // Master Transactor
        master_transactor.push_back(new Gem5SystemC::Gem5MasterTransactor("master_transactor" + i, tlmMasterPortName[object_name]));

        initiator.push_back(new Initiator("TrafficInitiator" + i, *(top.bridge[i])));
        initiator[i]->initiator_socket.bind(master_transactor[i]->socket);

        master_transactor[i]->sim_control.bind(sim_control);


        // Slave Transactor
        // unsigned long long int memorySize = 512*1024*1024ULL;
        slave_transactor.push_back(new Gem5SystemC::Gem5SlaveTransactor("slave_transactor" + i, tlmSlavePortName[object_name]));

        gem5::AddrRange range = tlmSlaveAddrRange[object_name];
        unsigned long long int target_start = range.start();
        unsigned long long int target_size = range.size();

        target.push_back(new Target("TrafficTarget" + i, *(top.bridge[i]),
                  parser.getVerboseFlag(),
                  target_size,
                  target_start));    // fixme
                  // parser.getMemoryOffset());    // fixme

        target[i]->socket.bind(slave_transactor[i]->socket);
        slave_transactor[i]->sim_control.bind(sim_control);

        top.bridge[i]->master_signals.Trace(trace_fp);
        top.bridge[i]->slave_signals.Trace(trace_fp);
    }

    sc_trace(trace_fp, top.rst_n, "rst_n");
    sc_trace(trace_fp, top.clk, "clk");
    // sc_trace(trace_fp, top.bridge.resetn, "bridge.resetn");
    // sc_trace(trace_fp, top.bridge.clk, "bridge.clk");

    // Reset is active low. Emit a reset cycle.
    top.rst_n.write(false);
    sc_start(10, SC_NS);
    top.rst_n.write(true);

    SC_REPORT_INFO("sc_main", "Start of Simulation");

    gem5_ready = true;
    {
        std::lock_guard<std::mutex> locker(gem5_lock);
        gem5_cv.notify_one();
    }

    sc_core::sc_start(); // Run to end of simulation
    // sc_core::sc_start(140, SC_US);

    SC_REPORT_INFO("sc_main", "End of Simulation");

    CxxConfig::statsDump();

    sc_stop();

    if (trace_fp) {
        sc_close_vcd_trace_file(trace_fp);
    }

    return EXIT_SUCCESS;
}

#define CONFIG_FILE "gem5_settings.ini"

extern "C" {
void app_direct_call_gpu(uint64_t gpusysno, uint64_t call_params);
void gem5_main() {
    util::config::ini_loader cfg_loader;
    int argc;

    cfg_loader.load_file(CONFIG_FILE);
    cfg_loader.dump_to("args.argc", argc);
    std::vector<char*> argv(argc+1, static_cast<char*>(NULL));
    char tmp[255];
    char argv0[] = "gem5_main";
    argv[0] = &argv0[0];
    for (int i = 1; i < argc + 1; ++i) {
        std::string arg_str("args.argv");
        cfg_loader.dump_to(arg_str + std::to_string(i), tmp);
        std::size_t size = std::strlen(tmp) + 1;
        argv[i] = new char[size];
        std::copy(tmp, tmp+size, argv[i]);
    }

    gem5_thread = new std::thread(sc_main, argc + 1, &argv[0]);
    std::unique_lock<std::mutex> locker(gem5_lock);
    gem5_cv.wait(locker, [] {return gem5_ready;});
}

void gem5_stop() {
    if (gem5_thread != nullptr) {
        gem5_thread->join();
        delete gem5_thread;
        gem5_thread = nullptr;
    }
}
}
