#pragma once

#include <string>
#include <sstream>

#include "FaceDef.h"

namespace face 
{

//! Simple templated string concatenator.
/*!
	\ingroup Others

	The + (addition) operator of the class is used as the << operator of the std::ostringstream class.
*/
class StringBuilder
{
public:
	//! Constructor.
	StringBuilder(void);

	//! Destructor.
	~StringBuilder(void);

	//! Addition operator.
	template <typename T>
	StringBuilder& operator +(const T& t)
	{
		stream_ << t;
		return *this;
	}

	//! Addition operator.
	template <typename T>
	StringBuilder& operator +=(const T& t)
	{
		stream_ << t;
		return *this;
	}

	//! String conversion.
	operator std::string();

private:
	std::ostringstream stream_;
};

}
