#include <iostream>
#include <sys/time.h>
#include <time.h>
#include "defined.h"
#include "exception.h"
#include "time2.h"
#include "trace.h"

using namespace std;

//////////////////////////////////////////////////////////////
// Class Time
//////////////////////////////////////////////////////////////

Time::Time()
: value_(0)
{
}

Time::Time(const Time& _time)
: value_(_time.value_)
{
}

Time::Time(uint64_t _microseconds)
: value_(_microseconds)
{
}

uint64_t	Time::Get() const
{
	return	value_;
}

uint64_t	Time::GetMilliseconds() const
{
	return	value_ / TIME_MILLISECOND;
}

uint32_t	Time::GetSeconds() const
{
	return	value_ / TIME_SECOND;
}

Time::operator uint64_t() const
{	
	return	value_; 
}


Time	Time::operator+(const Time& _time)
{
	Time	time;

	time.value_ = value_ + _time.value_;

	return	time;
}

Time	Time::operator-(const Time& _time)
{
	Time	time;

	if (value_ >= _time.value_)
	{
		time.value_ = value_ - _time.value_;		
	}

	return	time;
}

const 	Time&	Time::operator+=(const Time& _time)
{
	value_ += _time.value_;

	return	*this;
}

const 	Time&	Time::operator-=(const Time& _time)
{
	if (value_ >= _time.value_)
	{
		value_ -= _time.value_;	
	}
	else
	{
		value_ = 0;
	}

	return	*this;
}

std::string	Time::ToString() const
{
	return	to_string(value_);
}
//////////////////////////////////////////////////////////////
// Class Date
//////////////////////////////////////////////////////////////

Date::Date()
{
	struct timeval	value;

	gettimeofday(&value, 0);

	value_ = value.tv_sec * TIME_SECOND + value.tv_usec;
}

Date::Date(Date const& _date)
:	value_(_date.value_)
{
}

Date::Date(time_t _time)
{
	value_ = _time * TIME_SECOND;
}

Date::Date(std::string const& _date)
{
	struct tm	tm;

	if (_date.find('-') != std::string::npos)
	{
		if (strptime(_date.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL)
		{
			THROW_INVALID_ARGUMENT("Invalid time format[" << _date << "]");	
		}

		time_t time = mktime(&tm); 
		value_ = time * TIME_SECOND;
	}
	else
	{
		value_ = strtoul(_date.c_str(), 0, 10) * TIME_SECOND;
	}
}

bool	Date::IsValid() const
{
	return	(value_ != 0);
}

void	Date::Set(uint32_t _seconds)
{
	value_ = _seconds * TIME_SECOND;
}

Date	Date::operator+(const Time& _time)
{
	Date	date;

	date.value_ = value_ + _time.value_;

	return	date;
}

Time	Date::operator-(const Date& _date)
{
	if(value_ >= _date.value_)
	{
		uint64_t	diff;

		diff = value_ - _date.value_;
	std::cout << "REF2 : " << value_ << std::endl;
	std::cout << "CUR2 : " << _date.value_ << std::endl;
	std::cout << "REMAIN2 : " << diff << std::endl;
		return	Time(diff);	
	}

	return	Time(0);
}

Date	Date::operator-(const Time& _time)
{
	Date date;

	if(value_ >= _time.value_)
	{
		date.value_ = value_ - _time.value_;	
	}
	else
	{
		date.value_ = 0;
	}

	return	date;
}

const	Date&	Date::operator-=(const Time& _time)
{
	if (value_ >= _time.value_)
	{
		value_ -= _time.value_;
	}
	else
	{
		value_ = 0;
	}

	return	*this;
}

const	Date&	Date::operator+=(const Time& _time)
{
	value_ += _time.value_;

	return	*this;
}

const	Date&	Date::operator=(Date const& _value)
{
	value_ = _value.value_;

	return	*this;
}

const	Date&	Date::operator=(std::string const& _date)
{
	struct tm	tm;

	if (_date.find('-') != std::string::npos)
	{
		if (strptime(_date.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL)
		{
			THROW_INVALID_ARGUMENT("Invalid time format[" << _date << "]");	
		}

		time_t time = mktime(&tm); 
		value_ = time * TIME_SECOND;
	}
	else
	{
		value_ = strtoul(_date.c_str(), 0, 10) * TIME_SECOND;
	}

	return	*this;
}

Date::operator time_t() const
{
	uint64_t	second = value_ / TIME_SECOND;
	time_t	time = (time_t)second;

	return	time;
}

uint64_t	Date::GetMicroSecond() const
{
	return	value_;
}

const string	Date::ToString() const
{
 	char    buffer[64];
	time_t	time = time_t(value_ / TIME_SECOND);

   	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&time));

	return	string(buffer);
}

Date	Date::GetCurrent()
{
	Date	date;

	return	date;
}

bool	Compare(Date const &_date_a, Date const &_date_b)
{
	return	(time_t(_date_a) < time_t(_date_b));
}

std::ostream&	operator<<(std::ostream& os, Date const& _date) 
{
	return	os << _date.ToString();
}
