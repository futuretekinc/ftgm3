#ifndef	TIMER_H_
#define	TIMER_H_

#include"time2.h"

class	Timer
{
public:
	Timer();

	void		Set(const Date& _date);
	const Date&	ReferenceTime() const;
	Time		RemainTime() const;
	Time		OverTime() const;

	bool		Add(double _seconds);
	bool		Add(uint32_t _seconds);
	bool		Add(uint32_t _seconds, uint32_t _microseconds);
//	const Timer&	operator+=(const Time& _time);
//	const Timer&	operator+=(const uint64_t _microseconds);

//protected:
	Date	reference_date_;
	Time	time_;	

};

#endif
