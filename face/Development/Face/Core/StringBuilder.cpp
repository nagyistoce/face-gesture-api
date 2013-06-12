#include "StringBuilder.h"

namespace face 
{

StringBuilder::StringBuilder(void)
{
}


StringBuilder::~StringBuilder(void)
{
}


StringBuilder::operator std::string()
{
	return stream_.str();
}

}
