#include"timer.h"

Timer::Timer()
:	reference_date_(), time_(0)
{
}

void	Timer::Set(const Date& _date)
{
	reference_date_ = _date;
}

Time	Timer::RemainTime() const
{
	Date	current;

	return	reference_date_ - current;
}

Time	Timer::OverTime() const
{
	Date	current;

	return	current - reference_date_;
}

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
