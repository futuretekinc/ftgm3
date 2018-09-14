#include <iostream>
#include <iomanip>
#include "json.h"
#include "exception.h"
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "device.h"
#include "time2.h"
#include "rcs_client.h"
#include "utils.h"

using namespace std;

/*RetValue	ShellCommandData
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* _shell

)*/
RetValue	ShellCommandData(std::vector<std::string> const& _arguments, Shell* _shell, Shell::Command* _this)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	uint32_t	_count = _arguments.size();
	try
	{
		if (_count == 1)
		{

		}
		else if (_arguments[1] == MSG_TYPE_RCS_DEL)
		{
			if (_count < 3)
			{
				throw InvalidArgument("Invalid argument");	
			}

		}
		else if (_arguments[1] == MSG_TYPE_RCS_GET)
		{
			if (_count < 3)
			{
				throw InvalidArgument("Invalid argument");	
			}
			else if (_count == 4)
			{
				std::multimap<time_t, std::string>	value_map;

				uint32_t	count = strtoul(_arguments[3].c_str(), 0, 10);

				client->GetEPData(_arguments[2], count, value_map);
				
				for(std::multimap<time_t, std::string>::iterator it = value_map.begin();  it != value_map.end() ; it++)
				{
					_shell->Out() << it->first << " : " << it->second << std::endl;	
				}
			}
		}
		else if (_arguments[1] == MSG_TYPE_RCS_SET)
		{
			if (_count < 3)
			{
				throw InvalidArgument("Invalid argument");	
			}
			else if (_count == 4)
			{
				//client->SetEPData(_arguments[2], time_t(Date::GetCurrent()) + 10, _arguments[3]);
			}
		}
	}
	catch(InvalidArgument& e)
	{
		_shell->Out() << "Invalid argument" << std::endl;	
	}

	return	RET_VALUE_OK;
}

Shell::Command	object_manager_command_data 
(
	"data",
	"data",
	"data",
	ShellCommandData
);
