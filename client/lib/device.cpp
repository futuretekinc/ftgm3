#include <iostream>
#include <iomanip>
#include <libjson/libjson.h>
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "device.h"
#include "time2.h"
#include "tcp_client.h"

using namespace std;

RetValue	ShellCommandDevice
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* _shell
)
{
	TCPClient*	client = dynamic_cast<TCPClient*>(_shell->GetObject());

	if (_count == 1)
	{
		
	}
	else 
	{
		JSONNode	request_node;
		std::string	response;

		request_node.push_back(JSONNode(RMC_FIELD_SECTION, "device"));

		if (_arguments[1] == "list")
		{
			request_node.push_back(JSONNode(RMC_FIELD_COMMAND, _arguments[1]));
		}
		else if ((_arguments[1] == "get") || (_arguments[1] == "start") || (_arguments[1] == "stop") || (_arguments[1] == "del") || (_arguments[1] == "enable") || (_arguments[1] == "disable"))
		{
			if (_count < 3)
			{
				return		RET_VALUE_INVALID_ARGUMENTS;
			}

			request_node.push_back(JSONNode(RMC_FIELD_COMMAND, _arguments[1]));
			if (_count == 3)
			{
				request_node.push_back(JSONNode(RMC_FIELD_ID, _arguments[2]));
			}
			else
			{
				JSONNode	id_array(JSON_ARRAY);
				id_array.set_name(RMC_FIELD_ID_ARRAY);
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					id_array.push_back(JSONNode("", _arguments[i]));
				}

				request_node.push_back(id_array);
			}
		}
		else if ((_arguments[1] == "add") && (_count >= 3))
		{
			JSONNode	device_node;
			request_node.push_back(JSONNode(RMC_FIELD_COMMAND, _arguments[1]));

			device_node.push_back(JSONNode(RMC_FIELD_TYPE, _arguments[2]));
			for(uint32_t i = 3 ; i+1 < _count ; i+=2)
			{
				if (_arguments[i].substr(0,2) != "--")
				{
					return		RET_VALUE_INVALID_ARGUMENTS;
				}

				std::string	options = _arguments[i].substr(2, _arguments[i].size());
				device_node.push_back(JSONNode(options, _arguments[i+1]));
			}

			device_node.set_name(RMC_FIELD_DEVICE);
			request_node.push_back(device_node);
		}
		else if (_count == 2)
		{
			request_node.push_back(JSONNode(RMC_FIELD_COMMAND, _arguments[1]));
		}
		else
		{
			return		RET_VALUE_INVALID_ARGUMENTS;
		}

		if (client->RequestAndReply(request_node.write(), response))
		{
			try
			{
				JSONNode	json = libjson::parse(response);

				_shell->Out() << json.write_formatted() << std::endl;
			}
			catch(std::invalid_argument)
			{
				_shell->Out() << response << std::endl;
			}
		}
		else
		{
			_shell->Out() << "Failed to send message!" << std::endl;
		}
	}

	return	RET_VALUE_OK;
}

Shell::Command	object_manager_command_device =
{
	.command	= "device",
	.help		= "device",
	.short_help	= "device",
	.function	= ShellCommandDevice
};
