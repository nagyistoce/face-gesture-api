#include <algorithm>
#include "MetaData.h"

namespace face 
{

void swap(MetaData& first, MetaData& second)
{
    using std::swap;
    swap(first.frameNumber_, second.frameNumber_);
    swap(first.timestamp_, second.timestamp_);
}


MetaData::MetaData(void)
:   frameNumber_(0),
    timestamp_(0)
{
}


MetaData::MetaData(const MetaData& other)
:   frameNumber_(other.frameNumber_),
	timestamp_(other.timestamp_)
{
}


MetaData::~MetaData(void)
{
}


MetaData& MetaData::operator=(MetaData other)
{
    swap(*this, other);
    return *this;
}


void MetaData::IncrementFrameNumber(void)
{
    frameNumber_++;
}


int MetaData::GetFrameNumber(void) const
{
    return frameNumber_;
}


unsigned int MetaData::GetTimestamp(void) const
{
    return timestamp_;
}


void MetaData::SetFrameNumber(int value)
{
    frameNumber_ = value;
}


void MetaData::SetTimestamp(unsigned int value)
{
    timestamp_ = value;
}

}
