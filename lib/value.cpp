#include <cstdlib>
#include "defined.h"
#include "value.h"
#include "property.h"


using namespace std;
///////////////////////////////////////////////////////////////
// Class Value
///////////////////////////////////////////////////////////////
Value::~Value()
{
}

Value*	Duplicate(Value const *_value)
{
	Value*	value = NULL;
	
	if (_value != NULL)
	{
		return	_value->Duplicate();
	}

	return	value;
}

std::ostream& operator<<(std::ostream& os, Value const& _value)
{
	_value.Print(os);

	return	os;
}
///////////////////////////////////////////////////////////////
// Class ValueNumber
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Class ValueInt
///////////////////////////////////////////////////////////////
ValueInt::ValueInt(int _value)
: value_(_value)
{
}

ValueInt& ValueInt::operator=(int _value)
{
	value_ = _value;

	return	*this;
}

ValueInt::operator std::string() const
{
	return	to_string(value_);	
}

int		ValueInt::Get() const
{
	return	value_;
}

bool	ValueInt::Set(const string& _value)
{
	value_ = strtol(_value.c_str(), 0, 10);

	return	true;
} 

void	ValueInt::Set(int _value)
{
	value_ = _value;
}

Value*	ValueInt::Duplicate() const
{
	return	new ValueInt(value_);
}

void	ValueInt::Print(ostream&	os) const
{
	os << value_;
}

///////////////////////////////////////////////////////////////
// Class ValueUInt32
///////////////////////////////////////////////////////////////
ValueUInt32::ValueUInt32(uint32_t _value)
: value_(_value)
{
}

ValueUInt32& ValueUInt32::operator=(uint32_t _value)
{
	value_ = _value;

	return	*this;
}

uint32_t	ValueUInt32::Get() const
{
	return	value_;
}

bool	ValueUInt32::Set(const string& _value)
{
	value_ = strtol(_value.c_str(), 0, 10);

	return	true;
} 

void	ValueUInt32::Set(uint32_t _value)
{
	value_ = _value;
}

Value*	ValueUInt32::Duplicate() const
{
	return	new ValueUInt32(value_);
}

ValueUInt32::operator std::string() const
{
	return	to_string(value_);	
}

void	ValueUInt32::Print(ostream&	os) const
{
	os << value_;
}

///////////////////////////////////////////////////////////////
// Class ValueUInt64
///////////////////////////////////////////////////////////////
ValueUInt64::ValueUInt64(uint64_t _value)
: value_(_value)
{
}

ValueUInt64& ValueUInt64::operator=(uint64_t _value)
{
	value_ = _value;

	return	*this;
}

uint64_t	ValueUInt64::Get() const
{
	return	value_;
}

bool	ValueUInt64::Set(const string& _value)
{
	value_ = strtol(_value.c_str(), 0, 10);

	return	true;
} 

void	ValueUInt64::Set(uint64_t _value)
{
	value_ = _value;
}

Value*	ValueUInt64::Duplicate() const
{
	return	new ValueUInt64(value_);
}

ValueUInt64::operator std::string() const
{
	return	to_string(value_);	
}

void	ValueUInt64::Print(ostream&	os) const
{
	os << value_;
}

///////////////////////////////////////////////////////////////
// Class ValueBool
///////////////////////////////////////////////////////////////
ValueBool::ValueBool(bool _value)
: value_(_value)
{
}

ValueBool& ValueBool::operator=(bool _value)
{
	value_ = _value;

	return	*this;
}

ValueBool::operator bool() const
{
	return	value_;
}

bool	ValueBool::Get() const
{
	return	value_;
}

bool	ValueBool::Set(const string& _value)
{
	if (_value.compare("true") == 0)
	{	
		value_ = true;
	}
	else if (_value.compare("false") == 0)
	{
		value_ = false;
	}

	return	true;
}

void	ValueBool::Set(bool _value)
{
	value_ = _value;
}

Value*	ValueBool::Duplicate() const
{
	return	new ValueBool(value_);
}

ValueBool::operator std::string() const
{
	return	to_string(value_);	
}

void	ValueBool::Print(ostream&	os) const
{
	os << value_;
}
///////////////////////////////////////////////////////////////
// Class ValueFloat
///////////////////////////////////////////////////////////////
ValueFloat::ValueFloat(float _value)
: value_(_value)
{
}

ValueFloat::operator float() const
{
	return	value_;
}

ValueFloat& ValueFloat::operator=(float _value)
{
	value_ = _value;

	return	*this;
}

void	ValueFloat::Set(float _value)
{
	value_ = _value;
}

bool	ValueFloat::operator ==(const ValueFloat& _value)
{
	return	(value_ == _value.value_);
}

bool	ValueFloat::operator >=(const ValueFloat& _value)
{
	return	(value_ >= _value.value_);
}

bool	ValueFloat::operator >(const ValueFloat& _value)
{
	return	(value_ > _value.value_);
}

bool	ValueFloat::operator <=(const ValueFloat& _value)
{
	return	(value_ <= _value.value_);
}

bool	ValueFloat::operator <(const ValueFloat& _value)
{
	return	(value_ < _value.value_);
}

bool	ValueFloat::Set(const string& _value)
{
	value_ = ::atof(_value.c_str());

	return	true;
}

float	ValueFloat::Get() const
{
	return	value_;
}

Value*	ValueFloat::Duplicate() const
{
	return	new ValueFloat(value_);
}

ValueFloat::operator std::string() const
{
	return	to_string(value_);	
}

void	ValueFloat::Print(ostream&	os) const
{
	os << value_;
}
///////////////////////////////////////////////////////////////
// Class ValueString
///////////////////////////////////////////////////////////////
ValueString::ValueString()
: value_("")
{
}

ValueString::ValueString(char * _value)
: value_(_value)
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

bool	ValueString::operator==(std::string const& _value) const
{
	return	(value_ == _value);
}

bool	ValueString::operator==(char *_value) const
{
	return	value_.compare(_value);
}

ValueString::operator string() const
{
	return	value_;
}

Value*	ValueString::Duplicate() const
{
	return	new ValueString(value_);
}

const	std::string&	ValueString::Get() const
{
	return	value_;
}

bool	ValueString::Set(const string& _value)
{
	value_ = _value;

	return	true;
}

void	ValueString::Print(ostream&	os) const
{
	os << value_;
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

bool	ValueStringLimit::Set(const string& _value)
{
	if (_value.size() > limit_)
	{
		return	false;
	}

	value_ = _value.substr(0, limit_);

	return	true;
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

Value*	ValueID::Duplicate() const
{
	return	new ValueID(value_);
}

bool	ValueID::IsValid(std::string const& _id)
{
	return	(0 < _id.size() && _id.size() <= ID_LENGTH_MAX);
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

Value*	ValueName::Duplicate() const
{
	return	new ValueName(value_);
}

bool	ValueName::IsValid(std::string const& _name)
{
	return	(0 < _name.size() && _name.size() <= NAME_LENGTH_MAX);
}
///////////////////////////////////////////////////////////////
// Class ValueIP
///////////////////////////////////////////////////////////////
ValueIP::ValueIP(const ValueIP& _ip)
: ValueString(_ip.value_)
{
}

ValueIP::ValueIP(const std::string& _ip)
{
	if (IsValidIP(_ip))
	{
		value_ = _ip;	
	}
}

bool	ValueIP::Set(const string& _value)
{
	if (!IsValidIP(_value))
	{
		return	false;
	}

	value_ = _value;		

	return	true;
}

const	ValueIP&	ValueIP::operator=(const std::string& _value)
{
	Set(_value);

	return	*this;
}

bool	ValueIP::IsValidIP(const std::string& _ip)
{
	size_t found = _ip.find_first_not_of("0123456789.");
	if (found == string::npos)
	{
		int	number_of_dot = 0;
		int	position = -1;	

		size_t found = _ip.find_first_of(".");
		while(found != string::npos)
		{
			if (position + 1 == found)
			{
				break;
			}

			position = found;
			number_of_dot++;
			found = _ip.find_first_of(".", found+1);
		}

		if (number_of_dot == 3)
		{
			return	true;
		}
	}

	return	false;
}

Value*	ValueIP::Duplicate() const
{
	return	new ValueIP(value_);
}

///////////////////////////////////////////////////////////////
// Class ValueUnit
///////////////////////////////////////////////////////////////
ValueUnit::ValueUnit(const ValueUnit& _unit)
: ValueString(_unit.value_)
{
}

ValueUnit::ValueUnit(const std::string& _unit)
:	ValueString(_unit)	
{
}

bool	ValueUnit::Set(const string& _value)
{
	value_ = _value;		

	return	true;
}

Value*	ValueUnit::Duplicate() const
{
	return	new ValueUnit(value_);
}

const	ValueUnit&	ValueUnit::operator=(const std::string& _value)
{
	Set(_value);

	return	*this;
}

///////////////////////////////////////////////////////////////
// Class ValueTime
///////////////////////////////////////////////////////////////
ValueTime::ValueTime(const ValueTime& _value)
{
	value_ = _value.value_;
}

ValueTime::ValueTime(const Time& _date)
{
	value_ = _date;
}

const	Time&	ValueTime::Get() const
{
	return	value_;
}

bool	ValueTime::Set(const std::string& _value)
{
	return	false;
}

Value*	ValueTime::Duplicate() const
{
	return	new ValueTime(value_);
}

const	ValueTime&	ValueTime::operator=(Time const& _value)
{
	value_ = _value;

	return	*this;
}

ValueTime::operator std::string() const
{
	return	value_.ToString();
}


void	ValueTime::Print(ostream&	os) const
{
	os << value_.ToString();
}

///////////////////////////////////////////////////////////////
// Class ValueDate
///////////////////////////////////////////////////////////////
ValueDate::ValueDate(const ValueDate& _value)
{
	value_ = _value.value_;
}

ValueDate::ValueDate(const Date& _date)
{
	value_ = _date;
}

const	Date&	ValueDate::Get() const
{
	return	value_;
}

bool	ValueDate::Set(const std::string& _value)
{
	return	false;
}

Value*	ValueDate::Duplicate() const
{
	return	new ValueDate(value_);
}

const	ValueDate&	ValueDate::operator=(Date const& _value)
{
	value_ = _value;

	return	*this;
}

ValueDate::operator std::string() const
{
	return	value_.ToString();
}

void	ValueDate::Print(ostream&	os) const
{
	os << value_.ToString();
}

///////////////////////////////////////////////////////////////
// Class ValueProperties
///////////////////////////////////////////////////////////////
ValueProperties::ValueProperties(const ValueProperties& _value)
{
	value_ = new Properties(*_value.value_);
}

ValueProperties::ValueProperties(const Properties& _properties)
{
	value_ = new Properties(_properties);
}

ValueProperties::~ValueProperties()
{
	delete value_;
}

const	Properties&	ValueProperties::Get() const
{
	return	*value_;
}

bool	ValueProperties::Set(const std::string& _value)
{
	return	false;
}

Value*	ValueProperties::Duplicate() const
{
	return	new ValueProperties(*value_);
}

const	ValueProperties&	ValueProperties::operator=(Properties const& _value)
{
	*value_ = _value;

	return	*this;
}

ValueProperties::operator std::string() const
{
	std::ostringstream	oss;

	oss << *value_;

	return	oss.str();
}

void	ValueProperties::Print(ostream&	os) const
{
	os << *value_;
}

///////////////////////////////////////////////////////////////
// Class ValuePropertiesList
///////////////////////////////////////////////////////////////
ValuePropertiesList::ValuePropertiesList(const ValuePropertiesList& _value)
{
	value_ = new PropertiesList(*_value.value_);
}

ValuePropertiesList::ValuePropertiesList(const PropertiesList& _properties)
{
	value_ = new PropertiesList(_properties);
}

ValuePropertiesList::~ValuePropertiesList()
{
	delete value_;
}

const	PropertiesList&	ValuePropertiesList::Get() const
{
	return	*value_;
}

bool	ValuePropertiesList::Set(const std::string& _value)
{
	return	false;
}

Value*	ValuePropertiesList::Duplicate() const
{
	return	new ValuePropertiesList(*value_);
}

const	ValuePropertiesList&	ValuePropertiesList::operator=(PropertiesList const& _value)
{
	*value_ = _value;

	return	*this;
}

ValuePropertiesList::operator std::string() const
{
	std::ostringstream	oss;

	oss << *value_;

	return	oss.str();
}

void	ValuePropertiesList::Print(ostream&	os) const
{
	os << *value_;
}
