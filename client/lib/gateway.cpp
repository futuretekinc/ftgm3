#include <iostream>
#include <iomanip>
#include <libjson/libjson.h>
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "device.h"
#include "time2.h"
#include "rcs_client.h"
#include "utils.h"

using namespace std;

RetValue	ShellCommandGateway
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* _shell
)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	try
	{
		if (_count == 1)
		{

		}
		else if (_arguments[1] == MSG_TYPE_RCS_ADD)
		{
			JSONNode	properties;

			if ((_count < 4) || (_count % 2 != 0))
			{
				throw InvalidArgument("Invalid argument");	
			}

			for(uint32_t i = 2 ; i+1 < _count ; i+=2)
			{
				if (_arguments[i].substr(0, 2) != "--")
				{
					throw InvalidArgument("Invalid argument");	
				}

				properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].length() - 2), _arguments[i+1]));
			}

			if (client->AddGateway(properties))
			{
				_shell->Out() << "The gateway has been added." << std::endl;		
			}
			else
			{
				_shell->Out() << "Failed to add gateway gateway." << std::endl;		
			}
		}
		else if (_arguments[1] == MSG_TYPE_RCS_DEL)
		{
			if (_count < 3)
			{
				throw InvalidArgument("Invalid argument");	
			}

			for(uint32_t i = 2 ; i < _count ; i++)
			{
				if (client->DelGateway(_arguments[i]))
				{
					_shell->Out() << "The gateway[" << _arguments[i] << "] has been deleted." << std::endl;		
				}
				else
				{
					_shell->Out() << "Faild to delete gateway[" << _arguments[i] << "]" << std::endl;		
				}
			}
		}
		else if (_arguments[1] == MSG_TYPE_RCS_GET)
		{
			if (_count < 3)
			{
				throw InvalidArgument("Invalid argument");	
			}

			for(uint32_t i = 2 ; i < _count ; i++)
			{
				JSONNode	properties;

				if (client->GetGateway(_arguments[i], properties))
				{
					_shell->Out() << properties.write_formatted() << std::endl;
				}
				else
				{
					_shell->Out() << "Failed to get gateway[" << _arguments[i] <<" properties." << std::endl;
				}

			}
		}
		else if (_arguments[1] == MSG_TYPE_RCS_SET)
		{
			JSONNode	properties;

			if ((_count < 5) || (_count % 2 != 1))
			{
				throw InvalidArgument("Invalid argument");	
			}

			properties.push_back(JSONNode(TITLE_NAME_ID, _arguments[2]));

			for(uint32_t i = 3 ; i+1 < _count ; i+=2)
			{
				if (_arguments[i].substr(0, 2) != "--")
				{
					throw InvalidArgument("Invalid argument");	
				}

				properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].length() - 2), _arguments[i+1]));
			}

			if (client->SetGateway(properties))
			{
				_shell->Out() << "The gateway[" << _arguments[2] << "] properties changed." << std::endl;	
			
			}
			else
			{
				_shell->Out() << "Failed to set gateway[" << _arguments[2] << "] properties." << std::endl;	
			}
		}
		else if (_arguments[1] == MSG_TYPE_RCS_LIST)
		{
			std::vector<JSONNode>	_vector;

			if (client->GetGateway(_vector))
			{
				for(uint32_t i = 0 ; i < _vector.size() ; i++)
				{
					_shell->Out() << _vector[i].write_formatted() << std::endl;
				}
			}
			else
			{
				_shell->Out() << "Failed to get gateway list." << std::endl;
			}
		}
	}
	catch(InvalidArgument& e)
	{
		_shell->Out() << "Invalid argument" << std::endl;	
	}

	return	RET_VALUE_OK;
}

Shell::Command	object_manager_command_gateway =
{
	.command	= "gateway",
	.help		= "gateway",
	.short_help	= "gateway",
	.function	= ShellCommandGateway
};
