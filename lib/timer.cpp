#include"timer.h"

Timer::Timer()
:	reference_date_(), time_(0)
{
}

void	Timer::Set(const Date& _date)
{
	reference_date_ = _date;
}

Time	Timer::RemainTime()
{
	Date	current;

	return	reference_date_ - current;
}

Time	Timer::OverTime()
{
	Date	current;

	return	current - reference_date_;
}

const Timer& Timer::operator+=(const Time& _time)
{
	reference_date_ += _time;	

	return *this;
}

