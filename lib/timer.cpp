#include <iostream>
#include"timer.h"

Timer::Timer()
:	reference_date_(), time_(0)
{
}

void	Timer::Set(const Date& _date)
{
	reference_date_ = _date;
}

const Date&	Timer::ReferenceTime() const
{
	return	reference_date_;
}

Time	Timer::RemainTime() const
{
	Date	current;
	uint64_t	remain = 0;

	if (reference_date_.value_ > current.value_)
	{
		remain = reference_date_.value_ - current.value_;
	}

	return	Time(remain);
}

Time	Timer::OverTime() const
{
	Date	current;
	uint64_t	over= 0;

	if (current.value_ > reference_date_.value_)
	{
		over = current.value_ - reference_date_.value_;
	}

	return	Time(over);
}

bool	Timer::Add(double _seconds)
{
	uint64_t	value = (uint64_t)(_seconds * 1000000);

	reference_date_.Add(value / 1000000,  value % 1000000);

	return	true;
}

bool	Timer::Add(uint32_t _seconds)
{
	reference_date_.Add(_seconds);

	return	true;
}

bool	Timer::Add(uint32_t _seconds, uint32_t _microseconds)
{
	reference_date_.Add(_seconds, _microseconds);

	return	true;
}

/*
const Timer& Timer::operator+=(const Time& _time)
{
	reference_date_ += _time;	

	return *this;
}

const Timer& Timer::operator+=(uint64_t _microseconds)
{
	reference_date_ += Time(_microseconds);	

	return *this;
}
*/
