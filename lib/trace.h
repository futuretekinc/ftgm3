#ifndef	TRACE_H_
#define	TRACE_H_

#include <sstream>
#include <streambuf>

class	Object;
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

	Trace(Object* _object = NULL);
	~Trace();

	bool	IsEnable()	{	return	enable_;	};
	void	Enable(bool on);

	Trace& 	Begin(Trace::Level _level, std::string const& _pretty_function, uint32_t line);
	Trace& 	Begin(Trace::Level _level, std::string const& _pretty_function, uint32_t line, Object* _object);
	Trace&	Dump(Trace::Level _level, std::string const& _pretty_function, uint32_t line, const char* _buffer, uint32_t _length);
	Trace&	Dump(Trace::Level _level, std::string const& _pretty_function, uint32_t line, Object* _object, const char* _buffer, uint32_t _length);

	bool	SetClassName(std::string const& _class_name);
	bool	SetOut(std::ostream* _out);
	bool	SetOut(std::string const& _file, uint32_t ulSize = 1024*1024);
	bool	ConsoleMode();

	Trace& operator<< (std::streambuf* sb );
    Trace& operator<< (std::ostream& (*pf)(std::ostream&));
    Trace& operator<< (std::ios& (*pf)(std::ios&));
 	Trace& operator<< (std::ios_base& (*pf)(std::ios_base&));

	static std::ostream& End(std::ostream& _trace);
	static std::ostream& NL(std::ostream& _trace);

protected:

	void	Write(std::string const& _headline, uint32_t _headline_max, std::string const& _log);

	Object*			object_;
	std::ostream*	out_;
	bool			enable_;
	Mode			mode_;
	std::string		file_name_;
	uint32_t		file_size_;
	std::string		class_name_;
	Level			level_;
	bool			continue_;
	std::string		headline_;
	uint32_t		object_name_len_;
	uint32_t		function_name_len_;
	uint32_t		function_line_len_;
};

extern	Trace	trace;

#define	TRACE_INFO(x)	{	trace.Begin(Trace::INFO, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End; }
#define	TRACE_WARN(x)	{	trace.Begin(Trace::WARNING, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End; }
#define	TRACE_ERROR(x)	{	trace.Begin(Trace::ERROR, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End;}
#define	TRACE_CRITICAL	{	trace.Begin(Trace::CRITICAL, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End;}
#define	TRACE_INFO_DUMP(buffer, len)	{	trace.Dump(Trace::INFO, __PRETTY_FUNCTION__, __LINE__, buffer, len) << Trace::End;	}

#define	TRACE_INFO2(x, y)	{	::trace.Begin(Trace::INFO, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }
#define	TRACE_WARN2(x, y)	{	::trace.Begin(Trace::WARNING, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }
#define	TRACE_ERROR2(x, y)	{	::trace.Begin(Trace::ERROR, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }
#define	TRACE_CRITICAL2(x, y){	::trace.Begin(Trace::CRITICAL, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }

#define	TRACE_CREATE	TRACE_INFO("Create : " << this->GetClassName())
#define	TRACE_ENTRY		TRACE_INFO("Entry")
#define	TRACE_ENTRY2(x)	TRACE_INFO2(x, "Entry")
#define	TRACE_EXIT		TRACE_INFO("Exit")
#define	TRACE_EXIT2(x)	TRACE_INFO2(x, "Exit")

#endif
