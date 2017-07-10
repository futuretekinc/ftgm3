#include <iostream>
#include <iomanip>
#include <libjson/libjson.h>
#include <map>
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

static RetValue	CmdEndpointAdd( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdEndpointDel( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdEndpointGet( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdEndpointSet( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdEndpointStart( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdEndpointStop( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdEndpointList( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdEndpoint ( std::string* _arguments, uint32_t	_count, Shell* _shell);

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

RetValue	CmdEndpoint ( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RetValue ret_value = RET_VALUE_OK;	
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	if (_count == 1)
	{
		CmdEndpointList(NULL, 0, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_ADD)
	{
		ret_value = CmdEndpointAdd(&_arguments[2], _count - 2, _shell);	
	}
	else if (_arguments[1] == MSG_TYPE_RCS_DEL)
	{
		ret_value = CmdEndpointDel(&_arguments[2], _count - 2, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_GET)
	{
		ret_value = CmdEndpointGet(&_arguments[2], _count - 2, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_SET)
	{
		ret_value = CmdEndpointSet(&_arguments[2], _count - 2, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_START)
	{
		ret_value = CmdEndpointStart(&_arguments[2], _count - 2, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_STOP)
	{
		ret_value = CmdEndpointStop(&_arguments[2], _count - 2, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_LIST)
	{
		CmdEndpointList(&_arguments[2], _count - 2, _shell);
	}
	else
	{
		THROW_INVALID_ARGUMENT("Not supported subcommand[" << _arguments[1] << "]!");	
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointAdd( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	JSONNode	properties;

	if ((_count < 2) || (_count % 2 != 0))
	{
		throw InvalidArgument("Invalid argument");	
	}

	for(uint32_t i = 0 ; i+1 < _count ; i+=2)
	{
		if (_arguments[i].substr(0, 2) != "--")
		{
			throw InvalidArgument("Invalid argument");	
		}

		properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].length() - 2), _arguments[i+1]));
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

RetValue	CmdEndpointDel( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	for(uint32_t i = 0 ; i < _count ; i++)
	{
		if (client->DelEndpoint(_arguments[i]))
		{
			_shell->Out() << "The endpoint[" << _arguments[i] << "] has been deleted." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to delete endpoint[" << _arguments[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointGet( std::string* _arguments, uint32_t	_count, Shell* _shell)
{

	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<JSONNode>		endpoint_id_vector;
	Fields		fields;
	uint32_t	epid_offset = 0;
	std::map<std::string, uint32_t>	fields_size;

	if (_count > 1)
	{
		for(uint32_t i = 0 ; i < _count ; i++)
		{
			if (_arguments[i].substr(0, 2) != "--")
			{
				epid_offset = i;
				break;
			}
			else if (_arguments[i].size() < 3)
			{
				THROW_INVALID_ARGUMENT("Invalid argment!");
			}
			else if (_arguments[i] == "--all")
			{
				fields.Set(PROPERTY_ALL_FLAG);
			}
			else
			{
				fields.Set(_arguments[i].substr(2, _arguments[i].size() - 2));
			}
		}
	}
	else
	{
		fields = default_fields;	
	}

	if (epid_offset == _count)
	{
		if (client->GetEndpoint(endpoint_id_vector) == false)
		{
			_shell->Out() << "Failed to get endpoint list." << std::endl;
		}
	}
	else
	{
		for(uint32_t i = epid_offset ; i < _count ; i++)
		{
			JSONNode	node;
			node.push_back(JSONNode(TITLE_NAME_ID, _arguments[i]));
			endpoint_id_vector.push_back(node);
		}
	}

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

		uint32_t	title_size = 16;

		for(uint32_t i = 0 ; i < endpoint_properties_vector.size() ; i++)
		{
			for(JSONNode::iterator it = endpoint_properties_vector[i].begin() ; it != endpoint_properties_vector[i].end() ; it++)
			{
				if (title_size < it->name().size())
				{
					title_size = it->name().size();
				}
			}
		}

		for(uint32_t i = 0 ; i < endpoint_properties_vector.size() ; i++)
		{
			std::string	id = JSONNodeGetID(endpoint_properties_vector[i]);

			_shell->Out() << "< Endpoint[" << id  << "] Properties >" << std::endl;
			for(JSONNode::iterator it = endpoint_properties_vector[i].begin() ; it != endpoint_properties_vector[i].end() ; it++)
			{
				_shell->Out() << std::setw(title_size) << it->name() << " : " << it->as_string() << std::endl;
			}
		}

	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointSet( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	JSONNode	properties;

	if ((_count < 3) || (_count % 2 != 1))
	{
		throw InvalidArgument("Invalid argument");	
	}

	properties.push_back(JSONNode(TITLE_NAME_ID, _arguments[0]));

	for(uint32_t i = 1 ; i+1 < _count ; i+=2)
	{
		if (_arguments[i].substr(0, 2) != "--")
		{
			throw InvalidArgument("Invalid argument");	
		}

		properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].length() - 2), _arguments[i+1]));
	}

	if (client->SetEndpoint(properties))
	{
		_shell->Out() << "The endpoint[" << _arguments[0] << "] properties changed." << std::endl;	

	}
	else
	{
		_shell->Out() << "Failed to set endpoint[" << _arguments[0] << "] properties." << std::endl;	
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointList( std::string* _arguments, uint32_t	_count, Shell* _shell)
{

	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<JSONNode>		endpoint_id_vector;
	Fields						fields;
	std::map<std::string, uint32_t>	fields_size;

	if (_count > 1)
	{
		for(uint32_t i = 0 ; i < _count ; i++)
		{
			if ((_arguments[i].substr(0, 2) != "--") || _arguments[i].size() < 3)
			{
				THROW_INVALID_ARGUMENT("Invalid argment!");
			}

			fields.Set(_arguments[i].substr(2, _arguments[i].size() - 2));
		}
	}
	else
	{
		fields = default_fields;	
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

RetValue	CmdEndpointInfo( std::string* _arguments, uint32_t	_count, Shell* _shell)
{

	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<JSONNode>		endpoint_id_vector;
	Fields		fields;
	uint32_t	epid_offset = 0;
	std::map<std::string, uint32_t>	fields_size;

	if (_count > 1)
	{
		for(uint32_t i = 0 ; i < _count ; i++)
		{
			if (_arguments[i].substr(0, 2) != "--")
			{
				epid_offset = i;
				break;
			}
			else if (_arguments[i].size() < 3)
			{
				THROW_INVALID_ARGUMENT("Invalid argment!");
			}
			else if (_arguments[i] == "--all")
			{
				fields.Set(PROPERTY_ALL_FLAG);
			}
			else
			{
				fields.Set(_arguments[i].substr(2, _arguments[i].size() - 2));
			}
		}
	}
	else
	{
		fields = default_fields;	
	}

	if (epid_offset == _count)
	{
		if (client->GetEndpoint(endpoint_id_vector) == false)
		{
			_shell->Out() << "Failed to get endpoint list." << std::endl;
		}
	}
	else
	{
		for(uint32_t i = epid_offset ; i < _count ; i++)
		{
			JSONNode	node;
			node.push_back(JSONNode(TITLE_NAME_ID, _arguments[i]));
			endpoint_id_vector.push_back(node);
		}
	}

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

		uint32_t	title_size = 16;

		for(uint32_t i = 0 ; i < endpoint_properties_vector.size() ; i++)
		{
			for(JSONNode::iterator it = endpoint_properties_vector[i].begin() ; it != endpoint_properties_vector[i].end() ; it++)
			{
				if (title_size < it->name().size())
				{
					title_size = it->name().size();
				}
			}
		}

		for(uint32_t i = 0 ; i < endpoint_properties_vector.size() ; i++)
		{
			std::string	id = JSONNodeGetID(endpoint_properties_vector[i]);

			_shell->Out() << "< Endpoint[" << id  << "] Properties >" << std::endl;
			for(JSONNode::iterator it = endpoint_properties_vector[i].begin() ; it != endpoint_properties_vector[i].end() ; it++)
			{
				_shell->Out() << std::setw(title_size) << it->name() << " : " << it->as_string() << std::endl;
			}
		}

	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointStart( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	for(uint32_t i = 0 ; i < _count ; i++)
	{
		if (client->StartEndpoint(_arguments[i]))
		{
			_shell->Out() << "The endpoint[" << _arguments[i] << "] has been started." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to start endpoint[" << _arguments[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

RetValue	CmdEndpointStop( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	for(uint32_t i = 0 ; i < _count ; i++)
	{
		if (client->StopEndpoint(_arguments[i]))
		{
			_shell->Out() << "The endpoint[" << _arguments[i] << "] has been stopped." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to stop endpoint[" << _arguments[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

