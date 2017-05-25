#ifndef	TRACE_H_
#define	TRACE_H_

#include <sstream>
#include <streambuf>
#include <libjson/libjson.h>

class	Object;

enum	TraceMode
{
	TO_FILE,
	TO_CONSOLE
};

enum 	TraceLevel
{
	INFO,
	WARNING,
	ERROR,
	CRITICAL
};

class	TraceMaster : public std::ostringstream
{
public:
	friend	class	Trace;

	TraceMaster();
	~TraceMaster();

	bool	GetEnable()	{	return	enable_;	};
	void	SetEnable(bool on);

	bool	Load(JSONNode const& _json);
			operator JSONNode() const;

	uint32_t	GetFunctionNameSize()	{	return	function_name_len_;	};
	uint32_t	GetFunctionLineSize()	{	return	function_line_len_;	};
	uint32_t	GetObjectNameSize()		{	return	object_name_len_;	};

	TraceMode	GetMode()				{	return	mode_;	}

	bool	SetOut(std::ostream* _out);
	bool	SetOut(std::string const& _file, uint32_t ulSize = 1024*1024);
	bool	ConsoleMode();

protected:
	void	Write(std::string const& _headline, uint32_t _headline_max, std::string const& _log);

	std::ostream*	out_;
	bool			enable_;
	TraceMode		mode_;
	std::string		file_name_;
	uint32_t		file_size_;
	TraceLevel		level_;
	bool			continue_;
	std::string		headline_;
	uint32_t		object_name_len_;
	uint32_t		function_name_len_;
	uint32_t		function_line_len_;
};

class	Trace : public std::ostringstream
{
public:
	Trace(Object* _object = NULL);
	~Trace();

	enum	State
	{	
		UNDEFINED,
		ENABLE,
		DISABLE
	};

	bool	GetEnable();
	void	SetEnable(bool on);

	bool	Load(JSONNode const& _json);
			operator JSONNode() const;

	Trace& 	Begin(TraceLevel _level, std::string const& _pretty_function, uint32_t line);
	Trace& 	Begin(TraceLevel _level, std::string const& _pretty_function, uint32_t line, Object* _object);
	Trace&	Dump(TraceLevel _level, std::string const& _pretty_function, uint32_t line, const char* _buffer, uint32_t _length);
	Trace&	Dump(TraceLevel _level, std::string const& _pretty_function, uint32_t line, Object* _object, const char* _buffer, uint32_t _length);

	bool	SetClassName(std::string const& _class_name);

	Trace& operator<< (std::streambuf* sb );
    Trace& operator<< (std::ostream& (*pf)(std::ostream&));
    Trace& operator<< (std::ios& (*pf)(std::ios&));
 	Trace& operator<< (std::ios_base& (*pf)(std::ios_base&));

	static std::ostream& End(std::ostream& _trace);
	static std::ostream& NL(std::ostream& _trace);

protected:

	TraceMaster&	master_;
	Object*			object_;
	State			state_;
	std::string		class_name_;
	TraceLevel		level_;
	bool			continue_;
	std::string		headline_;
};

extern	TraceMaster	trace_master;
extern	Trace		trace;

#define	TRACE_INFO(x)	{	trace.Begin(TraceLevel::INFO, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End; }
#define	TRACE_WARN(x)	{	trace.Begin(TraceLevel::WARNING, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End; }
#define	TRACE_ERROR(x)	{	trace.Begin(TraceLevel::ERROR, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End;}
#define	TRACE_CRITICAL	{	trace.Begin(TraceLevel::CRITICAL, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End;}
#define	TRACE_INFO_DUMP(buffer, len)	{	trace.Dump(TraceLevel::INFO, __PRETTY_FUNCTION__, __LINE__, buffer, len) << Trace::End;	}

#define	TRACE_INFO2(x, y)	{	::trace.Begin(TraceLevel::INFO, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }
#define	TRACE_WARN2(x, y)	{	::trace.Begin(TraceLevel::WARNING, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }
#define	TRACE_ERROR2(x, y)	{	::trace.Begin(TraceLevel::ERROR, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }
#define	TRACE_CRITICAL2(x, y){	::trace.Begin(TraceLevel::CRITICAL, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }

#define	TRACE_CREATE	TRACE_INFO("Create : " << this->GetClassName())
#define	TRACE_ENTRY		TRACE_INFO("Entry")
#define	TRACE_ENTRY2(x)	TRACE_INFO2(x, "Entry")
#define	TRACE_EXIT		TRACE_INFO("Exit")
#define	TRACE_EXIT2(x)	TRACE_INFO2(x, "Exit")

#endif
