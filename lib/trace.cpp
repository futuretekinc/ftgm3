#include <iostream>
#include <iomanip>
#include <fstream>
#include <libjson/libjson.h>
#include "defined.h"
#include "object.h"
#include "trace.h"
#include "time2.h"

using namespace std;

Trace	trace;

Trace::Trace(Object* _object)
: object_(_object), level_(INFO), continue_(false), mode_(TO_FILE), enable_(false)
{
	out_ = &std::cout;
	file_name_ = std::string(LOG_FILE_PATH) + std::string(program_invocation_short_name) + (".log");
	file_size_ = LOG_FILE_SIZE;
	function_name_len_ = 32;
	object_name_len_ = 16;
	function_line_len_ = 4;
}

Trace::~Trace()
{
}

bool	Trace::SetClassName(std::string const& _class_name)
{
	class_name_ = _class_name;

	return	true;
}

bool	Trace::Load(JSONNode const& _json)
{
	if (_json.type() == JSON_NODE)
	{
		for(auto it = _json.begin(); it != _json.end() ; it++)
		{	
			Load(*it);
		}
	}
	else if (_json.name() == "enable")
	{
		if ((_json.as_string() == "yes") &&  (_json.as_string() == "on"))
		{
			Enable(true);
		}
		else if ((_json.as_string() == "no") &&  (_json.as_string() == "off"))
		{
			Enable(false);
		}
	}

	return	true;
}
		
void	Trace::Enable(bool _enable)
{
	enable_ = _enable;
}

bool   Trace::SetOut(std::ostream* _out)
{
	out_ = _out;

	return	true;
}

bool	Trace::SetOut(std::string const& _file, uint32_t ulSize)
{
	mode_ = TO_FILE;
	file_name_ = _file;
	file_size_ = ulSize;
	
	return	true;
}

bool	Trace::ConsoleMode()
{
	mode_ = TO_CONSOLE;
	return	true;
}

Trace& Trace::Begin(Level _level, std::string const& _pretty_function, uint32_t _line)
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
	os << "[" << date << "]";
	os << "[" << setw(function_name_len_) << function.substr(0, function_name_len_) << "]";
	os << "[" << setw(function_line_len_) <<_line << "]";
	if (object_ != NULL)
	{
		std::string	name = object_->GetName();

		if (object_name_len_ >= name.size())
		{
			os << "[" << setw(object_name_len_) <<name << "]";
		}
		else if(object_name_len_ < 5)
		{
			os << "[" << setw(object_name_len_) <<name.substr(0, 4) << "]";
		}
		else
		{
			int begin = (object_name_len_ - 3) / 2;
			int end   = object_name_len_ - begin - 3;
			os << "[" << setw(begin) << name.substr(0, begin) << "..." << name.substr(name.size() - end, end) << "]";
		}
	}
	else
	{
		os << "[" << setw(object_name_len_) << "global" << "]";
	}
	switch(level_)
	{
	case	INFO:	os << "[INFO]"; break;
	case	WARNING:os << "[WARN]"; break;
	case	ERROR:	os << "[\033[1;31mERRO\033[0m]"; break;
	case	CRITICAL:os << "[CRIT]"; break;
	}

	os << " : ";

	headline_ = os.str();

	return	*this;
}

Trace& Trace::Begin(Level _level, std::string const& _pretty_function, uint32_t _line, Object *_object)
{
	if (_object != NULL)
	{
		return	_object->trace.Begin(_level, _pretty_function, _line);
	}

	return	Begin(_level, _pretty_function, _line);
}

Trace&	Trace::Dump(Trace::Level _level, std::string const& _pretty_function, uint32_t line, const char* _buffer, uint32_t _length)
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

Trace&	Trace::Dump(Trace::Level _level, std::string const& _pretty_function, uint32_t line, Object* _object, const char* _buffer, uint32_t _length)
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
		if (trace->IsEnable() && (trace->str().size() != 0))
		{
			trace->Write(trace->headline_, trace->headline_.size(), trace->str());
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
		if (trace->IsEnable() && (trace->str().size() != 0))
		{
			trace->Write("", trace->headline_.size(), trace->str());
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

void	Trace::Write(std::string const& _headline, uint32_t _headline_len, std::string const& _log)
{
	switch(mode_)
	{
	case	TO_FILE:
		{
			ofstream	ofs;
			istringstream	message(_log);
			bool		first = true;

			ofs.open(file_name_, ofstream::out | ofstream::app);

			while(!message.eof())
			{
				char	buffer[1024];

				message.getline(buffer, sizeof(buffer) - 1);
				if (first)
				{
					ofs << setw(_headline_len) << _headline;
					first = false;
				}
				else
				{
					ofs << setw(_headline_len) << setfill(' ') << "";
				}

				ofs << buffer << std::endl;
			}

			ofs.close();
		}
		break;

	case	TO_CONSOLE:
		{
			istringstream	message(_log);
			bool		first = true;

			while(!message.eof())
			{
				char	buffer[1024];

				message.getline(buffer, sizeof(buffer) - 1);
				if (first)
				{
					cout << setw(_headline_len) << _headline;
					first = false;
				}
				else
				{
					cout << setw(_headline_len) << setfill(' ') << "";
				}

				cout << buffer << std::endl;
			}
		}
		break;
	
	}
}
