#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "device.h"
#include "device_snmp.h"
#include "object_manager.h"


RetValue	ShellCommandDevice
(
	std::string*	_arguments,
	uint32_t		_count,
	Shell*			_shell
)
{
	RetValue	ret_value = RET_VALUE_OK;
	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();

	if (object_manager == NULL)
	{
		std::cout << "Object manager not attached!" << std::endl;	
	}
	else if (_count < 2)
	{
		uint32_t	id_len 	= 32;
		uint32_t	name_len= 16;
		uint32_t	type_len= 16;
		uint32_t	stat_len= 16;
		uint32_t	ip_len= 16;
		uint32_t	module_len= 16;
		uint32_t	community_len= 16;

		std::cout << "Device count : " << object_manager->GetDeviceCount() << std::endl;
		
		std::list<Device*>	snmp_list;
		if (object_manager->GetDeviceList(DeviceSNMP::Type(), snmp_list) != 0)
		{
			std::cout << "* SNMP Device" << std::endl;
			std::cout << std::setw(id_len) << "ID";
			std::cout << " " << std::setw(name_len) << "Name";
			std::cout << " " << std::setw(type_len) << "Type";
			std::cout << " " << std::setw(stat_len) << "Stat";
			std::cout << " " << std::setw(ip_len) << "IP";
			std::cout << " " << std::setw(module_len) << "Module";
			std::cout << " " << std::setw(community_len) << "Community";
			std::cout << std::endl;

			for(auto it = snmp_list.begin() ; it != snmp_list.end() ; it++)
			{
				DeviceSNMP *device = dynamic_cast<DeviceSNMP*>(*it);

				std::cout << std::setw(id_len) << device->GetID();
				std::cout << " " << std::setw(name_len) << device->GetName();
				std::cout << " " << std::setw(type_len) << device->GetType();
				std::cout << " " << std::setw(stat_len) << Object::ToString(device->GetStat());
				std::cout << " " << std::setw(ip_len) << device->GetIP();
				std::cout << " " << std::setw(module_len) << device->GetModule();
				std::cout << " " << std::setw(community_len) << device->GetCommunity();
				std::cout << std::endl;	
			}
		}

		std::list<Device*>	mbtcp_list;
		if (object_manager->GetDeviceList(ValueType("d_mbtcp"), mbtcp_list) != 0)
		{
			std::cout << "* MBTCP Device" << std::endl;
			std::cout << std::setw(id_len) << "ID";
			std::cout << " " << std::setw(name_len) << "Name";
			std::cout << " " << std::setw(type_len) << "Type";
			std::cout << std::endl;

			for(auto it = mbtcp_list.begin() ; it != mbtcp_list.end() ; it++)
			{
				std::cout << std::setw(id_len) << (*it)->GetID();
				std::cout << " " << std::setw(name_len) << (*it)->GetName();
				std::cout << " " << std::setw(type_len) << (*it)->GetType();
				std::cout << std::endl;	
			}
		}

	}
	else if (_arguments[1] == "create")
	{
		Properties	properties;
		
		if (_count < 3)
		{
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
		}
		else
		{
			if (!properties.AppendDeviceType(_arguments[2]))
			{
				ret_value = RET_VALUE_INVALID_ARGUMENTS;
			}
			else
			{
				for(uint32_t i = 3; (ret_value == RET_VALUE_OK) && (i + 1 < _count)  ; i+=2)
				{
					if (_arguments[i] == "--id")
					{
						if (!properties.AppendID(_arguments[i+1]))
						{
							ret_value = RET_VALUE_INVALID_ARGUMENTS;
						}
					}
					else if (_arguments[i] == "--name")
					{
						if (!properties.AppendName(_arguments[i+1]))
						{
							ret_value = RET_VALUE_INVALID_ARGUMENTS;
						}
					}
				}

				if (ret_value == RET_VALUE_OK)
				{
					Device* device = object_manager->CreateDevice(properties);	
					if (device == NULL)
					{
						ret_value = RET_VALUE_INVALID_ARGUMENTS;
					}
					else
					{
						std::cout << "Device created." << std::endl;		
					}
				}
			}
		}

	}
	else if (_arguments[1] == "destroy")
	{
		if (_count < 3)
		{
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
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
					delete device;
					std::cout << "The device[" << _arguments[i] << "] has been deleted!" << std::endl;
				}
			}
		}
	}
	else if (_arguments[1] == "start")
	{
		if (_count < 3)
		{
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
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
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
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
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
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
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
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
		Properties	properties;
		
		if (_count < 5)
		{
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
		}
		else
		{
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
						std::cout << "Invalid argument options(" << _arguments[i] << ")" << std::endl;
						ret_value = RET_VALUE_INVALID_ARGUMENTS;
					}
					else if (!properties.Append(Property(_arguments[i].substr(2, _arguments[i].size() - 2), _arguments[i+1])))
					{
						std::cout << "properties.Append(" << _arguments[i].substr(2,_arguments[i].size() - 2)<< ", " << _arguments[i+1] << ")" << std::endl;
						ret_value = RET_VALUE_INVALID_ARGUMENTS;
					}
				}

				if (ret_value == RET_VALUE_OK)
				{
					device->SetProperties(properties);	
				}
			}
		}
	}
	else if (_arguments[1] == "load")
	{
		if (_count < 3)
		{
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
		}
		else
		{
			JSONNode	json;
			std::fstream	fs(_arguments[2], std::fstream::in);
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
				
					Properties	properties;
					properties.Append(json);
				
					Device* device = object_manager->CreateDevice(properties);
					if (device == NULL)
					{
						std::cout << "Failed to create device!" << std::endl;
					}
					else
					{
						std::cout << "The device is created!" << std::endl;
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


Shell::Command	shell_ftgm_command_device = 
{
	.name		=	"device",
	.help		=	"<command> \n"
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
	.short_help	=	"Management of device",
	.function	=	ShellCommandDevice
};
