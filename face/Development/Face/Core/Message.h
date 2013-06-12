#pragma once

#include "FaceDef.h"

namespace face 
{

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
};

}
