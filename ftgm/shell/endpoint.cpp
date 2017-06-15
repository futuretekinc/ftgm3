#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "endpoint.h"
#include "object_manager.h"


bool	ShellCommandEndpointList(ObjectManager* object_manager);

RetValue	ShellCommandEndpoint
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
	else	if (_count < 2)
	{
		std::cout << "Endpoint count : " << object_manager->GetEndpointCount() << std::endl;
		
		std::list<Endpoint*>	snmp_list;
		if (object_manager->GetEndpointList(snmp_list) != 0)
		{
			uint32_t	id_len = 32;
			uint32_t	name_len = 16;
			uint32_t	type_len = 16;
			uint32_t	stat_len = 16;

			for(auto it = snmp_list.begin() ; it != snmp_list.end() ; it++)
			{
				uint32_t	len;

				Endpoint *endpoint = dynamic_cast<Endpoint*>(*it);
		
				len = std::string(endpoint->GetID()).size();
				if (id_len < len)
				{
					id_len  = len;
				}

				len = std::string(endpoint->GetName()).size();
				if (name_len < len)
				{
					name_len  = len;
				}

				len = std::string(endpoint->GetType()).size();
				if (type_len < len)
				{
					type_len  = len;
				}

				len = Object::ToString(endpoint->GetStat()).size();
				if (stat_len < len)
				{
					stat_len  = len;
				}
			}

			std::cout << "* SNMP Endpoint" << std::endl;
			std::cout << std::setw(id_len) << "ID";
			std::cout << " " << std::setw(name_len+1) << "Name";
			std::cout << " " << std::setw(type_len+1) << "Type";
			std::cout << " " << std::setw(stat_len+1) << "Stat";
			std::cout << " " << std::setw(id_len) << "Device ID";
			std::cout << std::endl;

			for(auto it = snmp_list.begin() ; it != snmp_list.end() ; it++)
			{
				Endpoint *endpoint = dynamic_cast<Endpoint*>(*it);

				std::cout << std::setw(id_len) << endpoint->GetID();
				std::cout << " " << std::setw(name_len+1) << endpoint->GetName();
				std::cout << " " << std::setw(type_len+1) << endpoint->GetType();
				std::cout << " " << std::setw(stat_len+1) << Object::ToString(endpoint->GetStat());
				std::cout << " " << std::setw(id_len) << endpoint->GetParentID();
				std::cout << std::endl;	
			}
		}
	}
	else if (_arguments[1] == "create")
	{
		Properties	properties;
		
		if (_count < 4)
		{
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
		}
		else
		{
			Device* device = object_manager->GetDevice(_arguments[2]);
			if (device == NULL)
			{
				ret_value = RET_VALUE_INVALID_ARGUMENTS;
			} 
			else if (!properties.AppendType(_arguments[3]))
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
					Endpoint* endpoint = device->CreateEndpoint(properties);	
					if (endpoint == NULL)
					{
						ret_value = RET_VALUE_INVALID_ARGUMENTS;
					}
					else
					{
						std::cout << "Endpoint created." << std::endl;		
						std::cout << *endpoint << std::endl;
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
				Endpoint *endpoint = object_manager->GetEndpoint(_arguments[i]);
				if (endpoint == NULL)
				{
					std::cout << "Endpoint[" << _arguments[i] << "] not found!" << std::endl;
				}
				else
				{
					delete endpoint;
					std::cout << "The endpoint[" << _arguments[i] << "] has been deleted!" << std::endl;
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
		else if (_arguments[2] == "all")
		{
			std::list<Endpoint*>	endpoint_list;

			object_manager->GetEndpointList(endpoint_list);

			for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
			{
				(*it)->Start();
				std::cout << "The endpoint[" << (*it)->GetTraceName() << "] is started!" << std::endl;
			}
		}
		else
		{
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				
				Endpoint *endpoint = object_manager->GetEndpoint(_arguments[i]);
				if (endpoint == NULL)
				{
					std::cout << "Endpoint[" << _arguments[i] << "] not found!" << std::endl;
				}
				else
				{
					std::cout << "The endpoint[" << _arguments[i] << "] has started!" << std::endl;
					endpoint->Start();
					std::cout << "The endpoint[" << _arguments[i] << "] has started!" << std::endl;
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
			std::list<Endpoint*>	endpoint_list;

			object_manager->GetEndpointList(endpoint_list);

			for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
			{
				(*it)->Stop();
				std::cout << "The endpoint[" << (*it)->GetTraceName() << "] is stopped!" << std::endl;
			}
		}
		else
		{
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				Endpoint *endpoint = object_manager->GetEndpoint(_arguments[i]);
				if (endpoint == NULL)
				{
					std::cout << "Endpoint[" << _arguments[i] << "] not found!" << std::endl;
				}
				else
				{
					endpoint->Stop();
					std::cout << "The endpoint[" << _arguments[i] << "] has stopped!" << std::endl;
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
			std::list<Endpoint*>	endpoint_list;

			object_manager->GetEndpointList(endpoint_list);

			for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
			{
				(*it)->SetEnable(true);
				std::cout << "The endpoint[" << (*it)->GetTraceName() << "] is enabled!" << std::endl;
			}
		}
		else
		{
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				Endpoint *endpoint = object_manager->GetEndpoint(_arguments[i]);
				if (endpoint == NULL)
				{
					std::cout << "Endpoint[" << _arguments[i] << "] not found!" << std::endl;
				}
				else
				{
					endpoint->SetEnable(true);
					std::cout << "The endpoint[" << _arguments[i] << "] is enabled!" << std::endl;
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
		else if (_arguments[2] == "all")
		{
			std::list<Endpoint*>	endpoint_list;

			object_manager->GetEndpointList(endpoint_list);

			for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
			{
				(*it)->SetEnable(false);
				std::cout << "The endpoint[" << (*it)->GetTraceName() << "] is disabled!" << std::endl;
			}
		}
		else
		{
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				Endpoint *endpoint = object_manager->GetEndpoint(_arguments[i]);
				if (endpoint == NULL)
				{
					std::cout << "Endpoint[" << _arguments[i] << "] not found!" << std::endl;
				}
				else
				{
					endpoint->SetEnable(false);
					std::cout << "The endpoint[" << _arguments[i] << "] is disabled!" << std::endl;
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
			Endpoint*	endpoint = object_manager->GetEndpoint(_arguments[2]);
			if (endpoint == NULL)
			{
				std::cout << "Endpoint[" << _arguments[2] << " not found!" << std::endl;
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
					endpoint->SetProperties(properties);	
				}
			}
		}
	}
	else
	{
		Endpoint*	endpoint = object_manager->GetEndpoint(_arguments[1]);
		if (endpoint == NULL)
		{
			std::cout << "Endpoint[" << _arguments[1] << " not found!" << std::endl;
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
		}
		else
		{
			Properties	properties;

			endpoint->GetProperties(properties);
			for(auto it = properties.begin() ; it != properties.end() ; it++)
			{
				std::string		name = it->GetName();
				const Value*	value = it->GetValue();

				std::cout << std::setw(16) << name << " : " <<  std::string(*value) << std::endl;
			}
		}
	
	}

	switch(ret_value)
	{
	case	RET_VALUE_INVALID_ARGUMENTS: std::cout << "Invalid arguments!" << std::endl;	break;
	}

	return	ret_value;
}


Shell::Command	shell_ftgm_command_endpoint = 
{
	.name		=	"endpoint",
	.help		=	"<command> \n"
					"  Management of endpoint.\n"
					"COMMANDS:\n"
					"  create  <TYPE> [--id <ID>] [--name <NAME>]\n"
					"    Create endpoint\n"
					"  destroy <ID> [<ID> ...]\n"
					"    Destroy endpoints.\n"
					"  start   <ID> [<ID> ...]\n"
					"    Start endpoints.\n"
					"  stop    <ID> [<ID> ...]\n"
					"    Stop endpoints.\n"
					"PARAMETERS:\n"
					"  TYPE    Type of endpoint\n"
					"  ID      Endpoint ID\n",
	.short_help	=	"Management of endpoint",
	.function	=	ShellCommandEndpoint
};

bool	ShellCommandEndpointList(ObjectManager* object_manager)
{
	std::cout << "Endpoint count : " << object_manager->GetEndpointCount() << std::endl;

	std::list<Endpoint*>	snmp_list;
	if (object_manager->GetEndpointList(snmp_list) != 0)
	{
		uint32_t	id_len = 32;
		uint32_t	name_len = 16;
		uint32_t	type_len = 16;
		uint32_t	stat_len = 16;

		for(auto it = snmp_list.begin() ; it != snmp_list.end() ; it++)
		{
			uint32_t	len;

			Endpoint *endpoint = dynamic_cast<Endpoint*>(*it);

			len = std::string(endpoint->GetID()).size();
			if (id_len < len)
			{
				id_len  = len;
			}

			len = std::string(endpoint->GetName()).size();
			if (name_len < len)
			{
				name_len  = len;
			}

			len = std::string(endpoint->GetType()).size();
			if (type_len < len)
			{
				type_len  = len;
			}

			len = Object::ToString(endpoint->GetStat()).size();
			if (stat_len < len)
			{
				stat_len  = len;
			}
		}

		std::cout << "* SNMP Endpoint" << std::endl;
		std::cout << std::setw(id_len) << "ID";
		std::cout << " " << std::setw(name_len+1) << "Name";
		std::cout << " " << std::setw(type_len+1) << "Type";
		std::cout << " " << std::setw(stat_len+1) << "Stat";
		std::cout << " " << std::setw(id_len) << "Device ID";
		std::cout << std::endl;

		for(auto it = snmp_list.begin() ; it != snmp_list.end() ; it++)
		{
			Endpoint *endpoint = dynamic_cast<Endpoint*>(*it);

			std::cout << std::setw(id_len) << endpoint->GetID();
			std::cout << " " << std::setw(name_len+1) << endpoint->GetName();
			std::cout << " " << std::setw(type_len+1) << endpoint->GetType();
			std::cout << " " << std::setw(stat_len+1) << Object::ToString(endpoint->GetStat());
			std::cout << " " << std::setw(id_len) << endpoint->GetParentID();
			std::cout << std::endl;	
		}
	}

	return	true;
}
