#ifndef	TRACE_H_
#define	TRACE_H_

#include <ostream>

class	Trace : public std::ostream
{
public:
	Trace();
	~Trace();
};

//extern	Trace	trace_info;
//extern	Trace	trace_error;
extern	std::ostream&	trace_info;
extern	std::ostream&	trace_error;
#endif
