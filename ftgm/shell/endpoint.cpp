#include <iomanip>
#include <fstream>
#include "exception.h"
#include "shell_ftgm.h"
#include "endpoint.h"
#include "object_manager.h"


bool	ShellCommandEndpointList(Shell* _shell);

RetValue	ShellCommandEndpoint
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
			_shell->Out() << "Object manager not attached!" << std::endl;
		}
		else	if (_count < 2)
		{
			_shell->Out() << "Endpoint count : " << object_manager->GetEndpointCount() << std::endl;

			std::list<Endpoint*>	snmp_list;
			if (object_manager->GetEndpointList(snmp_list) != 0)
			{
				uint32_t	id_len = 32;
				uint32_t	name_len = 16;
				uint32_t	type_len = 16;
				uint32_t	stat_len = 16;
				uint32_t	value_len = 8;

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

					len = ToString(endpoint->GetStat()).size();
					if (stat_len < len)
					{
						stat_len  = len;
					}
				}

				_shell->Out() << "* SNMP Endpoint" << std::endl;
				_shell->Out() << std::setw(id_len) << "ID";
				_shell->Out() << " " << std::setw(name_len+1) << "Name";
				_shell->Out() << " " << std::setw(type_len+1) << "Type";
				_shell->Out() << " " << std::setw(stat_len+1) << "Stat";
				_shell->Out() << " " << std::setw(id_len) << "Device ID";
				_shell->Out() << " " << std::setw(value_len) << "Value";
				_shell->Out() << std::endl;

				for(auto it = snmp_list.begin() ; it != snmp_list.end() ; it++)
				{
					Endpoint *endpoint = dynamic_cast<Endpoint*>(*it);

					_shell->Out() << std::setw(id_len) << endpoint->GetID();
					_shell->Out() << " " << std::setw(name_len+1) << endpoint->GetName();
					_shell->Out() << " " << std::setw(type_len+1) << endpoint->GetType();
					_shell->Out() << " " << std::setw(stat_len+1) << ToString(endpoint->GetStat());
					_shell->Out() << " " << std::setw(id_len) << endpoint->GetParentID();
					_shell->Out() << " " << std::setw(value_len) << endpoint->GetValue();
					_shell->Out() << std::endl;	
				}
			}
		}
		else if (_arguments[1] == "create")
		{
			JSONNode	properties;

			if (_count < 4)
			{
				throw InvalidArgument("The paramater is insufficient.");
			}

			Device* device = object_manager->GetDevice(_arguments[2]);
			if (device == NULL)
			{
				throw ObjectNotFound(_arguments[2]);
			} 

			properties.push_back(JSONNode(TITLE_NAME_TYPE, _arguments[3]));

			for(uint32_t i = 3; (ret_value == RET_VALUE_OK) && (i + 1 < _count)  ; i+=2)
			{
				std::string	field = _arguments[i].substr(2, _arguments[i].length() - 2);

				properties.push_back(JSONNode(field, _arguments[i+1]));
			}

			Endpoint* endpoint = device->CreateEndpoint(properties);	
			if (endpoint == NULL)
			{
				throw InvalidArgument(properties.write_formatted());
			}

			_shell->Out() << "Endpoint created." << std::endl;		
			_shell->Out() << *endpoint << std::endl;
		}
		else if (_arguments[1] == "destroy")
		{
			if (_count < 3)
			{
				throw InvalidArgument("The paramater is insufficient.");
			}

			for(uint32_t i = 2 ; i < _count ; i++)
			{
				Endpoint *endpoint = object_manager->GetEndpoint(_arguments[i]);
				if (endpoint == NULL)
				{
					_shell->Out() << "Endpoint[" << _arguments[i] << "] not found!" << std::endl;
				}
				else
				{
					delete endpoint;
					_shell->Out() << "The endpoint[" << _arguments[i] << "] has been deleted!" << std::endl;
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
				std::list<Endpoint*>	endpoint_list;

				object_manager->GetEndpointList(endpoint_list);

				for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
				{
					(*it)->Start();
					_shell->Out() << "The endpoint[" << (*it)->GetTraceName() << "] is started!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{

					Endpoint *endpoint = object_manager->GetEndpoint(_arguments[i]);
					if (endpoint == NULL)
					{
						_shell->Out() << "Endpoint[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						_shell->Out() << "The endpoint[" << _arguments[i] << "] has started!" << std::endl;
						endpoint->Start();
						_shell->Out() << "The endpoint[" << _arguments[i] << "] has started!" << std::endl;
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
				std::list<Endpoint*>	endpoint_list;

				object_manager->GetEndpointList(endpoint_list);

				for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
				{
					(*it)->Stop();
					_shell->Out() << "The endpoint[" << (*it)->GetTraceName() << "] is stopped!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					Endpoint *endpoint = object_manager->GetEndpoint(_arguments[i]);
					if (endpoint == NULL)
					{
						_shell->Out() << "Endpoint[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						endpoint->Stop();
						_shell->Out() << "The endpoint[" << _arguments[i] << "] has stopped!" << std::endl;
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
				std::list<Endpoint*>	endpoint_list;

				object_manager->GetEndpointList(endpoint_list);

				for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
				{
					(*it)->SetEnable(true);
					_shell->Out() << "The endpoint[" << (*it)->GetTraceName() << "] is enabled!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					Endpoint *endpoint = object_manager->GetEndpoint(_arguments[i]);
					if (endpoint == NULL)
					{
						_shell->Out() << "Endpoint[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						endpoint->SetEnable(true);
						_shell->Out() << "The endpoint[" << _arguments[i] << "] is enabled!" << std::endl;
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
				std::list<Endpoint*>	endpoint_list;

				object_manager->GetEndpointList(endpoint_list);

				for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
				{
					(*it)->SetEnable(false);
					_shell->Out() << "The endpoint[" << (*it)->GetTraceName() << "] is disabled!" << std::endl;
				}
			}
			else
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					Endpoint *endpoint = object_manager->GetEndpoint(_arguments[i]);
					if (endpoint == NULL)
					{
						_shell->Out() << "Endpoint[" << _arguments[i] << "] not found!" << std::endl;
					}
					else
					{
						endpoint->SetEnable(false);
						_shell->Out() << "The endpoint[" << _arguments[i] << "] is disabled!" << std::endl;
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

			Endpoint*	endpoint = object_manager->GetEndpoint(_arguments[2]);
			if (endpoint == NULL)
			{
				throw ObjectNotFound(_arguments[2]);
			}

			for(uint32_t i = 3; (ret_value == RET_VALUE_OK) && (i + 1 < _count)  ; i+=2)
			{
				uint32_t	len = _arguments[i].size();

				if ((len <= 2) || (_arguments[i].substr(0,2) != "--"))
				{
					std::ostringstream	oss;
					
					oss << "Invalid argument options(" << _arguments[i] << ")" << std::endl;
					throw InvalidArgument(oss.str());
				}

				properties.push_back(JSONNode(_arguments[i].substr(2, len - 2), _arguments[i+1]));
			}

			endpoint->SetProperties(properties, false, false);	
		}
		else if (_arguments[1] == "data")
		{
			JSONNode	properties;

			if ((_count < 2) || (_count > 4))
			{
				throw InvalidArgument("The paramater is insufficient.");
			}

			uint32_t	count = 100;
			if (_count == 4)
			{
				count = strtoul(_arguments[3].c_str(), 0, 10);	
			}

			Endpoint*	endpoint = object_manager->GetEndpoint(_arguments[2]);
			if (endpoint == NULL)
			{
				std::ostringstream	oss;

				oss << "Endpoint[" << _arguments[2] << " not found!" << std::endl;
				throw ObjectNotFound(oss.str());
			}

			Endpoint::ValueMap	value_map(count);

			endpoint->GetData(count, value_map);

			for(auto it = value_map.begin() ; it != value_map.end() ; it++)
			{
				_shell->Out() << it->first << " : " << it->second << std::endl;	
			}
		}
		else
		{
			Endpoint*	endpoint = object_manager->GetEndpoint(_arguments[1]);
			if (endpoint == NULL)
			{
				_shell->Out() << "Endpoint[" << _arguments[1] << " not found!" << std::endl;
				ret_value = RET_VALUE_INVALID_ARGUMENTS;
			}
			else
			{
				JSONNode	properties;
				uint32_t	title_len = 16;

				endpoint->GetProperties(properties);
				for(auto it = properties.begin() ; it != properties.end() ; it++)
				{
					if (title_len < it->name().length())
					{
						title_len = it->name().length();
					}
				}

				title_len = (title_len + 3) / 4 * 4;

				for(auto it = properties.begin() ; it != properties.end() ; it++)
				{
					_shell->Out() << std::setw(title_len) << it->name() << " : " <<  it->as_string() << std::endl;
				}

				_shell->Out() << std::setw(title_len) << TITLE_NAME_COUNT << " : " << endpoint->GetDataCount() << std::endl;
				_shell->Out() << std::setw(title_len) << TITLE_NAME_VALUE << " : " << endpoint->GetValue() << std::endl;
				_shell->Out() << std::setw(title_len) << TITLE_NAME_START_TIME << " : " << endpoint->GetDateOfFirstData() << std::endl;
				_shell->Out() << std::setw(title_len) << TITLE_NAME_END_TIME << " : " << endpoint->GetDateOfLastData() << std::endl;
				_shell->Out() << std::setw(title_len) << TITLE_NAME_LAST_REPORT_TIME << " : " << endpoint->GetLastReportTime() << std::endl;
				_shell->Out() << std::setw(title_len) << TITLE_NAME_LAST_CONFIRM_TIME << " : " << endpoint->GetLastConfirmTime() << std::endl;

			}

		}
	}
	catch(InvalidArgument& e)
	{
		_shell->Out() << e.what() << std::endl;
	}
	catch(ObjectNotFound& e)
	{
		_shell->Out() << e.what() << std::endl;
	}

	return	RET_VALUE_OK;
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

bool	ShellCommandEndpointList(Shell* _shell)
{
	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();

	_shell->Out() << "Endpoint count : " << object_manager->GetEndpointCount() << std::endl;

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

			len = ToString(endpoint->GetStat()).size();
			if (stat_len < len)
			{
				stat_len  = len;
			}
		}

		_shell->Out() << "* SNMP Endpoint" << std::endl;
		_shell->Out() << std::setw(id_len) << "ID";
		_shell->Out() << " " << std::setw(name_len) << "Name";
		_shell->Out() << " " << std::setw(type_len) << "Type";
		_shell->Out() << " " << std::setw(stat_len) << "Stat";
		_shell->Out() << " " << std::setw(id_len) << "Device ID";
		_shell->Out() << std::endl;

		for(auto it = snmp_list.begin() ; it != snmp_list.end() ; it++)
		{
			Endpoint *endpoint = dynamic_cast<Endpoint*>(*it);

			_shell->Out() << std::setw(id_len) << endpoint->GetID();
			_shell->Out() << " " << std::setw(name_len) << endpoint->GetName();
			_shell->Out() << " " << std::setw(type_len) << endpoint->GetType();
			_shell->Out() << " " << std::setw(stat_len) << ToString(endpoint->GetStat());
			_shell->Out() << " " << std::setw(id_len) << endpoint->GetParentID();
			_shell->Out() << std::endl;	
		}
	}

	return	true;
}
