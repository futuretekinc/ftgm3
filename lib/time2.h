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
	
	virtual	std::string	ToString() const;

						operator uint64_t() const;
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
	friend	class	Timer;

public:
	Date();
	Date(Date const& _date);
	Date(uint32_t _seconds);

			bool	IsValid() const;
			void	Set(uint32_t _seconds);

	virtual	const std::string	ToString() const;

			Date	operator+(const Time& _time);
			Date	operator-(const Time& _time);
			Time	operator-(const Date& _date);
	const	Date&	operator-=(const Time& _time);
	const	Date&	operator+=(const Time& _time);
	const	Date&	operator=(Date const& _date);
	const	Date&	operator=(std::string const& _date);
					operator time_t() const;
			uint64_t	GetMicroSecond();

	friend	std::ostream&	::operator<<(std::ostream& os, Date const& _date) ;

	static	Date	GetCurrentDate();
	

protected:
	uint64_t	value_;
};

bool	Compare(Date const & _date_a, Date const& _date_b);
#endif
