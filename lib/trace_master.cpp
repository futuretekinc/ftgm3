#include <iostream>
#include <iomanip>
#include <fstream>
#include <libjson/libjson.h>
#include "defined.h"
#include "object.h"
#include "trace.h"
#include "time2.h"

using namespace std;

extern	char*	program_invocation_short_name;
TraceMaster	trace_master;
Trace		trace;

TraceMaster::TraceMaster()
: level_(INFO), continue_(false), mode_(TO_FILE), enable_(false), locker_()
{
	out_ = &std::cout;
	file_path_ = std::string(DEFAULT_CONST_LOG_FILE_PATH);	
	file_prefix_= std::string(program_invocation_short_name);
	file_name_ = file_path_ + file_prefix_ + (".log");
	file_size_ = DEFAULT_CONST_LOG_FILE_SIZE;
	function_name_len_ 	= 32;
	object_name_len_ 	= 16;
	function_line_len_ 	= 4;
}

TraceMaster::~TraceMaster()
{
}

bool	TraceMaster::Load(JSONNode const& _properties, bool _check)
{
	bool	ret_value = true;

	if (_properties.type() != JSON_NODE)
	{
		TRACE_ERROR("Invalid json format");
		ret_value = false;
	}
	else
	{
		for(JSONNode::const_iterator it = _properties.begin() ; it != _properties.end() ; it++)
		{
			if (it->name() == TITLE_NAME_ENABLE)
			{
				if ((it->as_string() == "yes") ||  (it->as_string() == "on"))
				{
					SetEnable(true);
				}
				else if ((it->as_string() == "no") ||  (it->as_string() == "off"))
				{
					SetEnable(false);
				}
			}
			else if (it->name() == TITLE_NAME_FILE_PATH)
			{
				file_path_ = it->as_string();
				file_name_ = file_path_ + file_prefix_ + (".log");
			}
			else if (it->name() == TITLE_NAME_FILE_SIZE)
			{
				SetFileSize(it->as_int());
			}
		}
	}

	return	ret_value;
}

bool	TraceMaster::SetFileSize(uint32_t _size)
{
	TRACE_INFO("Trace file size is changed from " << file_size_ << " to " << _size);
	file_size_ = _size;
	return	true;
}

TraceMaster::operator JSONNode() const
{
	JSONNode	root;

	root.push_back(JSONNode("enable", ((enable_)?"yes":"no")));
	root.push_back(JSONNode("file_path", file_path_));
	root.push_back(JSONNode("file_prefix", file_prefix_));
	root.push_back(JSONNode("file_size", file_size_));

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
	locker_.Lock();

	switch(mode_)
	{
	case	TO_FILE:
		{
			std::ofstream	ofs;
			std::ostringstream	oss;
			istringstream	message(_log);
			bool		first = true;

			ofs.open(file_name_.c_str(), std::ofstream::out | std::ofstream::app | std::ofstream::ate );
			uint32_t	size = ofs.tellp();
			
			if (size + _headline_len + _log.length() > file_size_)
			{
				ofs.close();

				for(uint32_t i = 10 ; i > 0 ; i--)
				{
					oss.str("");
					oss.clear();
					oss << "mv -f " << file_name_ << "." << i-1 << " " << file_name_ << "." << i << " > /dev/null 2>&1 ";

					system(oss.str().c_str());
				}
				
				oss.str("");
				oss.clear();
				oss << "mv -f " << file_name_ << " " << file_name_ << ".0" << " > /dev/null 2>&1 ";

				system(oss.str().c_str());

				ofs.open(file_name_.c_str(), ofstream::out | ofstream::app);
			}

			while(!message.eof())
			{
				char	buffer[1024];

				memset(buffer, 0, sizeof(buffer));
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

				memset(buffer, 0, sizeof(buffer));
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
	
	locker_.Unlock();
}
