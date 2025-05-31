#ifndef DEBUGMANAGER_H
#define DEBUGMANAGER_H

#include <systemc>
#include <tlm>
//#ifdef NDEBUG
//#define PRINTDEBUGMESSAGE(sender, message)                                                         \
//    {                                                                                              \
//    }
//#else
#define DBGPRINT(sender, message)                                                         \
    DebugManager::getInstance().printDebugMessage(sender, message)


//#define DBGTRANS(sender, trans, phase) {}
#define DBGTRANS(sender, trans, phase)                                                    \
    DebugManager::getInstance().printTrans(sender, trans, phase)
//#define DBGTRANS(sender, message)                                                         \

#include <fstream>
#include <string>


class DebugManager
{
public:
    static DebugManager& getInstance()
    {
        static DebugManager _instance;
        return _instance;
    }
    ~DebugManager();

private:
    DebugManager();

public:
    DebugManager(const DebugManager&) = delete;
    DebugManager& operator=(const DebugManager&) = delete;
    DebugManager(DebugManager&&) = delete;
    DebugManager& operator=(DebugManager&&) = delete;

    void setup(bool _debugEnabled, bool _writeToConsole, bool _writeToFile);

    void printTrans(const std::string& sender, tlm::tlm_generic_payload* payload, tlm::tlm_phase phase);
    void printDebugMessage(const std::string& sender, const std::string& message);
    static void printMessage(const std::string& sender, const std::string& message);
    void openDebugFile(const std::string& filename);

private:
    bool debugEnabled = false;
    bool writeToConsole = false;
    bool writeToFile = false;

    std::ofstream debugFile;
};

//#endif

#endif // DEBUGMANAGER_H
