#pragma once

#include "FaceDef.h"


//! Abstract class for passing messages from Publisher to Subscriber objects.
/*!
	\ingroup Core
	Classes can be derived from this class as long as they implement the Message::Clone() method.
*/
class Message 
{
public:
	//! Constructor.
	Message(void);

	//! Destructor.
	virtual ~Message(void);

	//! Abstract clone method which should be implemented.
	virtual Message* Clone(void) const = 0;

private:
	//! Disabled copy constructor.
	Message(const Message& other);

	//! Disabled copy operator.
	Message& operator =(const Message& other);
};
