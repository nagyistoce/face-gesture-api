#include "SourceLimiterBody.h"
#include "Tracer.h"
#include "ImageWrapper.h"

using namespace std;


SourceLimiterBody::SourceLimiterBody(const VertexElement& vertexElement)
:   Body(vertexElement)
{
}


SourceLimiterBody::SourceLimiterBody(const SourceLimiterBody& other)
:   Body(other)
{
}


SourceLimiterBody::~SourceLimiterBody(void)
{
}


void SourceLimiterBody::operator() (const Body::InputType1& input, Node::MultiNodeContinueType::output_ports_type& output)
{
    if(HasSuccessor() == false)
        return;

    BeforeProcess();
    Process();
    AfterProcess();
    
    //if(HasBeenProcessed(input))
    std::get<OUTPUT_LIMITER>(output).try_put(tbb::flow::continue_msg());
}


void SourceLimiterBody::operator= (const SourceLimiterBody& other)
{
    WARNING("Equality operator has been disabled.");
    return;
}


void SourceLimiterBody::Process(void)
{

}
