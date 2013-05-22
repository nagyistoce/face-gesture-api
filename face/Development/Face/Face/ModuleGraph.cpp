#include <iostream>
#include <algorithm> 

#include "ModuleGraph.h"
#include "GlobalSettings.h"
#include "Tracer.h"
#include "XmlReader.h"
#include "ModuleFactory.h"

using namespace std;
using namespace cv;


ModuleGraph::ModuleGraph(void)
{
}


ModuleGraph::~ModuleGraph(void)
{
}


void ModuleGraph::BuildFromFileNode(const FileNode& tasksNode)
{
	if(tasksNode.isSeq() == false) return;

    // Load all of the processes:
    for(int i = 0; i < int(tasksNode.size()); ++i)
    {
        const FileNode& moduleNode = tasksNode[i];
        const FileNode& nameNode = moduleNode["module"];
        const FileNode& childrenNode = moduleNode["to"];

        PortNameParser ppp(nameNode.isString() ? string(nameNode) : string());
        bool visible = XmlReader::ReadBool(moduleNode, "visible", false);

        if(NodeAlreadyExists(ppp))
        {
            LOG(ppp.GetFullName() + " - Duplicated entry in Process.xml. Skipped.");
            continue;
        }

        ChildrenList childrenList;
        ReadConnectionListFromFileNode(childrenNode, ppp, &childrenList);

        if(ppp.GetModuleName() == "Source") // Source
        {
            sources_.push_back(make_pair(ppp, childrenList));
        } 
        else // Module
        {
            modules_.push_back(make_pair(ppp, childrenList));
        }
    }

    ModuleFactoryPtr->CreateConnections(sources_, modules_);
}


void ModuleGraph::ReadConnectionListFromFileNode(const FileNode& childrenNode, const PortNameParser& ppp, ChildrenList* list)
{
	if(childrenNode.isString())
	{
        PortNameParser cppp(childrenNode.isString() ? string(childrenNode) : string());
        list->push_back(cppp);
	} 
	else if(childrenNode.isSeq())
	{
		for(int j = 0; j < int(childrenNode.size()); ++j)
        {
            PortNameParser cppp(childrenNode[j].isString() ? string(childrenNode[j]) : string());
			list->push_back(cppp);
        }
	}
}


bool ModuleGraph::NodeAlreadyExists(const PortNameParser& ppp) const
{
    if(ppp.GetModuleName() == "Source")
    {
        for(int i = 0; i < int(sources_.size()); i++)
            if(sources_[i].first == ppp) return true;
    }
    else
    {
        for(int i = 0; i < int(modules_.size()); i++)
            if(modules_[i].first == ppp) return true;
    }

	return false;
}
