#ifndef	TRACE_H_
#define	TRACE_H_

#include <stdint.h>
#include <sstream>
#include <streambuf>
#include <libjson/libjson.h>
#include "locker.h"

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

	bool		GetEnable()	{	return	enable_;	};
	void		SetEnable(bool on);

	uint32_t	GetFileSize()	{	return	file_size_;	}
	bool		SetFileSize(uint32_t _size);

	bool		Load(JSONNode const& _json, bool _check = false);
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

	Locker			locker_;
	std::ostream*	out_;
	bool			enable_;
	TraceMode		mode_;
	std::string		file_path_;
	std::string		file_prefix_;
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
	bool	SetState(State _state);
	Trace::State	GetState();

	bool	Load(JSONNode const& _json, bool _check = false);
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

	Locker			locker_;
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
extern	Locker		global_locker;

#define	TRACE_INFO(x)	{	global_locker.Lock(); trace.Begin(INFO, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End; global_locker.Unlock(); }
#define	TRACE_WARN(x)	{	trace.Begin(WARNING, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End; }
#define	TRACE_ERROR(x)	{	trace.Begin(ERROR, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End;}
#define	TRACE_CRITICAL	{	trace.Begin(CRITICAL, __PRETTY_FUNCTION__, __LINE__) << x << Trace::End;}
#define	TRACE_INFO_DUMP(buffer, len)	{	trace.Dump(INFO, __PRETTY_FUNCTION__, __LINE__, buffer, len) << Trace::End;	}
#define	TRACE_INFO_JSON(x)	{	JSONNode json = libjson::parse(x); trace.Begin(INFO, __PRETTY_FUNCTION__, __LINE__) << json.write_formatted() << Trace::End;	}

#define	TRACE_INFO2(x, y)	{	::trace.Begin(INFO, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }
#define	TRACE_WARN2(x, y)	{	::trace.Begin(WARNING, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }
#define	TRACE_ERROR2(x, y)	{	::trace.Begin(ERROR, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }
#define	TRACE_CRITICAL2(x, y){	::trace.Begin(CRITICAL, __PRETTY_FUNCTION__, __LINE__, x) << y << Trace::End; }
#define	TRACE_INFO_DUMP2(x, buffer, len)	{	::trace.Dump(INFO, __PRETTY_FUNCTION__, __LINE__, x, buffer, len) << Trace::End;	}

#define	TRACE_CREATE	TRACE_INFO("Create : " << this->GetClassName())
#define	TRACE_ENTRY		TRACE_INFO("Entry")
#define	TRACE_ENTRY2(x)	TRACE_INFO2(x, "Entry")
#define	TRACE_EXIT		TRACE_INFO("Exit")
#define	TRACE_EXIT2(x)	TRACE_INFO2(x, "Exit")

#endif
