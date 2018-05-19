#include <iostream>
#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "exception.h"
#include "json.h"
#include "device.h"
#include "device_snmp.h"
#include "object_manager.h"


RetValue	ShellCommandLoad
(
	const std::vector<std::string>&	_arguments,
	Shell*			_shell
)
{
	RetValue	ret_value = RET_VALUE_OK;
	ObjectManager	*manager = dynamic_cast<ObjectManager*>(_shell->GetObject());

	if (_arguments.size() >= 3)
	{
		if (_arguments[1] == "config")
		{
			for(uint32_t i = 2 ; i < _arguments.size() ; i++)
			{
				try
				{
					JSONNode	config = JSONNodeLoadFromFile(_arguments[i]);
					
					manager->SetProperties(config, false, false);
				}
				catch(InvalidArgument& e)
				{
					_shell->Out() << e.what() << std::endl;
				}

			}

		}
		else if (_arguments[1] == "gateway")
		{
			for(uint32_t i = 2 ; i < _arguments.size() ; i++)
			{
				try
				{
					JSONNode	config = JSONNodeLoadFromFile(_arguments[i]);

					Gateway *gateway = manager->CreateGateway(config);
					if (gateway != NULL)
					{
						_shell->Out() << "Gateway[" << gateway->GetTraceName() << "] created"  << std::endl;	
					}
					else
					{
						_shell->Out() << "Failed to create gateway!" << std::endl;	
					}
				}
				catch(InvalidArgument& e)
				{
					_shell->Out() << e.what() << std::endl;
				}
			}
		}	
		else if (_arguments[1] == "device")
		{
			for(uint32_t i = 2 ; i < _arguments.size() ; i++)
			{
				try
				{
					JSONNode	config = JSONNodeLoadFromFile(_arguments[i]);

					_shell->Out() << "Device Configuration" << std::endl;
					_shell->Out() << config.write_formatted() << std::endl;

					Device *device = manager->CreateDevice(config);
					if (device != NULL)
					{
						_shell->Out() << "Device[" << device->GetTraceName() << "] created"  << std::endl;	
					}
					else
					{
						_shell->Out() << "Failed to create device!" << std::endl;	
					}
				}
				catch(InvalidArgument& e)
				{
					_shell->Out() << e.what() << std::endl;
				}
			}
		}	
	}
	else
	{
		ret_value = RET_VALUE_INVALID_ARGUMENTS;
	}

	switch(ret_value)
	{
	case	RET_VALUE_INVALID_ARGUMENTS: _shell->Out() << "Invalid arguments!" << std::endl;	break;
	}

	return	ret_value;
}


Shell::Command	shell_ftgm_command_load
(
	"load",
	"<COMMAND> <FILE> [<FILE> ...] \n"
	"  Load objects(configurations, devices, endpoints ...).\n"
	"COMMANDS:\n"
	"  config\n"
	"    - Load configurations.\n"
	"  device\n"
	"    - Load devices.\n",
	"Load objects",
	ShellCommandLoad
);
