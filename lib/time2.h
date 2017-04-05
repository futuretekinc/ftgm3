#ifndef	TIME2_H_
#define	TIME2_H_

#include <sys/time.h>
#include <cstdint>
#include <string>

class	Time
{
public:
	Time();
	Time(const Time& _time);
	Time(uint64_t _microseconds);


			void		Set(uint64_t _microseconds);
			uint64_t	Get() const;
			uint64_t	GetMilliseconds() const;
			uint32_t	GetSeconds() const;
	
						operator uint64_t();
			Time		operator+(const Time& _time);
			Time		operator-(const Time& _time);
	const 	Time&		operator+=(const Time& _time);
	const 	Time&		operator-=(const Time& _time);


protected:
	friend	class	Date;
	friend	class	Timer;

	uint64_t	value_;	
};

class	Date
{
public:
	Date();
	Date(const Date& _date);
	Date(uint32_t _seconds);

			void	Set(uint32_t _seconds);

	virtual	const std::string&	ToString() const;

			Date	operator+(const Time& _time);
			Date	operator-(const Time& _time);
			Time	operator-(const Date& _date);
	const	Date&	operator-=(const Time& _time);
	const	Date&	operator+=(const Time& _time);

	static	Date	GetCurrentDate();

protected:
	uint64_t	value_;
	std::string	string_value_;
};
#endif
