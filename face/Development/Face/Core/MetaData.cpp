#include <algorithm>
#include "MetaData.h"


void swap(MetaData& first, MetaData& second)
{
    using std::swap;
    swap(first.position_, second.position_);
    swap(first.frameNumber_, second.frameNumber_);
    swap(first.timestamp_, second.timestamp_);
}


MetaData::MetaData(void)
:   position_(POSITION_UNDEFINED),
    frameNumber_(0),
    timestamp_(0)
{
}


MetaData::MetaData(const Position position, const int frameNumber, const unsigned int timestamp)
:   position_(position),
    frameNumber_(frameNumber),
    timestamp_(timestamp)
{
}


MetaData::MetaData(const MetaData& other)
{
    position_ = other.position_;
    frameNumber_ = other.frameNumber_;
    timestamp_ = other.timestamp_;
}


MetaData::~MetaData(void)
{
}


MetaData& MetaData::operator=(MetaData other)
{
    swap(*this, other);
    return *this;
}


MetaData::Position MetaData::GetPosition(void) const
{
    return position_;
}


int MetaData::GetFrameNumber(void) const
{
    return frameNumber_;
}


unsigned int MetaData::GetTimestamp(void) const
{
    return timestamp_;
}
