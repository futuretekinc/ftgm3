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

static Fields	default_fields = (PROPERTY_ID_FLAG	|\
							PROPERTY_TYPE_FLAG	|\
							PROPERTY_MODEL_FLAG	|\
							PROPERTY_NAME_FLAG	|\
							PROPERTY_TIME_FLAG	|\
							PROPERTY_ENABLE_FLAG|\
							PROPERTY_LOCATION_FLAG|\
							PROPERTY_REGISTERED_FLAG|\
							PROPERTY_UNIT_FLAG	|\
							PROPERTY_OPTIONS_FLAG);
							


static RetValue	CmdEndpointAdd(Shell* _shell, std::string* _arguments, uint32_t _count);
static RetValue	CmdEndpointDel(Shell* _shell, std::string* _arguments, uint32_t _count);
static RetValue	CmdEndpointGet(Shell* _shell, std::string const& _id, std::string* _arguments, uint32_t _count);
static RetValue	CmdEndpointSet(Shell* _shell, std::string const& _id, std::string* _arguments, uint32_t _count);
static RetValue	CmdEndpointStart(Shell* _shell, std::string* _arguments, uint32_t _count);
static RetValue	CmdEndpointStop(Shell* _shell, std::string* _arguments, uint32_t _count);
static RetValue	CmdEndpointEnable(Shell* _shell, std::string* _arguments, uint32_t _count);
static RetValue	CmdEndpointDisable(Shell* _shell, std::string* _arguments, uint32_t _count);
static RetValue	CmdEndpointList(Shell* _shell, std::string* _arguments, uint32_t _count);
static RetValue	CmdEndpoint (std::string* _arguments, uint32_t _count, Shell* _shell);

Shell::Command	object_manager_command_endpoint
(
	"endpoint", 
	"endpoint\n"
	"  Endpoint management.\n"
	"COMMANDS:\n"
	"  [empty]\n"
	"    Show endpoint list.\n"
	"  add [<PROPERTY> ...]\n"
	"    Add new endpoint\n"
	"  del <EPID> [<EPID> ...]\n"
	"    Delete endpoint\n"
	"  get <PROPERTY> [<PROPERTY> ...] <EPID> [<EPID> ...]\n"
	"    Get endpoint properties\n"
	"  set <EPID> <PROPERTY> <VALUE> [<PROPERTY> <VALUE> ...]\n"
	"    Delete endpoint\n"
	"  list [<PROPETY> ...]\n"
	"    Show endpoint list.\n"
	"PARAMETERS"
	"  EPID      endpoint id\n"
	"  PROPERTY  property type\n"
	"  VALUE     property value\n",
	"endpoint management.",
	CmdEndpoint
 );

Shell::Command	object_manager_command_ep
(
	"ep", 
	"ep\n"
	"  Endpoint management.\n"
	"COMMANDS:\n"
	"  [empty]\n"
	"    Show endpoint list.\n"
	"  add [<PROPERTY> ...]\n"
	"    Add new endpoint\n"
	"  del <EPID> [<EPID> ...]\n"
	"    Delete endpoint\n"
	"  get <PROPERTY> [<PROPERTY> ...] <EPID> [<EPID> ...]\n"
	"    Get endpoint properties\n"
	"  set <EPID> <PROPERTY> <VALUE> [<PROPERTY> <VALUE> ...]\n"
	"    Delete endpoint\n"
	"  list [<PROPETY> ...]\n"
	"    Show endpoint list.\n"
	"PARAMETERS"
	"  EPID      endpoint id\n"
	"  PROPERTY  property type\n"
	"  VALUE     property value\n",
	"endpoint management.",
	CmdEndpoint
 );

RetValue	CmdEndpoint ( std::string * _arguments, uint32_t _count, Shell* _shell)
{
	RetValue ret_value = RET_VALUE_OK;	
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	if (_count == 1)
	{
		ret_value = CmdEndpointList(_shell, NULL, 0);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_LIST)
	{
		ret_value = CmdEndpointList(_shell, &_arguments[2], _count - 1);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_ADD)
	{
		if (_count < 3)
		{
			THROW_INVALID_ARGUMENT("Argument too few.");	
		}
		ret_value = CmdEndpointAdd(_shell, &_arguments[2], _count - 1);	
	}
	else if (_arguments[1] == MSG_TYPE_RCS_DEL)
	{
		if (_count < 3)
		{
			THROW_INVALID_ARGUMENT("Argument too few.");	
		}
		ret_value = CmdEndpointDel(_shell, &_arguments[2], _count - 1);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_GET)
	{
		if (_count < 3)
		{
			THROW_INVALID_ARGUMENT("Argument too few.");	
		}
		ret_value = CmdEndpointGet(_shell, _arguments[2], &_arguments[3], _count - 2);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_SET)
	{
		if (_count < 5)
		{
			THROW_INVALID_ARGUMENT("Argument too few.");	
		}
		ret_value = CmdEndpointSet(_shell, _arguments[2], &_arguments[3], _count - 2);
	}
	else if (_arguments[1] == "enable")
	{
		if (_count < 2)
		{
			THROW_INVALID_ARGUMENT("Argument too few.");	
		}
		ret_value = CmdEndpointEnable(_shell, &_arguments[2], _count - 1);
	}
	else if (_arguments[1] == "disable")
	{
		if (_count < 2)
		{
			THROW_INVALID_ARGUMENT("Argument too few.");	
		}
		ret_value = CmdEndpointDisable(_shell, &_arguments[2], _count - 1);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_START)
	{
		if (_count < 2)
		{
			THROW_INVALID_ARGUMENT("Argument too few.");	
		}
		ret_value = CmdEndpointStart(_shell, &_arguments[2], _count - 1);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_STOP)
	{
		if (_count < 2)
		{
			THROW_INVALID_ARGUMENT("Argument too few.");	
		}
		ret_value = CmdEndpointStop(_shell, &_arguments[2], _count - 1);
	}
	else if (_count == 2)
	{
		ret_value = CmdEndpointGet(_shell, _arguments[1], NULL, 0);
	}
	else
	{
		THROW_INVALID_ARGUMENT("Not supported subcommand[" << _arguments[1] << "]!");	
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointAdd( Shell* _shell, std::string* _options, uint32_t _count)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	JSONNode	properties;

	if (_count % 2 != 0)
	{
		throw InvalidArgument("Invalid argument");	
	}

	for(uint32_t i = 0 ; i < _count ; i+= 2)
	{
		if ((_options[i].length() < 3) || (_options[i].substr(0, 2) != "--"))
		{
			throw InvalidArgument("Invalid argument");	
		}

		properties.push_back(JSONNode(_options[i].substr(2, _options[i].length() - 2), _options[i+1]));
	}


	if (client->AddEndpoint(properties))
	{
		_shell->Out() << "The endpoint has been added." << std::endl;		
	}
	else
	{
		_shell->Out() << "Failed to add endpoint endpoint." << std::endl;		
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointDel( Shell* _shell, std::string* _id_list, uint32_t _count)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	if (_count < 1)
	{
		throw InvalidArgument("Invalid argument");	
	}

	for(uint32_t i = 0 ; i < _count ; i++)
	{
		if (client->DelEndpoint(_id_list[i]))
		{
			_shell->Out() << "The endpoint[" << _id_list[i] << "] has been deleted." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to delete endpoint[" << _id_list[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointGet( Shell* _shell, std::string const& _id, std::string* _options, uint32_t _count)
{

	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	Fields		fields;
	std::map<std::string, uint32_t>	fields_size;

	if (_count == 0)
	{
		fields = default_fields;	
	}

	for(uint32_t i = 0 ; i < _count ; i++)
	{
		if ((_options[i].substr(0, 2) != "--") || (_options[i].size() < 3))
		{
			THROW_INVALID_ARGUMENT("Invalid argment!");
		}
		
		if (_options[i] == "--all")
		{
			fields.Set(PROPERTY_ALL_FLAG);
		}
		else
		{
			fields.Set(_options[i].substr(2, _options[i].size() - 2));
		}
	}

	JSONNode	properties;

	if (client->GetEndpoint(_id, fields, properties))
	{
		uint32_t	title_size = 16;

		for(JSONNode::iterator it = properties.begin() ; it != properties.end() ; it++)
		{
			if (title_size < it->name().size())
			{
				title_size = it->name().size();
			}
		}

		_shell->Out() << "< Endpoint Properties >" << std::endl;
		for(JSONNode::iterator it = properties.begin() ; it != properties.end() ; it++)
		{
			_shell->Out() << std::setw(title_size) << it->name() << " : " << it->as_string() << std::endl;
		}
	}
	else
	{
		_shell->Out() << "Failed to get endpoint[" << _id << "] properties." << std::endl;
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointSet( Shell* _shell, std::string const& _id, std::string* _options, uint32_t _count)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	JSONNode	properties;

	if ((_count < 2) || (_count % 2 != 0))
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}

	properties.push_back(JSONNode(TITLE_NAME_ID, _id));

	for(uint32_t i = 0 ; i + 1 < _count ; i+=2)
	{
		if ((_options[i].substr(0, 2) != "--") || (_options[i].size() < 3))
		{
			throw InvalidArgument("Invalid argument");	
		}

		properties.push_back(JSONNode(_options[i].substr(2, _options[i].length() - 2), _options[i+1]));
	}

	if (client->SetEndpoint(properties))
	{
		_shell->Out() << "The endpoint[" << _id << "] properties changed." << std::endl;	

	}
	else
	{
		_shell->Out() << "Failed to set endpoint[" << _id << "] properties." << std::endl;	
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointList( Shell* _shell, std::string* _options, uint32_t _count)
{

	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<JSONNode>		endpoint_id_vector;
	Fields						fields = default_fields;	
	std::map<std::string, uint32_t>	fields_size;

	for(uint32_t i = 0 ; i < _count ; i++)
	{
		if ((_options[i].substr(0, 2) != "--") || (_options[i].size() < 3))
		{
			THROW_INVALID_ARGUMENT("Invalid argment!");
		}
		
		if (_options[i] == "--all")
		{
			fields.Set(PROPERTY_ALL_FLAG);
		}
		else
		{
			fields.Set(_options[i].substr(2, _options[i].size() - 2));
		}
	}

	if (client->GetEndpoint(endpoint_id_vector))
	{
		if (endpoint_id_vector.size() != 0)
		{
			std::vector<JSONNode>	endpoint_properties_vector;
			JSONNode	properties;

			for(uint32_t i = 0 ; i < endpoint_id_vector.size() ; i++)
			{
				std::string	id = JSONNodeGetID(endpoint_id_vector[i]);

				if (client->GetEndpoint(id, fields, properties))
				{
					endpoint_properties_vector.push_back(properties);
				}
			}

			std::multimap<uint32_t, std::string >	fields_name_map;

			fields.Names(fields_name_map);
		
			for(std::map<uint32_t, std::string>::iterator it = fields_name_map.begin(); it != fields_name_map.end() ; it++)
			{
				fields_size.insert(fields_size.end(), std::pair<std::string, uint32_t>(it->second, it->second.size()));	
			}

			for(uint32_t i = 0 ; i < endpoint_properties_vector.size() ; i++)
			{
				for(std::map<std::string, uint32_t>::iterator it = fields_size.begin() ; it != fields_size.end() ; it++)
				{
					JSONNode::iterator property_it = endpoint_properties_vector[i].find(it->first);
					if (property_it != endpoint_properties_vector[i].end())
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

			for(uint32_t i = 0 ; i < endpoint_properties_vector.size() ; i++)
			{
				for(std::map<std::string, uint32_t>::iterator it = fields_size.begin() ; it != fields_size.end() ; it++)
				{
					JSONNode::iterator property_it = endpoint_properties_vector[i].find(it->first);
					if (property_it != endpoint_properties_vector[i].end())
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
		_shell->Out() << "Failed to get endpoint list." << std::endl;
	}

}

RetValue	CmdEndpointStart( Shell* _shell, std::string* _options, uint32_t _count)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<std::string>	id_list;

	if (_count < 1)
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}
	
	if (_options[0] == "all")
	{
		client->GetEndpointList(id_list);
	}
	else
	{
		for(uint32_t i = 0 ; i < _count ; i++)
		{
			id_list.push_back(_options[i]);	
		}
	}

	for(uint32_t i = 0 ; i < id_list.size() ; i++)
	{
		if (client->StartEndpoint(id_list[i]))
		{
			_shell->Out() << "The endpoint[" << id_list[i] << "] has been started." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to start endpoint[" << id_list[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointStop( Shell* _shell, std::string* _options, uint32_t _count)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<std::string>	id_list;

	if (_count < 0)
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}

	if (_options[0] == "all")
	{
		client->GetEndpointList(id_list);
	}
	else
	{
		for(uint32_t i = 0 ; i < _count ; i++)
		{
			id_list.push_back(_options[i]);	
		}
	}

	for(uint32_t i = 0 ; i < id_list.size() ; i++)
	{
		if (client->StopEndpoint(id_list[i]))
		{
			_shell->Out() << "The endpoint[" << id_list[i] << "] has been stopped." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to stop endpoint[" << id_list[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointEnable( Shell* _shell, std::string* _options, uint32_t _count)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<std::string>	id_list;

	if (_count < 0)
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}

	if (_options[0] == "all")
	{
		client->GetEndpointList(id_list);
	}
	else
	{
		for(uint32_t i = 0 ; i < _count ; i++)
		{
			id_list.push_back(_options[i]);	
		}
	}

	if (client->SetEndpointEnable(id_list, true))
	{
		_shell->Out() << "The endpoint has been enabled." << std::endl;		
	}
	else
	{
		_shell->Out() << "Faild to enable endpoint" << std::endl;		
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointDisable( Shell* _shell, std::string* _options, uint32_t _count)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<std::string>	id_list;

	if (_count < 1)
	{
		THROW_INVALID_ARGUMENT("Invalid argment!");
	}

	if (_options[0] == "all")
	{
		client->GetEndpointList(id_list);
	}
	else
	{
		for(uint32_t i = 0 ; i < _count ; i++)
		{
			id_list.push_back(_options[i]);	
		}
	}

	if (client->SetEndpointEnable(id_list, true))
	{
		_shell->Out() << "The endpoint has been enabled." << std::endl;		
	}
	else
	{
		_shell->Out() << "Faild to enable endpoint" << std::endl;		
	}

	return	RET_VALUE_OK;
}

