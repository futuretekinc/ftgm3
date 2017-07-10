#include <iostream>
#include <iomanip>
#include <libjson/libjson.h>
#include <map>
#include <vector>
#include "exception.h"
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "device.h"
#include "time2.h"
#include "rcs_client.h"
#include "utils.h"
#include "json.h"

using namespace std;

static Fields	default_fields = PROPERTY_ID_FLAG | PROPERTY_NAME_FLAG | PROPERTY_TYPE_FLAG | PROPERTY_STAT_FLAG | PROPERTY_VALUE_FLAG;

static RetValue	CmdGatewayAdd(std::vector<std::string> const& _arguments, Shell* _shell);
static RetValue	CmdGatewayDel(std::vector<std::string> const& _arguments, Shell* _shell);
static RetValue	CmdGatewayGet(std::vector<std::string> const& _arguments, Shell* _shell);
static RetValue	CmdGatewaySet(std::vector<std::string> const& _arguments, Shell* _shell);
static RetValue	CmdGatewayStart(std::vector<std::string> const& _arguments, Shell* _shell);
static RetValue	CmdGatewayStop(std::vector<std::string> const& _arguments, Shell* _shell);
static RetValue	CmdGatewayEnable(std::vector<std::string> const& _arguments, Shell* _shell);
static RetValue	CmdGatewayDisable(std::vector<std::string> const& _arguments, Shell* _shell);
static RetValue	CmdGatewayList(std::vector<std::string> const& _arguments, Shell* _shell);
static RetValue	CmdGateway (std::string* _arguments, uint32_t _count, Shell* _shell);

Shell::Command	object_manager_command_gateway
(
	"gateway", 
	"gateway\n"
	"  Gateway management.\n"
	"COMMANDS:\n"
	"  [empty]\n"
	"    Show gateway list.\n"
	"  add [<PROPERTY> ...]\n"
	"    Add new gateway\n"
	"  del <EPID> [<EPID> ...]\n"
	"    Delete gateway\n"
	"  get <PROPERTY> [<PROPERTY> ...] <EPID> [<EPID> ...]\n"
	"    Get gateway properties\n"
	"  set <EPID> <PROPERTY> <VALUE> [<PROPERTY> <VALUE> ...]\n"
	"    Delete gateway\n"
	"  list [<PROPETY> ...]\n"
	"    Show gateway list.\n"
	"PARAMETERS"
	"  EPID      gateway id\n"
	"  PROPERTY  property type\n"
	"  VALUE     property value\n",
	"gateway management.",
	CmdGateway
 );

RetValue	CmdGateway ( std::string * _arguments, uint32_t _count, Shell* _shell)
{
	RetValue ret_value = RET_VALUE_OK;	
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<std::string> arguments;

	for(uint32_t i = 1 ; i < _count ; i++)
	{
		arguments.push_back(_arguments[i]);
	}

	if (_count == 1)
	{
		arguments.push_back(MSG_TYPE_RCS_LIST);
		CmdGatewayList(arguments, _shell);
	}
	else if (arguments[0] == MSG_TYPE_RCS_ADD)
	{
		ret_value = CmdGatewayAdd(arguments, _shell);	
	}
	else if (arguments[0] == MSG_TYPE_RCS_DEL)
	{
		ret_value = CmdGatewayDel(arguments, _shell);
	}
	else if (arguments[0] == MSG_TYPE_RCS_GET)
	{
		ret_value = CmdGatewayGet(arguments, _shell);
	}
	else if (arguments[0] == MSG_TYPE_RCS_SET)
	{
		ret_value = CmdGatewaySet(arguments, _shell);
	}
	else if (arguments[0] == "enable")
	{
		ret_value = CmdGatewayEnable(arguments, _shell);
	}
	else if (arguments[0] == "disable")
	{
		ret_value = CmdGatewayDisable(arguments, _shell);
	}
	else if (arguments[0] == MSG_TYPE_RCS_START)
	{
		ret_value = CmdGatewayStart(arguments, _shell);
	}
	else if (arguments[0] == MSG_TYPE_RCS_STOP)
	{
		ret_value = CmdGatewayStop(arguments, _shell);
	}
	else if (arguments[0] == MSG_TYPE_RCS_LIST)
	{
		CmdGatewayList(arguments, _shell);
	}
	else
	{
		THROW_INVALID_ARGUMENT("Not supported subcommand[" << arguments[0] << "]!");	
	}

	return	RET_VALUE_OK;
}

RetValue	CmdGatewayAdd( std::vector<std::string> const& _arguments, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	JSONNode	properties;

	if (_arguments.size() % 2 != 1)
	{
		throw InvalidArgument("Invalid argument");	
	}

	for(uint32_t i = 1 ; i < _arguments.size() - 1 ; i+= 2)
	{
		if ((_arguments[i].length() < 3) || (_arguments[i].substr(0, 2) != "--"))
		{
			throw InvalidArgument("Invalid argument");	
		}

		properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].length() - 2), _arguments[i+1]));
	}


	if (client->AddGateway(properties))
	{
		_shell->Out() << "The gateway has been added." << std::endl;		
	}
	else
	{
		_shell->Out() << "Failed to add gateway gateway." << std::endl;		
	}

	return	RET_VALUE_OK;
}

RetValue	CmdGatewayDel( std::vector<std::string> const& _arguments, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	if (_arguments.size() < 2)
	{
		throw InvalidArgument("Invalid argument");	
	}

	for(uint32_t i = 1 ; i < _arguments.size() ; i++)
	{
		if (client->DelGateway(_arguments[i]))
		{
			_shell->Out() << "The gateway[" << _arguments[i] << "] has been deleted." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to delete gateway[" << _arguments[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

RetValue	CmdGatewayGet( std::vector<std::string> const& _arguments, Shell* _shell)
{

	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	Fields		fields = default_fields;	
	std::map<std::string, uint32_t>	fields_size;

	if (_arguments.size() < 2)
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}

	for(uint32_t i = 2 ; i < _arguments.size() ; i++)
	{
		if ((_arguments[i].substr(0, 2) != "--") || (_arguments[i].size() < 3))
		{
			THROW_INVALID_ARGUMENT("Invalid argment!");
		}
		
		if (_arguments[i] == "--all")
		{
			fields.Set(PROPERTY_ALL_FLAG);
		}
		else
		{
			fields.Set(_arguments[i].substr(2, _arguments[i].size() - 2));
		}
	}


	JSONNode	properties;

	if (client->GetGateway(_arguments[1], fields, properties))
	{
		uint32_t	title_size = 16;

		for(JSONNode::iterator it = properties.begin() ; it != properties.end() ; it++)
		{
			if (title_size < it->name().size())
			{
				title_size = it->name().size();
			}
		}

		_shell->Out() << "< Gateway Properties >" << std::endl;
		for(JSONNode::iterator it = properties.begin() ; it != properties.end() ; it++)
		{
			_shell->Out() << std::setw(title_size) << it->name() << " : " << it->as_string() << std::endl;
		}
	}
	else
	{
		_shell->Out() << "Failed to get gateway[" << _arguments[1] << "] properties." << std::endl;
	}

	return	RET_VALUE_OK;
}

RetValue	CmdGatewaySet( std::vector<std::string> const& _arguments, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	JSONNode	properties;

	if ((_arguments.size() < 2) || ((_arguments.size() - 2) % 2 != 0))
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}

	properties.push_back(JSONNode(TITLE_NAME_ID, _arguments[1]));

	for(uint32_t i = 2 ; i + 1 < _arguments.size() ; i+=2)
	{
		if ((_arguments[i].substr(0, 2) != "--") || (_arguments[i].size() < 3))
		{
			throw InvalidArgument("Invalid argument");	
		}

		properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].length() - 2), _arguments[i+1]));
	}

	if (client->SetGateway(properties))
	{
		_shell->Out() << "The gateway[" << _arguments[1] << "] properties changed." << std::endl;	

	}
	else
	{
		_shell->Out() << "Failed to set gateway[" << _arguments[1] << "] properties." << std::endl;	
	}

	return	RET_VALUE_OK;
}

RetValue	CmdGatewayList( std::vector<std::string> const& _arguments, Shell* _shell)
{

	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<JSONNode>		gateway_id_vector;
	Fields						fields = default_fields;	
	std::map<std::string, uint32_t>	fields_size;

	if (_arguments.size() < 1)
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}

	for(uint32_t i = 1 ; i < _arguments.size() ; i++)
	{
		if ((_arguments[i].substr(0, 2) != "--") || (_arguments[i].size() < 3))
		{
			THROW_INVALID_ARGUMENT("Invalid argment!");
		}
		
		if (_arguments[i] == "--all")
		{
			fields.Set(PROPERTY_ALL_FLAG);
		}
		else
		{
			fields.Set(_arguments[i].substr(2, _arguments[i].size() - 2));
		}
	}

	if (client->GetGateway(gateway_id_vector))
	{
		if (gateway_id_vector.size() != 0)
		{
			std::vector<JSONNode>	gateway_properties_vector;
			JSONNode	properties;

			for(uint32_t i = 0 ; i < gateway_id_vector.size() ; i++)
			{
				std::string	id = JSONNodeGetID(gateway_id_vector[i]);

				if (client->GetGateway(id, fields, properties))
				{
					gateway_properties_vector.push_back(properties);
				}
			}

			std::multimap<uint32_t, std::string >	fields_name_map;

			fields.Names(fields_name_map);
		
			for(std::map<uint32_t, std::string>::iterator it = fields_name_map.begin(); it != fields_name_map.end() ; it++)
			{
				fields_size.insert(fields_size.end(), std::pair<std::string, uint32_t>(it->second, it->second.size()));	
			}

			for(uint32_t i = 0 ; i < gateway_properties_vector.size() ; i++)
			{
				for(std::map<std::string, uint32_t>::iterator it = fields_size.begin() ; it != fields_size.end() ; it++)
				{
					JSONNode::iterator property_it = gateway_properties_vector[i].find(it->first);
					if (property_it != gateway_properties_vector[i].end())
					{
						if (fields_size[it->first] < property_it->as_string().size())
						{
							fields_size[it->first] = property_it->as_string().size();
						}
					}
				}
			}

			uint32_t i = 0;
			for(std::map<std::string, uint32_t>::iterator it = fields_size.begin() ; it != fields_size.end() ; it++)
			{
				_shell->Out() << setw(it->second+1) << it->first;
			}
			_shell->Out() << std::endl;

			for(uint32_t i = 0 ; i < gateway_properties_vector.size() ; i++)
			{
				for(std::map<std::string, uint32_t>::iterator it = fields_size.begin() ; it != fields_size.end() ; it++)
				{
					JSONNode::iterator property_it = gateway_properties_vector[i].find(it->first);
					if (property_it != gateway_properties_vector[i].end())
					{
						_shell->Out() << std::setw(it->second+1) << property_it->as_string();
					}
					else
					{
						_shell->Out() << std::setw(it->second+1) << "";
					}
				}
				_shell->Out() << std::endl;
			}

		}
	}
	else
	{
		_shell->Out() << "Failed to get gateway list." << std::endl;
	}

}

RetValue	CmdGatewayStart( std::vector<std::string> const& _arguments, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<std::string>	id_list;

	if (_arguments.size() < 2)
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}
	
	if (_arguments[1] == "all")
	{
		client->GetGatewayList(id_list);
	}
	else
	{
		for(uint32_t i = 1 ; i < _arguments.size() ; i++)
		{
			id_list.push_back(_arguments[i]);	
		}
	}

	for(uint32_t i = 0 ; i < id_list.size() ; i++)
	{
		if (client->StartGateway(id_list[i]))
		{
			_shell->Out() << "The gateway[" << id_list[i] << "] has been started." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to start gateway[" << id_list[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

RetValue	CmdGatewayStop( std::vector<std::string> const& _arguments, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<std::string>	id_list;

	if (_arguments.size() < 2)
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}

	if (_arguments[1] == "all")
	{
		client->GetGatewayList(id_list);
	}
	else
	{
		for(uint32_t i = 1 ; i < _arguments.size() ; i++)
		{
			id_list.push_back(_arguments[i]);	
		}
	}

	for(uint32_t i = 0 ; i < id_list.size() ; i++)
	{
		if (client->StopGateway(id_list[i]))
		{
			_shell->Out() << "The gateway[" << id_list[i] << "] has been stopped." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to stop gateway[" << id_list[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

RetValue	CmdGatewayEnable( std::vector<std::string> const& _arguments, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<std::string>	id_list;

	if (_arguments.size() < 2)
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}

	if (_arguments[1] == "all")
	{
		client->GetGatewayList(id_list);
	}
	else
	{
		for(uint32_t i = 1 ; i < _arguments.size() ; i++)
		{
			id_list.push_back(_arguments[i]);	
		}
	}

	if (client->SetGatewayEnable(id_list, true))
	{
		_shell->Out() << "The gateway has been enabled." << std::endl;		
	}
	else
	{
		_shell->Out() << "Faild to enable gateway" << std::endl;		
	}

	return	RET_VALUE_OK;
}

RetValue	CmdGatewayDisable( std::vector<std::string> const& _arguments, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<std::string>	id_list;

	if (_arguments.size() < 2)
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}

	if (_arguments[1] == "all")
	{
		client->GetGatewayList(id_list);
	}
	else
	{
		for(uint32_t i = 1 ; i < _arguments.size() ; i++)
		{
			id_list.push_back(_arguments[i]);	
		}
	}

	if (client->SetGatewayEnable(id_list, true))
	{
		_shell->Out() << "The gateway has been enabled." << std::endl;		
	}
	else
	{
		_shell->Out() << "Faild to enable gateway" << std::endl;		
	}

	return	RET_VALUE_OK;
}

