#ifndef	TRACE_H_
#define	TRACE_H_

#include <sstream>
#include <streambuf>

class	Trace : public std::ostringstream
{
public:
	enum	Mode
	{
		TO_FILE,
		TO_CONSOLE
	};

	enum 	Level
	{
		INFO,
		WARNING,
		ERROR,
		CRITICAL
	};

	Trace();
	~Trace();

	Trace& 	Begin(Trace::Level _level, std::string const& _pretty_function, uint32_t line);
	Trace&	Dump(Trace::Level _level, std::string const& _pretty_function, uint32_t line, const char* _buffer, uint32_t _length);

	bool	SetClassName(std::string const& _class_name);
	bool	SetOut(std::ostream* _out);
	bool	SetOut(std::string const& _file, uint32_t ulSize = 1024*1024);

	Trace& operator<< (std::streambuf* sb );
    Trace& operator<< (std::ostream& (*pf)(std::ostream&));
    Trace& operator<< (std::ios& (*pf)(std::ios&));
 	Trace& operator<< (std::ios_base& (*pf)(std::ios_base&));

	static std::ostream& End(std::ostream& _trace);
	static std::ostream& NL(std::ostream& _trace);

protected:

	void	Write(std::string const& _headline, uint32_t _headline_max, std::string const& _log);

	std::ostream*	out_;
	Mode			mode_;
	std::string		file_name_;
	uint32_t		file_size_;
	std::string		class_name_;
	Level			level_;
	bool			continue_;
	std::string		headline_;
	uint32_t		function_name_len_;
	uint32_t		function_line_len_;
};

extern	Trace	trace;

#define	TRACE_INFO		trace.Begin(Trace::INFO, __PRETTY_FUNCTION__, __LINE__)
#define	TRACE_WARN		trace.Begin(Trace::WARNING, __PRETTY_FUNCTION__, __LINE__)
#define	TRACE_ERROR		trace.Begin(Trace::ERROR, __PRETTY_FUNCTION__, __LINE__)
#define	TRACE_CRITICAL	trace.Begin(Trace::CRITICAL, __PRETTY_FUNCTION__, __LINE__)
#define	TRACE_INFO_DUMP(buffer, len)	trace.Dump(Trace::INFO, __PRETTY_FUNCTION__, __LINE__, buffer, len)

#define	TRACE_INFO2		::trace.Begin(Trace::INFO, __PRETTY_FUNCTION__, __LINE__)
#define	TRACE_WARN2		::trace.Begin(Trace::WARNING, __PRETTY_FUNCTION__, __LINE__)
#define	TRACE_ERROR2	::trace.Begin(Trace::ERROR, __PRETTY_FUNCTION__, __LINE__)
#define	TRACE_CRITICAL2	::trace.Begin(Trace::CRITICAL, __PRETTY_FUNCTION__, __LINE__)

#define	TRACE_CREATE	{	TRACE_INFO <<  "Create : " << this->GetClassName() << Trace::End; }
#define	TRACE_ENTRY		{	TRACE_INFO <<  "Entry" << Trace::End; }
#define	TRACE_ENTRY2	{	TRACE_INFO2 <<  "Entry" << Trace::End; }
#endif
