#include "DebugManager.h"

#include <iostream>
#include <systemc>


void DebugManager::printTrans(const std::string& sender, tlm::tlm_generic_payload* payload, tlm::tlm_phase phase) {
    std::ostringstream oss;
    oss << "Payload:" << payload << " phase:" << phase;
    printMessage(sender, oss.str());
}

void DebugManager::printDebugMessage(const std::string& sender, const std::string& message)
{
    if (debugEnabled)
    {
        if (writeToConsole)
            std::cout << " at " << sc_core::sc_time_stamp() << "\t in " << sender
                      << "\t: " << message << std::endl;

        if (writeToFile && debugFile)
            debugFile << " at " << sc_core::sc_time_stamp() << "\t in " << sender
                      << "\t: " << message << std::endl;
    }
}

void DebugManager::setup(bool _debugEnabled, bool _writeToConsole, bool _writeToFile)
{
    debugEnabled = _debugEnabled;
    writeToConsole = _writeToConsole;
    writeToFile = _writeToFile;
}

void DebugManager::printMessage(const std::string& sender, const std::string& message)
{
    std::cout << " at " << sc_core::sc_time_stamp() << "\t in " << sender << "\t: " << message
              << std::endl;
}

void DebugManager::openDebugFile(const std::string& filename)
{
    if (debugFile)
        debugFile.close();
    debugFile.open(filename);
}

DebugManager::DebugManager() = default;

DebugManager::~DebugManager()
{
    if (writeToFile)
    {
        debugFile.flush();
        debugFile.close();
    }
}


