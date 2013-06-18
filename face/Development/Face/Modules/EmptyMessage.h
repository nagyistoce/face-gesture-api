#pragma once

#include "Message.h"


class EmptyMessage
:   public Message
{
public:
    EmptyMessage(void);
    EmptyMessage(const EmptyMessage& other);
    ~EmptyMessage(void);

    EmptyMessage& operator=(EmptyMessage other);
};

