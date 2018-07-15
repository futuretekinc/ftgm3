#include <iostream>
#include <iomanip>
#include <fstream>
#include <libjson/libjson.h>
#include "defined.h"
#include "object.h"
#include "trace.h"
#include "time2.h"

using namespace std;

Locker	global_locker;

Trace::Trace(Object* _object)
: master_(trace_master), object_(_object), level_(INFO), continue_(false), state_(ENABLE), locker_()
{
}

Trace::~Trace()
{
}

bool	Trace::SetClassName(std::string const& _class_name)
{
	class_name_ = _class_name;

	return	true;
}

bool	Trace::Load(JSONNode const& _json, bool _check)
{
	bool	ret_value = true;

	if (_json.name() == TITLE_NAME_ENABLE)
	{
		if ((_json.as_string() == "yes") ||  (_json.as_string() == "on"))
		{
			if (!_check)
			{
				SetEnable(true);
			}
		}
		else if ((_json.as_string() == "no") ||  (_json.as_string() == "off"))
		{
			if (!_check)
			{
				SetEnable(false);
			}
		}
	}
	else if (_json.type() == JSON_NODE)
	{
		for(JSONNode::const_iterator it = _json.begin(); it != _json.end() ; it++)
		{	
			ret_value = Load(*it, _check);
			if (ret_value != true)
			{
				break;
			}
		}
	}
	else 
	{
		TRACE_ERROR("Invalid json format");
		ret_value = false;
	}

	return	ret_value;
}
		
Trace::operator JSONNode() const
{
	JSONNode	root;

	if (state_ != UNDEFINED)
	{
		root.push_back(JSONNode(TITLE_NAME_ENABLE, ((state_ == ENABLE)?"yes":"no")));
	}

	return	root;
}

bool	Trace::GetEnable()
{
	if(state_ == ENABLE)
	{
		return	true;
	}
	else if (state_ == DISABLE)
	{
		return	false;
	}

	if (object_ != NULL)
	{
		Object* parent = Object::Get(object_->GetParentID());
		if (parent != NULL)
		{
			return	parent->trace.GetEnable();
		}
	}

	return	master_.GetEnable();	
}

void	Trace::SetEnable(bool _enable)
{
	if (_enable)
	{
		state_ = ENABLE;
	}
	else
	{
		state_ = DISABLE;
	}
}

bool	Trace::SetState(State _state)
{
	state_ = _state;
}

Trace::State Trace::GetState()
{
	return	state_;
}

Trace& Trace::Begin(TraceLevel _level, std::string const& _pretty_function, uint32_t _line)
{
	level_ = _level;
	headline_ = "";

	size_t colons, begin, end;

	colons = _pretty_function.find("::");
	if (colons != _pretty_function.npos)
	{
		begin = _pretty_function.substr(0,colons).rfind(" ") + 1;
	}
	else
	{
		begin = _pretty_function.substr(0,colons).find(" ");
	}

	end = _pretty_function.rfind("(") - begin;

	std::string function = _pretty_function.substr(begin,end);
	ostringstream	os;

	Date	date;
//	os << "[" << date << "." << std::setw(6) << std::setfill('0') << date.GetMicroSecond() % 1000000 << std::setfill(' ') << "]";
	os << "[" << setw(master_.GetFunctionNameSize()) << function.substr(0, master_.GetFunctionNameSize()) << "]";
	os << "[" << setw(master_.GetFunctionLineSize()) <<_line << "]";
#if 0
	if (object_ != NULL)
	{
		std::string	name = object_->GetName();

		if (master_.GetObjectNameSize() >= name.size())
		{
			os << "[" << setw(master_.GetObjectNameSize()) <<name << "]";
		}
		else if(master_.GetObjectNameSize() < 5)
		{
			os << "[" << setw(master_.GetObjectNameSize()) <<name.substr(0, 4) << "]";
		}
		else
		{
			int begin = (master_.GetObjectNameSize() - 3) / 2;
			int end   = master_.GetObjectNameSize() - begin - 3;
			os << "[" << setw(begin) << name.substr(0, begin) << "..." << name.substr(name.size() - end, end) << "]";
		}
	}
	else
	{
		os << "[" << setw(master_.GetObjectNameSize()) << "global" << "]";
	}
#endif
	switch(level_)
	{
	case	INFO:	os << "[INFO]"; break;
	case	WARNING:os << "[\033[1;33mWARN\033[0m]"; break;
	case	ERROR:	os << "[\033[0;31mERRO\033[0m]"; break;
	case	CRITICAL:os <<"[\033[1;31mCRIT\033[0m]"; break;
	}

	os << " : ";

	headline_ = os.str();

	return	*this;
}

Trace& Trace::Begin(TraceLevel _level, std::string const& _pretty_function, uint32_t _line, Object *_object)
{
	if (_object != NULL)
	{
		return	_object->trace.Begin(_level, _pretty_function, _line);
	}

	return	Begin(_level, _pretty_function, _line);
}

Trace&	Trace::Dump(TraceLevel _level, std::string const& _pretty_function, uint32_t line, const char* _buffer, uint32_t _length)
{
	bool	binary_message = false;

	try
	{
		if (libjson::is_valid(_buffer))
		{
			JSONNode	json = libjson::parse(_buffer);

			Begin(_level, _pretty_function, line) << json.write_formatted() << Trace::End;
		}
		else
		{
			binary_message = true;
		}
	}
	catch(std::invalid_argument)
	{
		binary_message = true;
	}

	if (binary_message)
	{
		for(uint32_t i = 0 ; i < _length ; i++)
		{
			if ((i % 16) == 0)
			{
				Begin(_level, _pretty_function, line) << std::hex << std::setw(4) << std::setfill('0') << i << " : ";
			}

			Begin(_level, _pretty_function, line) << std::hex << std::setw(2) << std::setfill('0') << (int)((uint8_t *)_buffer)[i] << " ";
			if ((i + 1) % 16 == 0)
			{
				Begin(_level, _pretty_function, line) << std::oct << Trace::End;	
			}
		}
		Begin(_level, _pretty_function, line) << std::oct << Trace::End;	
	}

	return	*this;
}

Trace&	Trace::Dump(TraceLevel _level, std::string const& _pretty_function, uint32_t line, Object* _object, const char* _buffer, uint32_t _length)
{
	if (_object != NULL)
	{
		return	_object->trace.Dump(_level, _pretty_function, line, _buffer, _length);
	}

	return	Dump(_level, _pretty_function, line, _buffer, _length);
}

std::ostream& Trace::End(std::ostream& _trace)
{
	Trace* trace = dynamic_cast<Trace*>(&_trace);
	if (trace != NULL)
	{
		if (trace->GetEnable() && (trace->str().size() != 0))
		{
			trace_master.Write(trace->headline_, trace->headline_.size(), trace->str());
			trace->str("");
		}

		trace->continue_ 	= false;
	}

	_trace << std::setw(0);

	return	_trace;
}

std::ostream& Trace::NL(std::ostream& _trace)
{
	Trace* trace = dynamic_cast<Trace*>(&_trace);
	if (trace != NULL)
	{
		if (trace->GetEnable() && (trace->str().size() != 0))
		{
			trace_master.Write("", trace->headline_.size(), trace->str());
			trace->str("");
		}

		trace->continue_ = true;
	}

	return	_trace;
}

Trace& Trace::operator<< (streambuf* sb )
{
	ostringstream::operator<<(sb);	

	return	*this;
}

Trace& Trace::operator<< (ostream& (*pf)(ostream&))
{
	ostringstream::operator<<(pf);	

	return	*this;
}

Trace& Trace::operator<< (ios& (*pf)(ios&))
{
	ostringstream::operator<<(pf);	

	return	*this;
}

Trace& Trace::operator<< (ios_base& (*pf)(ios_base&))
{
	ostringstream::operator<<(pf);	

	return	*this;
}

