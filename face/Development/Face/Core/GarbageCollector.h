#pragma once

#include <map>
#include <tuple>
#include <string>
#include <tbb/mutex.h>
#include <tbb/compat/condition_variable>

#include "PortNameParser.h"

#define GarbageCollectorPtr GarbageCollector::GetInstance()


class Message;

class GarbageCollector
{
    typedef std::map<std::string, int> InputMap;
    typedef std::tuple<int, tbb::mutex*, std::condition_variable*, bool> GarbageItem;
    typedef std::map<Message*, GarbageItem> GarbageContainer;

public:
    enum GarbageItemIds
    {
        REF_COUNT = 0,
        MUTEX,
        COND_VAR,
        PRESENT
    };

    static GarbageCollector* GetInstance(void);

    void ParseConnectionMap(const VertexContainer& modules);
    void PushNewOutput(Message* newOutput, const std::string& moduleFullName);
    void InputHasBeenProcessed(Message* input);


private:
    static GarbageCollector* instance_;
    static tbb::mutex mutex_;

    GarbageCollector(void);
    ~GarbageCollector(void);

    InputMap inputMap_;
    GarbageContainer garbageContainer_;
};
