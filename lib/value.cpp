#include "defined.h"
#include "value.h"


using namespace std;
///////////////////////////////////////////////////////////////
// Class Value
///////////////////////////////////////////////////////////////
Value::~Value()
{
}

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

void	ValueInt::Set(const string& _value)
{
	value_ = strtol(_value.c_str(), 0, 10);
} 

string	ValueInt::ToString()
{
	return	to_string(value_);	
}
///////////////////////////////////////////////////////////////
// Class ValueBool
///////////////////////////////////////////////////////////////
ValueBool& ValueBool::operator=(bool _value)
{
	value_ = _value;

	return	*this;
}

void	ValueBool::Set(const string& _value)
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

string	ValueBool::ToString()
{
	return	to_string(value_);	
}
///////////////////////////////////////////////////////////////
// Class ValueFloat
///////////////////////////////////////////////////////////////
ValueFloat& ValueFloat::operator=(float _value)
{
	value_ = _value;

	return	*this;
}

void	ValueFloat::Set(const string& _value)
{
	value_ = ::atof(_value.c_str());
}

string	ValueFloat::ToString()
{
	to_string(value_);	
}
///////////////////////////////////////////////////////////////
// Class ValueString
///////////////////////////////////////////////////////////////
ValueString::ValueString()
: value_("")
{
}

ValueString::ValueString(const std::string& _value)
: value_(_value)
{
}

ValueString& ValueString::operator=(const string& _value)
{
	value_ = _value;

	return	*this;
}

void	ValueString::Set(const string& _value)
{
	value_ = _value;
}

string	ValueString::ToString()
{
	return	value_;
}
///////////////////////////////////////////////////////////////
// Class ValueStringLimit
///////////////////////////////////////////////////////////////
ValueStringLimit::ValueStringLimit(uint32_t _limit)
: limit_(_limit)
{
}

ValueStringLimit& ValueStringLimit::operator=(string& _value)
{
	value_ = _value.substr(0, limit_);

	return	*this;
}

ValueStringLimit& ValueStringLimit::operator=(char *_value)
{
	string value = _value;

	if (value.length() > limit_)
	{

		value_ = value.substr(0, limit_);
	}
	else
	{
		value_ = _value;
	}

	return	*this;
}

void	ValueStringLimit::Set(const string& _value)
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

ValueID::ValueID(const string& _value)
: ValueStringLimit(ID_LENGTH_MAX)
{
	Set(_value);
}

///////////////////////////////////////////////////////////////
// Class ValueName
///////////////////////////////////////////////////////////////
ValueName::ValueName()
: ValueStringLimit(NAME_LENGTH_MAX)
{
}

ValueName::ValueName(const string& _value)
: ValueStringLimit(NAME_LENGTH_MAX)
{
	Set(_value);
}

///////////////////////////////////////////////////////////////
// Class ValueIP
///////////////////////////////////////////////////////////////
void	ValueIP::Set(const string& _value)
{
	size_t found = _value.find_first_not_of("0123456789.");
	if (found == string::npos)
	{
		int	number_of_dot = 0;
		int	position = -1;	

		size_t found = _value.find_first_of(".");
		while(found != string::npos)
		{
			if (position + 1 == found)
			{
				break;
			}

			position = found;
			number_of_dot++;
			found = _value.find_first_of(".", found+1);
		}

		if (number_of_dot == 3)
		{
			value_ = _value;		
		}
	}
}

