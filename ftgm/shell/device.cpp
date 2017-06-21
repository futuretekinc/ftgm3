#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "exception.h"
#include "device.h"
#include "device_snmp.h"
#include "object_manager.h"


bool	ShellCommandDeviceList(Shell* _shell);

RetValue	ShellCommandDevice
(
	std::string*	_arguments,
	uint32_t		_count,
	Shell*			_shell
)
{
	RetValue	ret_value = RET_VALUE_OK;
	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();

	try
	{
		if (object_manager == NULL)
		{
			std::cout << "Object manager not attached!" << std::endl;	
		}
		else if (_count < 2)
		{
			ShellCommandDeviceList(_shell);
		}
		else if (_arguments[1] == "keep_alive")
		{
			if (_count < 3)
			{
				throw InvalidArgument("The paramater is insufficient.");
			}


			for(uint32_t i = 2 ; i < _count ; i++)
			{
				Device*	device = object_manager->GetDevice(_arguments[i]);
				if (device == NULL)
				{
					std::cout << "Failed to get device[" << _arguments[i] << "]." << std::endl;
				}
				else
				{

				}
			}
		}
		else if (_arguments[1] == "create")
		{
			JSONNode	properties;

			if (_count < 3)
			{
				throw InvalidArgument("The paramater is insufficient.");
			}

			properties.push_back(JSONNode(TITLE_NAME_TYPE, _arguments[2]));

			for(uint32_t i = 3; (ret_value == RET_VALUE_OK) && (i + 1 < _count)  ; i+=2)
			{
				uint32_t	len = _arguments[i].size();
				
				if ((len <= 2) || (_arguments[i].substr(0,2) == "--"))
				{
					std::ostringstream	oss;

					oss << "Invalid argument option[" << _arguments[i] << "]" << std::endl;
			
					throw InvalidArgument(oss.str());
				}

				properties.push_back(JSONNode(_arguments[i].substr(2, len - 2), _arguments[i+1]));
			}

			Device* device = object_manager->CreateDevice(properties);	
			if (device == NULL)
			{
				throw InvalidArgument(properties.write_formatted());
			}

			std::cout << "Device created." << std::endl;		

		}
		else if (_arguments[1] == "destroy")
		{
			if (_count < 3)
			{
				throw InvalidArgument("The paramater is insufficient.");
			}

			for(uint32_t i = 2 ; i < _count ; i++)
			{
				Device *device = object_manager->GetDevice(_arguments[i]);
				if (device == NULL)
				{
					std::cout << "Device[" << _arguments[i] << "] not found!" << std::endl;
				}
				else
				{
					delete device;
					std::cout << "The device[" << _arguments[i] << "] has been deleted!" << std::endl;
				}
			}
		}
		else if (_arguments[1] == "start")
		{
			if (_count < 3)
			{
				throw InvalidArgument("The paramater is insufficient.");
			}

			if (_arguments[2] == "all")
			{
				std::list<Device*>	device_list;

				object_manager->GetDeviceList(device_list);

				for(std::list<Device*>::iterator it = device_list.begin(); it != device_list.end() ; it++)
				{
					(*it)->Start();
					std::cout << "The device[" << (*it)->GetTraceName() << "] is started!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					Device *device = object_manager->GetDevice(_arguments[i]);
					if (device == NULL)
					{
						std::cout << "Device[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						device->Start();
						std::cout << "The device[" << _arguments[i] << "] has started!" << std::endl;
					}
				}
			}
		}
		else if (_arguments[1] == "stop")
		{
			if (_count < 3)
			{
				throw InvalidArgument("The paramater is insufficient.");
			}

			if (_arguments[2] == "all")
			{
				std::list<Device*>	device_list;

				object_manager->GetDeviceList(device_list);

				for(std::list<Device*>::iterator it = device_list.begin(); it != device_list.end() ; it++)
				{
					(*it)->Stop();
					std::cout << "The device[" << (*it)->GetTraceName() << "] is stopped!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					Device *device = object_manager->GetDevice(_arguments[i]);
					if (device == NULL)
					{
						std::cout << "Device[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						device->Stop();
						std::cout << "The device[" << _arguments[i] << "] has stopped!" << std::endl;
					}
				}
			}
		}
		else if (_arguments[1] == "enable")
		{
			if (_count < 3)
			{
				throw InvalidArgument("The paramater is insufficient.");
			}

			if (_arguments[2] == "all")
			{
				std::list<Device*>	device_list;

				object_manager->GetDeviceList(device_list);

				for(std::list<Device*>::iterator it = device_list.begin(); it != device_list.end() ; it++)
				{
					(*it)->SetEnable(true);
					std::cout << "The device[" << (*it)->GetTraceName() << "] is enabled!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					Device *device = object_manager->GetDevice(_arguments[i]);
					if (device == NULL)
					{
						std::cout << "Device[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						device->SetEnable(true);
						std::cout << "The device[" << _arguments[i] << "] is enabled!" << std::endl;
					}
				}
			}
		}
		else if (_arguments[1] == "disable")
		{
			if (_count < 3)
			{
				throw InvalidArgument("The paramater is insufficient.");
			}

			if (_arguments[2] == "all")
			{
				std::list<Device*>	device_list;

				object_manager->GetDeviceList(device_list);

				for(std::list<Device*>::iterator it = device_list.begin(); it != device_list.end() ; it++)
				{
					(*it)->SetEnable(false);
					std::cout << "The device[" << (*it)->GetTraceName() << "] is disabled!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					Device *device = object_manager->GetDevice(_arguments[i]);
					if (device == NULL)
					{
						std::cout << "Device[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						device->SetEnable(false);
						std::cout << "The device[" << _arguments[i] << "] is disabled!" << std::endl;
					}
				}
			}
		}
		else if (_arguments[1] == "set")
		{
			JSONNode	properties;

			if (_count < 5)
			{
				throw InvalidArgument("The paramater is insufficient.");
			}

			Device*	device = object_manager->GetDevice(_arguments[2]);
			if (device == NULL)
			{
				std::cout << "Device[" << _arguments[2] << " not found!" << std::endl;
				ret_value = RET_VALUE_INVALID_ARGUMENTS;
			}
			else
			{
				for(uint32_t i = 3; (ret_value == RET_VALUE_OK) && (i + 1 < _count)  ; i+=2)
				{
					if ((_arguments[i].size() <= 2) || (_arguments[i].substr(0,2) != "--"))
					{
						std::ostringstream	oss;

						oss << "Invalid argument options(" << _arguments[i] << ")" << std::endl;
						throw InvalidArgument(oss.str());
					}

					properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].size() - 2), _arguments[i+1]));
				}

				device->SetProperties(properties, false, false);	
			}
		}
	}
	catch(InvalidArgument& e)
	{
		_shell->Out() << e.what() << std::endl;		
		ret_value = RET_VALUE_INVALID_ARGUMENTS;
	}


	return	ret_value;
}


Shell::Command	shell_ftgm_command_device
(
	"device",
	"<command> \n"
	"  Management of device.\n"
	"COMMANDS:\n"
	"  create  <TYPE> [--id <ID>] [--name <NAME>]\n"
	"    Create device\n"
	"  destroy <ID> [<ID> ...]\n"
	"    Destroy devices.\n"
	"  start   <ID> [<ID> ...]\n"
	"    Start devices.\n"
	"  stop    <ID> [<ID> ...]\n"
	"    Stop devices.\n"
	"PARAMETERS:\n"
	"  TYPE    Type of device\n"
	"  ID      Device ID\n",
	"Management of device",
	ShellCommandDevice
);

bool	ShellCommandDeviceList(Shell* _shell)
{
	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();

	uint32_t	id_len 	= 32;
	uint32_t	name_len= 16;
	uint32_t	type_len= 16;
	uint32_t	stat_len= 16;
	uint32_t	options_len= 16;

	std::cout << "Device count : " << object_manager->GetDeviceCount() << std::endl;

	std::list<Device*>	device_list;
	if (object_manager->GetDeviceList(device_list) != 0)
	{
		std::cout << "* Device" << std::endl;
		std::cout << std::setw(id_len) << "ID";
		std::cout << " " << std::setw(name_len) << "Name";
		std::cout << " " << std::setw(type_len) << "Type";
		std::cout << " " << std::setw(stat_len) << "Stat";
		std::cout << " " << std::setw(options_len) << "Options";
		std::cout << std::endl;

		for(std::list<Device*>::iterator it = device_list.begin() ; it != device_list.end() ; it++)
		{

			JSONNode	options;

			std::cout << std::setw(id_len) << (*it)->GetID();
			std::cout << " " << std::setw(name_len) << (*it)->GetName();
			std::cout << " " << std::setw(type_len) << (*it)->GetType();
			std::cout << " " << std::setw(stat_len) << ToString((*it)->GetStat());
			(*it)->GetOptions(options);

			std::cout << " " << std::setw(stat_len) << options.write();

			std::cout << std::endl;	
		}
	}

	return	true;
}

