#pragma once

namespace face 
{

class MetaData
{
public:
    MetaData(void);
    MetaData(const MetaData& other);
    ~MetaData(void);

    MetaData& operator= (MetaData other);

    void IncrementFrameNumber(void);

    int GetFrameNumber(void) const;
    unsigned int GetTimestamp(void) const;

    void SetFrameNumber(int value);
    void SetTimestamp(unsigned int value);

private:
    friend void swap(MetaData& first, MetaData& second);

    int frameNumber_;
    unsigned int timestamp_;
};

}
