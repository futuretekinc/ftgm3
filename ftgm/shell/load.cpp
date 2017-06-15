#include <iostream>
#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
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
				JSONNode	json;
				std::fstream	fs(_arguments[i], std::fstream::in);
				if (fs)
				{
					fs.seekg (0, fs.end);
					int length = fs.tellg();
					fs.seekg (0, fs.beg);

					char * buffer = new char [length + 1];
					fs.read(buffer, length);
					buffer[length] = 0;
					fs.close();

					if (libjson::is_valid(buffer))
					{
						if (!manager->Load(buffer))
						{
							std::cout << "The configuration file[" << _arguments[i] << "] loading failed." << std::endl;
						}
						else
						{
							std::cout << "The configuration file[" << _arguments[i] << "] loaded." << std::endl;
						}
					}
					else
					{
						std::cout << "Invalid json format" << std::endl;	
					}

					delete buffer;
				}
				else
				{
					std::cout << "Invalid file[" << _arguments[i] << "]."<< std::endl;	
				}
			}

		}
		else if (_arguments[1] == "gateway")
		{
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				JSONNode	json;
				std::fstream	fs(_arguments[i], std::fstream::in);
				if (fs)
				{
					fs.seekg (0, fs.end);
					int length = fs.tellg();
					fs.seekg (0, fs.beg);

					char * buffer = new char [length + 1];
					fs.read(buffer, length);
					buffer[length] = 0;
					fs.close();

					if (libjson::is_valid(buffer))
					{
						json = libjson::parse(buffer);

						Gateway *gateway = manager->CreateGateway(json);
						if (gateway != NULL)
						{
							std::cout << "Gateway[" << gateway->GetTraceName() << "] created"  << std::endl;	
						}
						else
						{
							std::cout << "Failed to create device!" << std::endl;	
						}
					}
					else
					{
						std::cout << "Invalid json format" << std::endl;	
					}
					delete buffer;
				}
			}
		}	
		else if (_arguments[1] == "device")
		{
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				JSONNode	json;
				std::fstream	fs(_arguments[i], std::fstream::in);
				if (fs)
				{
					fs.seekg (0, fs.end);
					int length = fs.tellg();
					fs.seekg (0, fs.beg);

					char * buffer = new char [length + 1];
					fs.read(buffer, length);
					buffer[length] = 0;
					fs.close();

					if (libjson::is_valid(buffer))
					{
						json = libjson::parse(buffer);

						Device *device = manager->CreateDevice(json);
						if (device != NULL)
						{
							std::cout << "Device[" << device->GetTraceName() << "] created"  << std::endl;	
						}
						else
						{
							std::cout << "Failed to create device!" << std::endl;	
						}
					}
					else
					{
						std::cout << "Invalid json format" << std::endl;	
					}
					delete buffer;
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


Shell::Command	shell_ftgm_command_load = 
{
	.name		=	"load",
	.help		=	"<COMMAND> <FILE> [<FILE> ...] \n"
					"  Load objects(configurations, devices, endpoints ...).\n"
					"COMMANDS:\n"
					"  config\n"
					"    - Load configurations.\n"
					"  device\n"
					"    - Load devices.\n",
	.short_help	=	"Load objects",
	.function	=	ShellCommandLoad
};
