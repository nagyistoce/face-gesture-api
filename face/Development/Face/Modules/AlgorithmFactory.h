#pragma once

#include <string>
#include "tbb/mutex.h"
#include "tbb/flow_graph.h"

#include "FaceDef.h"

#define AlgorithmFactoryPtr AlgorithmFactory::GetInstance()


class Algorithm;

class AlgorithmFactory
{
public:
	static AlgorithmFactory* GetInstance(void);

	void Push(const std::string& name, const std::string& config);
    void Run(void);


private:
    static AlgorithmFactory* instance_;
    static tbb::mutex mutex_;

    AlgorithmFactory(void);
    void Release(void);

    typedef tbb::flow::continue_msg MessageType;
    typedef tbb::flow::broadcast_node<MessageType> BroadcastNode;
    typedef tbb::flow::continue_node<MessageType> ContinueNode;

    tbb::flow::graph graph_;
    BroadcastNode* broadcastNode_;
    std::vector<ContinueNode*> continueNodes_;
};
