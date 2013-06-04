#pragma once

#include <map>
#include <tuple>
#include <string>
#include <tbb/mutex.h>
#include "PortNameParser.h"

#define GarbageCollectorPtr GarbageCollector::GetInstance()


class Message;

class GarbageCollector
{
    typedef std::map<std::string, int> InputMap;
    typedef std::map<Message*, int> ModuleOutputMap;

public:
    static GarbageCollector* GetInstance(void);

    void ParseConnectionMap(const ConnectionMap& modules);
    void PushNewOutput(Message* newItem, const std::string& moduleFullName);
    bool InputHasBeenProcessed(Message* input);


private:
    static GarbageCollector* instance_;
    static tbb::mutex mutex_;

    GarbageCollector(void);
    ~GarbageCollector(void);

    InputMap inputMap_;
    ModuleOutputMap moduleOutputMap_;
};
