#ifndef	TIMER_H_
#define	TIMER_H_

#include"time2.h"

class	Timer
{
public:
	Timer();

	void		Set(const Date& _date);
	Time		RemainTime();
	Time		OverTime();

	const Timer&	operator+=(const Time& _time);

protected:
	Date	reference_date_;
	Time	time_;	

};

#endif
