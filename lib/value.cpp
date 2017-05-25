#include <cstdlib>
#include <typeinfo>
#include <iomanip>
#include "defined.h"
#include "trace.h"
#include "value.h"
#include "property.h"


using namespace std;
///////////////////////////////////////////////////////////////
// Class Value
///////////////////////////////////////////////////////////////
Value::Value()
: date_()
{

}

Value::Value(Date const& _date)
: date_(_date)
{
}

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
	return	os << std::string(_value);
}

std::string	Value::GetTypeString() const
{
	return	typeid(this).name();
}
///////////////////////////////////////////////////////////////
// Class ValueNumber
///////////////////////////////////////////////////////////////
ValueNumber::ValueNumber()
: Value()
{
}

ValueNumber::ValueNumber(Date const& _date)
: Value(_date)
{
}

std::string	ValueNumber::GetTypeString() const
{
	return	typeid(this).name();
}
///////////////////////////////////////////////////////////////
// Class ValueInt
///////////////////////////////////////////////////////////////
ValueInt::ValueInt(int _value)
: ValueNumber(), value_(_value)
{
}

ValueInt::ValueInt(int _value, Date const& _date)
: ValueNumber(_date), value_(_value)
{
}

std::string	ValueInt::GetTypeString() const
{
	return	typeid(this).name();
}

ValueInt& ValueInt::operator=(int _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();

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
	date_ = Date::GetCurrentDate();

	return	true;
} 

void	ValueInt::Set(int _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();
}

Value*	ValueInt::Duplicate() const
{
	return	new ValueInt(value_, date_);
}

void	ValueInt::Print(ostream&	os) const
{
	os << value_;
}

///////////////////////////////////////////////////////////////
// Class ValueUInt32
///////////////////////////////////////////////////////////////
ValueUInt32::ValueUInt32(uint32_t _value)
: ValueNumber(), value_(_value)
{
}

ValueUInt32::ValueUInt32(uint32_t _value, Date const& _date)
: ValueNumber(_date), value_(_value)
{
}

std::string	ValueUInt32::GetTypeString() const
{
	return	typeid(this).name();
}

ValueUInt32& ValueUInt32::operator=(uint32_t _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();

	return	*this;
}

uint32_t	ValueUInt32::Get() const
{
	return	value_;
}

bool	ValueUInt32::Set(const string& _value)
{
	value_ = strtol(_value.c_str(), 0, 10);
	date_ = Date::GetCurrentDate();

	return	true;
} 

void	ValueUInt32::Set(uint32_t _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();
}

Value*	ValueUInt32::Duplicate() const
{
	return	new ValueUInt32(value_, date_);
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
: ValueNumber(), value_(_value)
{
}

ValueUInt64::ValueUInt64(uint64_t _value, Date const& _date)
: ValueNumber(_date), value_(_value)
{
}

std::string	ValueUInt64::GetTypeString() const
{
	return	typeid(this).name();
}

ValueUInt64& ValueUInt64::operator=(uint64_t _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();

	return	*this;
}

uint64_t	ValueUInt64::Get() const
{
	return	value_;
}

bool	ValueUInt64::Set(const string& _value)
{
	value_ = strtol(_value.c_str(), 0, 10);
	date_ = Date::GetCurrentDate();

	return	true;
} 

void	ValueUInt64::Set(uint64_t _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();
}

Value*	ValueUInt64::Duplicate() const
{
	return	new ValueUInt64(value_, date_);
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
: ValueNumber(), value_(_value)
{
}

ValueBool::ValueBool(bool _value, Date const& _date)
: ValueNumber(_date), value_(_value)
{
}

std::string	ValueBool::GetTypeString() const
{
	return	typeid(this).name();
}

ValueBool& ValueBool::operator=(bool _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();

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
	date_ = Date::GetCurrentDate();

	return	true;
}

void	ValueBool::Set(bool _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();
}

Value*	ValueBool::Duplicate() const
{
	return	new ValueBool(value_, date_);
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
: ValueNumber(), value_(_value), precision_(2)
{
}

ValueFloat::ValueFloat(float _value, Date const& _date)
: ValueNumber(_date), value_(_value), precision_(2)
{
}

std::string	ValueFloat::GetTypeString() const
{
	return	typeid(this).name();
}

ValueFloat::operator float() const
{
	return	value_;
}

ValueFloat& ValueFloat::operator=(float _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();

	return	*this;
}

void	ValueFloat::Set(float _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();
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
	date_ = Date::GetCurrentDate();

	return	true;
}

float	ValueFloat::Get() const
{
	return	value_;
}

Value*	ValueFloat::Duplicate() const
{
	return	new ValueFloat(value_, date_);
}

ValueFloat::operator std::string() const
{
#if 0
	std::ostringstream	oss;
	oss << value_;
	return	oss.str();
#else
	char	buffer[32];

	sprintf(buffer, "%.2f", value_);

	return	std::string(buffer);
#endif
}

void	ValueFloat::Print(ostream&	os) const
{
	os << value_;
}
///////////////////////////////////////////////////////////////
// Class ValueString
///////////////////////////////////////////////////////////////
ValueString::ValueString()
: Value(), value_("")
{
}

ValueString::ValueString(char * _value)
: value_(_value)
{
}

ValueString::ValueString(char * _value, Date const& _date)
: Value(_date), value_(_value)
{
}

ValueString::ValueString(const std::string& _value)
: value_(_value)
{
}

ValueString::ValueString(const std::string& _value, Date const& _date)
: Value(_date), value_(_value)
{
}

std::string	ValueString::GetTypeString() const
{
	return	typeid(this).name();
}

ValueString& ValueString::operator=(const string& _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();

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
	return	new ValueString(value_, date_);
}

const	std::string&	ValueString::Get() const
{
	return	value_;
}

bool	ValueString::Set(const string& _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();

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
: ValueString(), limit_(_limit)
{
}

std::string	ValueStringLimit::GetTypeString() const
{
	return	typeid(this).name();
}

ValueStringLimit& ValueStringLimit::operator=(string const& _value)
{
	value_ = _value.substr(0, limit_);
	date_ = Date::GetCurrentDate();

	return	*this;
}

ValueStringLimit& ValueStringLimit::operator=(char const* _value)
{
	string value = _value;
	date_ = Date::GetCurrentDate();

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
	date_ = Date::GetCurrentDate();

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

ValueID::ValueID(const string& _value, Date const& _date)
: ValueStringLimit(ID_LENGTH_MAX)
{
	Set(_value);
	date_ = _date;
}

std::string	ValueID::GetTypeString() const
{
	return	typeid(this).name();
}

Value*	ValueID::Duplicate() const
{
	return	new ValueID(value_, date_);
}

bool	ValueID::IsValid(std::string const& _id)
{
	return	(0 < _id.size() && _id.size() <= ID_LENGTH_MAX);
}

///////////////////////////////////////////////////////////////
// Class ValueType
///////////////////////////////////////////////////////////////
ValueType::ValueType(const ValueType& _unit)
: ValueString(_unit.value_, _unit.date_)
{
}

ValueType::ValueType(const std::string& _unit)
:	ValueString(_unit)	
{
}

ValueType::ValueType(const std::string& _unit, Date const& _date)
:	ValueString(_unit, _date)	
{
}

std::string	ValueType::GetTypeString() const
{
	return	typeid(this).name();
}

bool	ValueType::Set(const string& _value)
{
	value_ = _value;		
	date_ = Date::GetCurrentDate();

	return	true;
}

Value*	ValueType::Duplicate() const
{
	return	new ValueType(value_, date_);
}

const	ValueType&	ValueType::operator=(const std::string& _value)
{
	Set(_value);

	return	*this;
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
	if (_value.size() >	NAME_LENGTH_MAX)
	{
		std::ostringstream	message;

		message << "Name length is too long.( < " << NAME_LENGTH_MAX << ")";
		throw std::invalid_argument(message.str());
	}

	Set(_value);
}

ValueName::ValueName(const string& _value, Date const& _date)
: ValueStringLimit(NAME_LENGTH_MAX)
{
	if (_value.size() >	NAME_LENGTH_MAX)
	{
		std::ostringstream	message;

		message << "Name length is too long.( < " << NAME_LENGTH_MAX << ")";
		throw std::invalid_argument(message.str());
	}

	Set(_value);
	date_ = _date;
}

std::string	ValueName::GetTypeString() const
{
	return	typeid(this).name();
}

ValueName& 	ValueName::operator=(const std::string& _value)
{
	Set(_value);

	return	*this;
}

Value*	ValueName::Duplicate() const
{
	return	new ValueName(value_, date_);
}

bool	ValueName::IsValid(std::string const& _name)
{
	return	(0 < _name.size() && _name.size() <= NAME_LENGTH_MAX);
}
///////////////////////////////////////////////////////////////
// Class ValueIP
///////////////////////////////////////////////////////////////
ValueIP::ValueIP(const ValueIP& _ip)
: ValueString(_ip.value_, _ip.date_)
{
}

ValueIP::ValueIP(const std::string& _ip)
: ValueString()
{
	if (IsValidIP(_ip))
	{
		value_ = _ip;	
	}
}

ValueIP::ValueIP(const std::string& _ip, Date const& _date)
: ValueString()
{
	if (IsValidIP(_ip))
	{
		value_ = _ip;	
		date_ = _date;
	}
}

std::string	ValueIP::GetTypeString() const
{
	return	typeid(this).name();
}

bool	ValueIP::Set(const string& _value)
{
	if (!IsValidIP(_value))
	{
		return	false;
	}

	value_ = _value;		
	date_ = Date::GetCurrentDate();

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
	return	new ValueIP(value_, date_);
}

///////////////////////////////////////////////////////////////
// Class ValueUnit
///////////////////////////////////////////////////////////////
ValueUnit::ValueUnit(const ValueUnit& _unit)
: ValueString(_unit.value_, _unit.date_)
{
}

ValueUnit::ValueUnit(const std::string& _unit)
:	ValueString(_unit)	
{
}

ValueUnit::ValueUnit(const std::string& _unit, Date const& _date)
:	ValueString(_unit, _date)	
{
}

std::string	ValueUnit::GetTypeString() const
{
	return	typeid(this).name();
}

bool	ValueUnit::Set(const string& _value)
{
	value_ = _value;		
	date_ = Date::GetCurrentDate();

	return	true;
}

Value*	ValueUnit::Duplicate() const
{
	return	new ValueUnit(value_, date_);
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
: Value()
{
	value_ = _value.value_;
	date_ = _value.date_;
}

ValueTime::ValueTime(const Time& _time)
: Value()
{
	value_ = _time;
}

ValueTime::ValueTime(const Time& _time, Date const& _date)
: Value()
{
	value_ = _time;
	date_  = _date;
}

std::string	ValueTime::GetTypeString() const
{
	return	typeid(this).name();
}

const	Time&	ValueTime::Get() const
{
	return	value_;
}

bool	ValueTime::Set(const std::string& _value)
{
	date_ = Date::GetCurrentDate();
	return	false;
}

Value*	ValueTime::Duplicate() const
{
	return	new ValueTime(value_, date_);
}

const	ValueTime&	ValueTime::operator=(Time const& _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();

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
: Value()
{
	value_ = _value.value_;
	date_  = _value.date_;
}

ValueDate::ValueDate(const Date& _value)
: Value()
{
	value_ = _value;
}

ValueDate::ValueDate(const Date& _value, Date const& _date)
: Value()
{
	value_ = _value;
	date_  = _date;
}

std::string	ValueDate::GetTypeString() const
{
	return	typeid(this).name();
}

const	Date&	ValueDate::Get() const
{
	return	value_;
}

bool	ValueDate::Set(const std::string& _value)
{
	date_ = Date::GetCurrentDate();
	return	false;
}

Value*	ValueDate::Duplicate() const
{
	return	new ValueDate(value_, date_);
}

const	ValueDate&	ValueDate::operator=(Date const& _value)
{
	value_ = _value;
	date_ = Date::GetCurrentDate();

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
: Value()
{
	value_ = new Properties(*_value.value_);
}

ValueProperties::ValueProperties(const ValueProperties& _value, Date const& _date)
: Value()
{
	value_ = new Properties(*_value.value_);
	date_  = _date;
}

ValueProperties::ValueProperties(const Properties& _properties)
: Value()
{
	value_ = new Properties(_properties);
}

ValueProperties::ValueProperties(const Properties& _properties, Date const& _date)
: Value()
{
	value_ = new Properties(_properties);
	date_  = _date;
}

ValueProperties::~ValueProperties()
{
	delete value_;
}

std::string	ValueProperties::GetTypeString() const
{
	return	typeid(this).name();
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
	return	new ValueProperties(*value_, date_);
}

const	ValueProperties&	ValueProperties::operator=(Properties const& _value)
{
	*value_ = _value;
	date_ = Date::GetCurrentDate();

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
: Value()
{
	value_ = new PropertiesList(*_value.value_);
}

ValuePropertiesList::ValuePropertiesList(const ValuePropertiesList& _value, Date const& _date)
: Value()
{
	value_ = new PropertiesList(*_value.value_);
	date_  = _date;
}

ValuePropertiesList::ValuePropertiesList(const PropertiesList& _properties)
: Value()
{
	value_ = new PropertiesList(_properties);
}

ValuePropertiesList::ValuePropertiesList(const PropertiesList& _properties, Date const& _date)
: Value()
{
	value_ = new PropertiesList(_properties);
	date_  = _date;
}

ValuePropertiesList::~ValuePropertiesList()
{
	delete value_;
}

std::string	ValuePropertiesList::GetTypeString() const
{
	return	typeid(this).name();
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
	return	new ValuePropertiesList(*value_, date_);
}

const	ValuePropertiesList&	ValuePropertiesList::operator=(PropertiesList const& _value)
{
	*value_ = _value;
	date_ = Date::GetCurrentDate();

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
