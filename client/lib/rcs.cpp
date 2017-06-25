#include <iostream>
#include <iomanip>
#include <libjson/libjson.h>
#include "shell.h"
#include "exception.h"
#include "object_manager.h"
#include "data_manager.h"
#include "device.h"
#include "time2.h"
#include "rcs_client.h"
#include "utils.h"

using namespace std;

RetValue	ShellCommandRCS
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* _shell
)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	if (_count == 1)
	{
		THROW_INVALID_ARGUMENT("Too few arguments!");
	}

	if (_arguments[1] == MSG_TYPE_RCS_ADD)
	{
		bool		ret_value = false;
		JSONNode	properties;

		if ((_count < 5) || (_count % 2 == 0))
		{
			THROW_INVALID_ARGUMENT("Invalid argument");	
		}

		for(uint32_t i = 3 ; i+1 < _count ; i+=2)
		{
			if (_arguments[i].substr(0, 2) != "--")
			{
				THROW_INVALID_ARGUMENT("Invalid argument : " << _arguments[i]);	
			}

			properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].length() - 2), _arguments[i+1]));
		}

		if (_arguments[2] == "gateway")
		{
			ret_value = client->AddGateway(properties);
		}
		else if (_arguments[2] == "device")
		{
			ret_value = client->AddDevice(properties);
		}
		else if (_arguments[2] == "endpoint")
		{
			ret_value = client->AddEndpoint(properties);
		}

		if (ret_value)
		{
			_shell->Out() << "The " << _arguments[2] << "has been added." << std::endl;		
		}
		else
		{
			_shell->Out() << "Failed to add " << _arguments[2] << "." << std::endl;		
		}
	}
	else if (_arguments[1] == MSG_TYPE_RCS_DEL)
	{
		if (_count < 3)
		{
			THROW_INVALID_ARGUMENT("Invalid argument");	
		}

		for(uint32_t i = 3 ; i < _count ; i++)
		{
			if (client->DelEndpoint(_arguments[i]))
			{
				_shell->Out() << "The endpoint[" << _arguments[i] << "] has been deleted." << std::endl;		
			}
			else
			{
				_shell->Out() << "Faild to delete endpoint[" << _arguments[i] << "]" << std::endl;		
			}
		}
	}
	else if (_arguments[1] == MSG_TYPE_RCS_GET)
	{
		if (_count < 4)
		{
			THROW_INVALID_ARGUMENT("Too few arguments!");
		}

		for(uint32_t i = 3 ; i < _count ; i++)
		{
			bool	ret_value = false;

			if (_arguments[2] == "gateway")
			{
				JSONNode	properties;

				if (client->GetGateway(_arguments[i], properties))
				{
					_shell->Out() << properties.write_formatted() << std::endl;
				}
				else
				{
					_shell->Out() << "Failed to get gateway[" << _arguments[i] <<" properties." << std::endl;
					break;
				}
			}
			else if (_arguments[2] == "device")
			{
				JSONNode	properties;

				if (client->GetDevice(_arguments[i], properties))
				{
					_shell->Out() << properties.write_formatted() << std::endl;
				}
				else
				{
					_shell->Out() << "Failed to get device[" << _arguments[i] <<" properties." << std::endl;
					break;
				}
			}
			else if (_arguments[2] == "endpoint")
			{
				JSONNode	properties;

				if (client->GetEndpoint(_arguments[i], properties))
				{
					_shell->Out() << properties.write_formatted() << std::endl;
				}
				else
				{
					_shell->Out() << "Failed to get endpoint[" << _arguments[i] <<" properties." << std::endl;
					break;
				}
			}

		}
	}
	else if (_arguments[1] == MSG_TYPE_RCS_SET)
	{
		JSONNode	properties;

		if (_count < 6)
		{
			THROW_INVALID_ARGUMENT("Too few arguments!");	
		}
		
		if (_count % 2 != 0)
		{
			THROW_INVALID_ARGUMENT("Invalid argument!");	
		}

		properties.push_back(JSONNode(TITLE_NAME_ID, _arguments[2]));

		for(uint32_t i = 4 ; i+1 < _count ; i+=2)
		{
			if (_arguments[i].substr(0, 2) != "--")
			{
				THROW_INVALID_ARGUMENT("Invalid argument[" << _arguments[i] << "].");	
			}

			properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].length() - 2), _arguments[i+1]));
		}

		bool	ret_value = false;

		if (_arguments[2] == "gateway")
		{
			ret_value = client->SetGateway(properties);
		}
		else if (_arguments[2] == "devicie")
		{
			ret_value = client->SetDevice(properties);
		}
		if (_arguments[2] == "endpoint")
		{
			ret_value = client->SetEndpoint(properties);
		}

		if (ret_value == true)
		{
			_shell->Out() << "The " << _arguments[2] << "[" << _arguments[3] << "] properties changed." << std::endl;	

		}
		else
		{
			_shell->Out() << "Failed to set " << _arguments[2] << "[" << _arguments[3] << "] properties." << std::endl;	
		}
	}
	else if (_arguments[1] == MSG_TYPE_RCS_LIST)
	{
		std::vector<JSONNode>	_vector;

		if (_count < 3)
		{
			THROW_INVALID_ARGUMENT("Too few arguments!");	
		}

		if (_arguments[2] == "gateway")
		{
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
		else if (_arguments[2] == "device")
		{
			if (client->GetDevice(_vector))
			{
				for(uint32_t i = 0 ; i < _vector.size() ; i++)
				{
					_shell->Out() << _vector[i].write_formatted() << std::endl;
				}
			}
			else
			{
				_shell->Out() << "Failed to get endpoint list." << std::endl;
			}
		}
		if (_arguments[2] == "endpoint")
		{
			if (client->GetEndpoint(_vector))
			{
				for(uint32_t i = 0 ; i < _vector.size() ; i++)
				{
					_shell->Out() << _vector[i].write_formatted() << std::endl;
				}
			}
			else
			{
				_shell->Out() << "Failed to get endpoint list." << std::endl;
			}
		}
	
	}
	return	RET_VALUE_OK;
}

Shell::Command	object_manager_command_rcs
(
	"rcs",
	"Remote Control Service",
	"Remote Control Service",
	ShellCommandRCS
);
