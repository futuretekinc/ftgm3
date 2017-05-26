#include <iostream>
#include <iomanip>
#include <fstream>
#include <libjson/libjson.h>
#include "defined.h"
#include "object.h"
#include "trace.h"
#include "time2.h"

using namespace std;

TraceMaster	trace_master;
Trace		trace;

TraceMaster::TraceMaster()
: level_(INFO), continue_(false), mode_(TO_FILE), enable_(false)
{
	out_ = &std::cout;
	file_name_ = std::string(DEFAULT_CONST_LOG_FILE_PATH) + std::string(program_invocation_short_name) + (".log");
	file_size_ = DEFAULT_CONST_LOG_FILE_SIZE;
	function_name_len_ 	= 32;
	object_name_len_ 	= 16;
	function_line_len_ 	= 4;
}

TraceMaster::~TraceMaster()
{
}

bool	TraceMaster::Load(JSONNode const& _json)
{
	bool	ret_value = true;

	if (_json.name() == TITLE_NAME_ENABLE)
	{
		if ((_json.as_string() == "yes") ||  (_json.as_string() == "on"))
		{
			SetEnable(true);
		}
		else if ((_json.as_string() == "no") ||  (_json.as_string() == "off"))
		{
			SetEnable(false);
		}
	}
	else if (_json.name() == TITLE_NAME_PATH)
	{
		file_name_ = _json.as_string() + std::string(program_invocation_short_name) + (".log");
	}
	else if (_json.name() == TITLE_NAME_SIZE)
	{
		file_size_ = _json.as_int();
	}
	else if (_json.type() == JSON_NODE)
	{
		for(auto it = _json.begin(); it != _json.end() ; it++)
		{	
			ret_value = Load(*it);
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
		
TraceMaster::operator JSONNode() const
{
	JSONNode	root;

	root.push_back(JSONNode("enable", ((enable_)?"yes":"no")));

	return	root;
}

void	TraceMaster::SetEnable(bool _enable)
{
	enable_ = _enable;
}

bool   TraceMaster::SetOut(std::ostream* _out)
{
	out_ = _out;

	return	true;
}

bool	TraceMaster::SetOut(std::string const& _file, uint32_t ulSize)
{
	mode_ = TO_FILE;
	file_name_ = _file;
	file_size_ = ulSize;
	
	return	true;
}

bool	TraceMaster::ConsoleMode()
{
	mode_ = TO_CONSOLE;
	return	true;
}

void	TraceMaster::Write(std::string const& _headline, uint32_t _headline_len, std::string const& _log)
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
