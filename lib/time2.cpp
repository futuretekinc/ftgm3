#include <sys/time.h>
#include "time2.h"

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
	return	value_ / 1000;
}

uint32_t	Time::GetSeconds() const
{
	return	value_ / 1000000;
}

Time::operator uint64_t()
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

//////////////////////////////////////////////////////////////
// Class Date
//////////////////////////////////////////////////////////////

Date::Date()
{
 	char    buffer[64];
	struct timeval	value;

	gettimeofday(&value, 0);

   	strftime(buffer, sizeof(buffer), "%Y%m%d-%H:%M:%S", localtime(&value.tv_sec));
    
	string_value_ = buffer;

	value_ = value.tv_sec * 1000000 + value.tv_usec;
}

Date::Date(const Date& _date)
:	value_(_date.value_)
{
 	char    buffer[64];
	time_t	time = (time_t)value_ / 1000000;

   	strftime(buffer, sizeof(buffer), "%Y%m%d-%H:%M:%S", localtime(&time));

	string_value_ = buffer;
}

Date::Date(uint32_t _seconds)
{
 	char    buffer[64];
	time_t	time = (time_t)_seconds;

	value_ = _seconds * 1000000;

   	strftime(buffer, sizeof(buffer), "%Y%m%d-%H:%M:%S", localtime(&time));

	string_value_ = buffer;
}

void	Date::Set(uint32_t _seconds)
{
 	char    buffer[64];
	time_t	time = (time_t)_seconds;

	value_ = _seconds * 1000000;

   	strftime(buffer, sizeof(buffer), "%Y%m%d-%H:%M:%S", localtime(&time));

	string_value_ = buffer;
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
		return	Time(value_ - _date.value_);	
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

const string&	Date::ToString() const
{
	return	string_value_;	
}

Date	Date::GetCurrentDate()
{
	Date	date;
	struct timeval	time;

	gettimeofday(&time, 0);

	return	Date(time.tv_sec * (uint64_t)1000000 + time.tv_usec);
}

