#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "device.h"
#include "device_snmp.h"


RetValue	ShellCommandDevice
(
	std::string*	_arguments,
	uint32_t		_count,
	Shell*			_shell
)
{
	RetValue	ret_value = RET_VALUE_OK;

	if (_count < 2)
	{
		std::cout << "Device count : " << Device::Count() << std::endl;
		
		std::list<Device*>	snmp_list;
		if (Device::GetList(Device::SNMP, snmp_list) != 0)
		{
			std::cout << "* SNMP Device" << std::endl;
			std::cout << std::setw(16) << "ID";
			std::cout << std::setw(16) << "Name";
			std::cout << std::setw(16) << "Type";
			std::cout << std::setw(8) << "Enable";
			std::cout << std::setw(8) << "Stat";
			std::cout << std::setw(16) << "IP";
			std::cout << std::setw(16) << "Module";
			std::cout << std::setw(16) << "Community";
			std::cout << std::endl;

			for(auto it = snmp_list.begin() ; it != snmp_list.end() ; it++)
			{
				DeviceSNMP *device = dynamic_cast<DeviceSNMP*>(*it);

				std::cout << std::setw(16) << device->GetID();
				std::cout << std::setw(16) << device->GetName();
				std::cout << std::setw(16) << ToString(Device::SNMP);
				std::cout << std::setw(8) << device->GetEnable();
				std::cout << std::setw(8) << (device->IsRunning()?"run":"stop");
				std::cout << std::setw(16) << device->GetIP();
				std::cout << std::setw(16) << device->GetModule();
				std::cout << std::setw(16) << device->GetCommunity();
				std::cout << std::endl;	
			}
		}

		std::list<Device*>	mbtcp_list;
		if (Device::GetList(Device::MBTCP, mbtcp_list) != 0)
		{
			std::cout << "* MBTCP Device" << std::endl;
			std::cout << std::setw(16) << "ID";
			std::cout << std::setw(16) << "Name";
			std::cout << std::setw(16) << "Type";
			std::cout << std::endl;

			for(auto it = mbtcp_list.begin() ; it != mbtcp_list.end() ; it++)
			{
				std::cout << std::setw(16) << (*it)->GetID();
				std::cout << std::setw(16) << (*it)->GetName();
				std::cout << std::setw(16) << ToString(Device::MBTCP);
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
					Device* device = Device::Create(properties);	
					if (device == NULL)
					{
						ret_value = RET_VALUE_INVALID_ARGUMENTS;
					}
					else
					{
						std::cout << "Device created." << std::endl;		
						std::cout << *device << std::endl;
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
				Device *device = Device::Get(_arguments[i]);
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
				Device *device = Device::Get(_arguments[i]);
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
				Device *device = Device::Get(_arguments[i]);
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
	else if (_arguments[1] == "set")
	{
		Properties	properties;
		
		if (_count < 5)
		{
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
		}
		else
		{
			Device*	device = Device::Get(_arguments[2]);
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
					else if (!properties.Append(_arguments[i].substr(2, _arguments[i].size() - 2), _arguments[i+1]))
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

					uint32_t count = Device::Create(json);
					std::cout << "Device created : " << count << std::endl;
				}
				else
				{
					std::cout << "Invalid json format" << std::endl;	
				}
				delete buffer;
			}
			
		}


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
	.help		=	"<command>\n",
	.short_help	=	"Management of device",
	.function	=	ShellCommandDevice
};
