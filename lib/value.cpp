#include <cstdlib>
#include "limit.h"
#include "value.h"

using namespace std;
///////////////////////////////////////////////////////////////
// Class Value
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Class ValueNumber
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Class ValueInt
///////////////////////////////////////////////////////////////
ValueInt& ValueInt::operator=(int _value)
{
	value_ = _value;

	return	*this;
}

void	ValueInt::Set(const std::string& _value)
{
	value_ = strtol(_value.c_str(), 0, 10);
}
///////////////////////////////////////////////////////////////
// Class ValueBool
///////////////////////////////////////////////////////////////
ValueBool& ValueBool::operator=(bool _value)
{
	value_ = _value;

	return	*this;
}

void	ValueBool::Set(const std::string& _value)
{
	if (_value.compare("true") == 0)
	{	
		value_ = true;
	}
	else if (_value.compare("false") == 0)
	{
		value_ = false;
	}
}
///////////////////////////////////////////////////////////////
// Class ValueFloat
///////////////////////////////////////////////////////////////
ValueFloat& ValueFloat::operator=(float _value)
{
	value_ = _value;

	return	*this;
}

void	ValueFloat::Set(const std::string& _value)
{
	value_ = ::atof(_value.c_str());
}
///////////////////////////////////////////////////////////////
// Class ValueString
///////////////////////////////////////////////////////////////
ValueString& ValueString::operator=(const std::string& _value)
{
	value_ = _value;

	return	*this;
}
///////////////////////////////////////////////////////////////
// Class ValueStringLimit
///////////////////////////////////////////////////////////////
ValueStringLimit::ValueStringLimit(uint32_t _limit)
: limit_(_limit)
{
}

ValueStringLimit& ValueStringLimit::operator=(std::string& _value)
{
	value_ = _value.substr(0, limit_);

	return	*this;
}

ValueStringLimit& ValueStringLimit::operator=(char *_value)
{
	if (strlen(_value) > limit_)
	{
		string value = _value;

		value_ = value.substr(0, limit_);
	}
	else
	{
		value_ = _value;
	}

	return	*this;
}

void	ValueStringLimit::Set(const std::string& _value)
{
	value_ = _value.substr(0, limit_);
}

///////////////////////////////////////////////////////////////
// Class ValueID
///////////////////////////////////////////////////////////////
ValueID::ValueID()
: ValueStringLimit(ID_LENGTH_MAX)
{
}

///////////////////////////////////////////////////////////////
// Class ValueName
///////////////////////////////////////////////////////////////
ValueName::ValueName()
: ValueStringLimit(NAME_LENGTH_MAX)
{
}

///////////////////////////////////////////////////////////////
// Class ValueIP
///////////////////////////////////////////////////////////////
void	ValueStringLimit::Set(const std::string& _value)
{
	size_t found = _value.find_first_not_of("0123456789.");
	if (found == string::npos)
	{
		int	number_of_dot = 0;
		int	postition = -1;	

		size_t found = _value.find_first_of(".");
		while(found != string::npos)
		{
			if (position + 1 == found)
			{
				break;
			}

			number_of_dot++;
			found = _value.find_first_of(".", found+1);
		}

		if (number_of_dot == 3)
		{
			value_ = _value;		
		}
	}
}

