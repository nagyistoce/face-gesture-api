#include <iostream>
#include <algorithm> 

#include "ModuleGraph.h"
#include "GlobalSettings.h"
#include "Tracer.h"
#include "XmlReader.h"
#include "ModuleFactory.h"
#include "GarbageCollector.h"

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

        modules_.push_back(make_pair(ppp, childrenList));
    }

    CheckModules();

    GarbageCollectorPtr->ParseConnectionMap(modules_);
    ModuleFactoryPtr->CreateConnections(modules_);
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
    for(int i = 0; i < int(modules_.size()); i++)
    {
        if(modules_[i].first == ppp) 
        {
            return true;
        }
    }

	return false;
}


void ModuleGraph::CheckModules(void)
{
    for(ConnectionMap::const_iterator itCpOut = modules_.begin(); itCpOut != modules_.end(); itCpOut++)
    {
        for(ChildrenList::const_iterator itChild = itCpOut->second.begin(); itChild != itCpOut->second.end(); itChild++)
        {
            bool found = false;
            for(ConnectionMap::const_iterator itCpIn = modules_.begin(); itCpIn != modules_.end(); itCpIn++)
            {
                if(itChild->GetFullName() == itCpIn->first.GetFullName())
                {
                    found = true;
                    break;
                }
            }

            if(found == false)
                CV_Error(-1, "Use of undefined children module in process.xml: " + itCpOut->first.GetFullName() + " -> " + itChild->GetFullName());
        }
    }
}
