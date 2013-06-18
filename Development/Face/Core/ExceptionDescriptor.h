#pragma once

#include <exception>
#include <iostream>
#include <string>

#include "FaceDef.h"
#include "StringBuilder.h"

namespace face 
{

//! Exception class.
/*!
	\ingroup Core

	Exception class whose instances store an error message, source file, function and line number.
*/
class ExceptionDescriptor 
:   public std::exception
{
public:
	//! Constructor.
	/*!
		\param error    Error message.
		\param fileName Name of the file in which the exception was raised.
		\param funcName Name of the function in which the exception was raised.
		\param line     Number of line on which the exception was raised.
		\param isError  Error or abnormal operation.

		Should be used through the macro NewError(message) or NewWarning(message).
	*/
	ExceptionDescriptor(
		const std::string& error,
		const std::string& fileName,
		const std::string& funcName,
		int                line,
		bool               isError = true) throw();

	//! Copy constructor.
	/*!
		\param e Another ExceptionDescriptor.
	*/
	ExceptionDescriptor(const ExceptionDescriptor& e) throw();

	//! Destructor.
	~ExceptionDescriptor(void) throw();

	//! std::exception::what()
	const char* what(void) const throw();

	//! Writes the exception description to stderr.
	void TraceError(void) throw();

private:
	std::string error_;    //!< Error message.
	std::string fileName_; //!< Name of the file.
	std::string funcName_; //!< Name of the function.
	int         line_;     //!< Line number.
	bool        isError_;  //!< Error or abnormal operation.
};

#define ExceptionError(message)   ExceptionDescriptor(std::string(StringBuilder() + message), std::string(__FILE__), std::string(__FUNCTION__), __LINE__, true)
#define ExceptionWarning(message) ExceptionDescriptor(std::string(StringBuilder() + message), std::string(__FILE__), std::string(__FUNCTION__), __LINE__, false)

}
