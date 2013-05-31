#pragma once

class MetaData
{
public:
    enum Position
    {
        POSITION_FRONT = 0,
        POSITION_UNDEFINED
    };

    MetaData(void);
    MetaData(const Position position, const int frameNumber, const unsigned int timestamp);
    MetaData(const MetaData& other);
    ~MetaData(void);

    MetaData& operator=(MetaData other);

    Position GetPosition(void) const;
    int GetFrameNumber(void) const;
    unsigned int GetTimestamp(void) const;

private:
    friend void swap(MetaData& first, MetaData& second);

    Position position_;
    int frameNumber_;
    unsigned int timestamp_;
};