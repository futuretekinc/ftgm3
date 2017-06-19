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
	std::string*	_arguments,
	uint32_t		_count,
	Shell*			_shell
)
{
	RetValue	ret_value = RET_VALUE_OK;
	ObjectManager	*manager = dynamic_cast<ObjectManager*>(_shell->GetObject());

	if (_count >= 3)
	{
		if (_arguments[1] == "config")
		{
			for(uint32_t i = 2 ; i < _count ; i++)
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
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				try
				{
					JSONNode	config = JSONNodeLoadFromFile(_arguments[i]);

					Gateway *gateway = manager->CreateGateway(config);
					if (gateway != NULL)
					{
						std::cout << "Gateway[" << gateway->GetTraceName() << "] created"  << std::endl;	
					}
					else
					{
						std::cout << "Failed to create device!" << std::endl;	
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
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				try
				{
					JSONNode	config = JSONNodeLoadFromFile(_arguments[i]);

					Gateway *gateway = manager->CreateGateway(config);
					if (gateway != NULL)
					{
						std::cout << "Gateway[" << gateway->GetTraceName() << "] created"  << std::endl;	
					}
					else
					{
						std::cout << "Failed to create device!" << std::endl;	
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
	case	RET_VALUE_INVALID_ARGUMENTS: std::cout << "Invalid arguments!" << std::endl;	break;
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
