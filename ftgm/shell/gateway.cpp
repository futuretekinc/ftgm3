#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "gateway.h"
#include "gateway_gen.h"
#include "object_manager.h"

bool	ShellCommandGatewayList(Shell* _shell)
{
	std::list<Gateway*>	gen_list;
	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();

	if (object_manager->GetGatewayList(GatewayGen::Type(), gen_list) != 0)
	{
		uint32_t	id_len = 8;
		uint32_t	name_len = 8;
		uint32_t	type_len = 8;
		uint32_t	stat_len = 8;
		uint32_t	registered_len = 10;

		for(std::list<Gateway*>::iterator it = gen_list.begin() ; it != gen_list.end() ; it++)
		{
			GatewayGen	*gateway = dynamic_cast<GatewayGen*>(*it);

			if (id_len < std::string(gateway->GetID()).length())
			{
				id_len = std::string(gateway->GetID()).length();
			}

			if (name_len < std::string(gateway->GetName()).length())
			{
				name_len = std::string(gateway->GetName()).length();
			}

			if (type_len < std::string(gateway->GetType()).length())
			{
				type_len = std::string(gateway->GetType()).length();
			}

			if (stat_len < ToString(gateway->GetStat()).length())
			{
				stat_len = ToString(gateway->GetStat()).length();
			}
		}

		std::cout << "* General Gateway" << std::endl;
		std::cout << std::setw(id_len) << "ID";
		std::cout << " " << std::setw(name_len) << "Name";
		std::cout << " " << std::setw(type_len) << "Type";
		std::cout << " " << std::setw(stat_len) << "Stat";
		std::cout << " " << std::setw(registered_len) << "Registered";
		std::cout << std::endl;

		for(std::list<Gateway*>::iterator it = gen_list.begin() ; it != gen_list.end() ; it++)
		{
			GatewayGen	*gateway = dynamic_cast<GatewayGen*>(*it);

			std::cout << std::setw(id_len) << gateway->GetID();
			std::cout << " " << std::setw(name_len) << gateway->GetName();
			std::cout << " " << std::setw(type_len) << gateway->GetType();
			std::cout << " " << std::setw(stat_len) << ToString(gateway->GetStat());
			std::cout << " " << std::setw(registered_len) << (gateway->GetRegistered()?"yes":"no");
			std::cout << std::endl;	
		}
	}

	return	true;
}

RetValue	ShellCommandGateway
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
			ShellCommandGatewayList(_shell);
		}
		else if (_arguments[1] == "destroy")
		{
		}
		else if (_arguments[1] == "start")
		{
			if (_count < 3)
			{
				ret_value = RET_VALUE_INVALID_ARGUMENTS;
			}
			else if (_arguments[2] == "all")
			{
				std::list<Gateway*>	gateway_list;

				object_manager->GetGatewayList(gateway_list);

				for(std::list<Gateway*>::iterator it = gateway_list.begin(); it != gateway_list.end() ; it++)
				{
					(*it)->Start();
					std::cout << "The gateway[" << (*it)->GetTraceName() << "] is started!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					Gateway *gateway = object_manager->GetGateway(_arguments[i]);
					if (gateway == NULL)
					{
						std::cout << "Gateway[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						gateway->Start();
						std::cout << "The gateway[" << _arguments[i] << "] has started!" << std::endl;
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
			else if (_arguments[2] == "all")
			{
				std::list<Gateway*>	gateway_list;

				object_manager->GetGatewayList(gateway_list);

				for(std::list<Gateway*>::iterator it = gateway_list.begin(); it != gateway_list.end() ; it++)
				{
					(*it)->Stop();
					std::cout << "The gateway[" << (*it)->GetTraceName() << "] is stopped!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					Gateway *gateway = object_manager->GetGateway(_arguments[i]);
					if (gateway == NULL)
					{
						std::cout << "Gateway[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						gateway->Stop();
						std::cout << "The gateway[" << _arguments[i] << "] has stopped!" << std::endl;
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
			else if (_arguments[2] == "all")
			{
				std::list<Gateway*>	gateway_list;

				object_manager->GetGatewayList(gateway_list);

				for(std::list<Gateway*>::iterator it = gateway_list.begin(); it != gateway_list.end() ; it++)
				{
					(*it)->SetEnable(false);
					std::cout << "The gateway[" << (*it)->GetTraceName() << "] is disabled!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					Gateway *gateway = object_manager->GetGateway(_arguments[i]);
					if (gateway == NULL)
					{
						std::cout << "Gateway[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						gateway->SetEnable(false);
						std::cout << "The gateway[" << _arguments[i] << "] is disabled!" << std::endl;
					}
				}
			}
		}
		else if (_arguments[1] == "set")
		{
			if (_count < 4)
			{
				throw std::invalid_argument("Invalid arguments!");
			}
			else
			{
				JSONNode	properties;
				Gateway *gateway = object_manager->GetGateway(_arguments[2]);

				for(uint32_t i = 3 ; i + 1 < _count ; i += 2)
				{
					if((_arguments[i].size() < 3) || (_arguments[i].substr(0,2) != "--"))
					{
						throw std::invalid_argument("Invalid arguments!");
					}

					JSONNode	property(_arguments[i].substr(2,_arguments[i].size() - 2), _arguments[i+1]);

					properties.push_back(property);
				}

				if (properties.size() == 0)
				{
					throw std::invalid_argument("Invalid arguments!");
				}

				gateway->SetProperties(properties, false, false);
			}

		}
		else
		{
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Invalid arguments!" << std::endl;	
		ret_value = RET_VALUE_INVALID_ARGUMENTS;
	}

	return	ret_value;
}


Shell::Command	shell_ftgm_command_gateway
(
	"gateway",
	"<command> \n"
	"  Management of gateway.\n"
	"COMMANDS:\n"
	"  create  <TYPE> [--id <ID>] [--name <NAME>]\n"
	"    Create gateway\n"
	"  destroy <ID> [<ID> ...]\n"
	"    Destroy gateways.\n"
	"  start   <ID> [<ID> ...]\n"
	"    Start gateways.\n"
	"  stop    <ID> [<ID> ...]\n"
	"    Stop gateways.\n"
	"PARAMETERS:\n"
	"  TYPE    Type of gateway\n"
	"  ID      Gateway ID\n",
	"Management of gateway",
	ShellCommandGateway
);
