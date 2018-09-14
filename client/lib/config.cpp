#include <iostream>
#include <iomanip>
#include <libjson/libjson.h>
#include <map>
#include <vector>
#include "exception.h"
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "time2.h"
#include "rcs_client.h"
#include "utils.h"
#include "json.h"

using namespace std;

//static RetValue	CmdLoad(std::string* _arguments, uint32_t _count, Shell* _shell);

static RetValue	CmdLoad(std::vector<std::string> const& _arguments, Shell* _shell, Shell::Command* _this);
static RetValue	CmdLoadGateway(Shell* _shell, JSONNode const& node);
static RetValue	CmdLoadDevice(Shell* _shell, JSONNode const& node);
static RetValue	CmdLoadDevice(Shell* _shell, std::string const& _parent_id, JSONNode const& node);
static RetValue	CmdLoadEndpoint(Shell* _shell, JSONNode const& node);
static RetValue	CmdLoadEndpoint(Shell* _shell, std::string const& _parent_id, JSONNode const& node);

Shell::Command	object_manager_command_load
(
	"load", 
	"load\n"
	"  Device management.\n"
	"COMMANDS:\n"
	"  [empty]\n"
	"    Show load list.\n"
	"  add [<PROPERTY> ...]\n"
	"    Add new load\n"
	"  del <EPID> [<EPID> ...]\n"
	"    Delete load\n"
	"  get <PROPERTY> [<PROPERTY> ...] <EPID> [<EPID> ...]\n"
	"    Get load properties\n"
	"  set <EPID> <PROPERTY> <VALUE> [<PROPERTY> <VALUE> ...]\n"
	"    Delete load\n"
	"  list [<PROPETY> ...]\n"
	"    Show load list.\n"
	"PARAMETERS"
	"  EPID      load id\n"
	"  PROPERTY  property type\n"
	"  VALUE     property value\n",
	"load management.",
	CmdLoad
 );

RetValue	CmdLoad(std::vector<std::string> const& _arguments, Shell* _shell, Shell::Command* _this)
{
	RetValue ret_value = RET_VALUE_OK;	
	std::vector<std::string> arguments;
	uint32_t	_count = _arguments.size();
	if (_count == 2)
	{
		try
		{
			JSONNode	node = JSONNodeLoadFromFile(_arguments[1]);	

			if (JSONNodeIsExistField(node, TITLE_NAME_GATEWAY))
			{
				CmdLoadGateway(_shell, JSONNodeGetNode(node, TITLE_NAME_GATEWAY));
			}
			
			if (JSONNodeIsExistField(node, TITLE_NAME_DEVICE))
			{
				CmdLoadDevice(_shell, JSONNodeGetNode(node, TITLE_NAME_DEVICE));
			}

			if (JSONNodeIsExistField(node, TITLE_NAME_ENDPOINT))
			{
				CmdLoadEndpoint(_shell, JSONNodeGetNode(node, TITLE_NAME_ENDPOINT));
			}
		}
		catch(InvalidArgument& e)
		{
			_shell->Out() << e.what() << std::endl;	
		}

	}
	else if (_count == 3)
	{
		try
		{
			JSONNode	node = JSONNodeLoadFromFile(_arguments[2]);	

			if (_arguments[1] == TITLE_NAME_GATEWAY)
			{
				CmdLoadGateway(_shell, node);
			}
			else
			{
				_shell->Out() << "Invalid argument!" << std::endl;
			}
		}
		catch(InvalidArgument& e)
		{
			_shell->Out() << e.what() << std::endl;	
		}

	}
	else if (_count == 4)
	{
		try
		{
			JSONNode	node = JSONNodeLoadFromFile(_arguments[3]);	

			_shell->Out() << node.write_formatted() << std::endl;

			if (_arguments[1] == TITLE_NAME_DEVICE)
			{
				CmdLoadDevice(_shell, _arguments[2], node);
			}
			else if (_arguments[1] == TITLE_NAME_ENDPOINT)
			{
				CmdLoadEndpoint(_shell, _arguments[2], node);
			}
			else
			{
				_shell->Out() << "Invalid argument!" << std::endl;
			}
		}
		catch(InvalidArgument& e)
		{
			_shell->Out() << e.what() << std::endl;	
		}

	}
	else
	{
		throw InvalidArgument("Invalid argument");	
	}

	return	RET_VALUE_OK;
}


static RetValue	CmdLoadGateway(Shell* _shell, JSONNode const& node)
{
	RetValue ret_value = RET_VALUE_OK;	
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	if(node.type() == JSON_ARRAY)
	{
		for(JSONNode::const_iterator it = node.begin() ; it != node.end() ; it++)
		{
			CmdLoadGateway(_shell, *it);	
		}
	}
	else
	{
		JSONNode	gateway;

		if(client->AddGateway(node, gateway))
		{
			_shell->Out() << "Gateway added!" << std::endl;
			_shell->Out() << gateway.write_formatted() << std::endl;
		}
		else
		{
			_shell->Out() << "Failed to add gateway!" << std::endl;
			_shell->Out() << node.write_formatted() << std::endl;
		
		}
	}

	return	RET_VALUE_OK;
}

static RetValue	CmdLoadDevice(Shell* _shell, JSONNode const& node)
{
	RetValue ret_value = RET_VALUE_OK;	
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	if(node.type() == JSON_ARRAY)
	{
		for(JSONNode::const_iterator it = node.begin() ; it != node.end() ; it++)
		{
			CmdLoadDevice(_shell, *it);	
		}
	}
	else
	{
		JSONNode	result;

		if(client->AddDevice(node, result))
		{
			_shell->Out() << "Gateway added!" << std::endl;
			_shell->Out() << result.write_formatted() << std::endl;
		}
		else
		{
			_shell->Out() << "Failed to add Device!" << std::endl;
			_shell->Out() << node.write_formatted() << std::endl;
		
		}
	}

	return	RET_VALUE_OK;
}

static RetValue	CmdLoadDevice(Shell* _shell, std::string const& _parent_id, JSONNode const& node)
{
	RetValue ret_value = RET_VALUE_OK;	
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	JSONNode	gateway;

	if (!client->GetGateway(_parent_id, gateway))
	{
		_shell->Out() << "Gateway[" << _parent_id << "] not found!" << std::endl;
		return	RET_VALUE_INVALID_ID;
	}

	if(node.type() == JSON_ARRAY)
	{
		for(JSONNode::const_iterator it = node.begin() ; it != node.end() ; it++)
		{
			CmdLoadDevice(_shell, _parent_id, *it);	
		}
	}
	else
	{
		JSONNode	base;
		JSONNode	result;

		base = node;

		if (!JSONNodeIsExistField(base, TITLE_NAME_PARENT_ID))
		{
			base.push_back(JSONNode(TITLE_NAME_PARENT_ID, _parent_id));
		}
	
		if(client->AddDevice(base, result))
		{
			_shell->Out() << "Gateway added!" << std::endl;
			_shell->Out() << result.write_formatted() << std::endl;
		}
		else
		{
			_shell->Out() << "Failed to add Device!" << std::endl;
			_shell->Out() << base.write_formatted() << std::endl;
		
		}
	}

	return	RET_VALUE_OK;
}

static RetValue	CmdLoadEndpoint(Shell* _shell, JSONNode const& node)
{
	RetValue ret_value = RET_VALUE_OK;	
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	if(node.type() == JSON_ARRAY)
	{
		for(JSONNode::const_iterator it = node.begin() ; it != node.end() ; it++)
		{
			CmdLoadEndpoint(_shell, *it);	
		}
	}
	else
	{
		JSONNode	result;

		if(client->AddEndpoint(node, result))
		{
			_shell->Out() << "Endpoint added!" << std::endl;
			_shell->Out() << result.write_formatted() << std::endl;
		}
		else
		{
			_shell->Out() << "Failed to add endpoint!" << std::endl;
			_shell->Out() << node.write_formatted() << std::endl;
		
		}
	}

	return	RET_VALUE_OK;
}

static RetValue	CmdLoadEndpoint(Shell* _shell, std::string const& _parent_id, JSONNode const& node)
{
	RetValue ret_value = RET_VALUE_OK;	
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	JSONNode	device;

	if (!client->GetDevice(_parent_id, device))
	{
		_shell->Out() << "Device[" << _parent_id << "] not found!" << std::endl;
		return	RET_VALUE_INVALID_ID;
	}

	if(node.type() == JSON_ARRAY)
	{
		for(JSONNode::const_iterator it = node.begin() ; it != node.end() ; it++)
		{
			CmdLoadEndpoint(_shell, _parent_id, *it);	
		}
	}
	else
	{
		JSONNode	base;
		JSONNode	result;

		base = node;

		if (!JSONNodeIsExistField(base, TITLE_NAME_PARENT_ID))
		{
			base.push_back(JSONNode(TITLE_NAME_PARENT_ID, _parent_id));
		}
	
		if(client->AddEndpoint(base, result))
		{
			_shell->Out() << "Endpoint added!" << std::endl;
			_shell->Out() << result.write_formatted() << std::endl;
		}
		else
		{
			_shell->Out() << "Failed to add endpoint!" << std::endl;
			_shell->Out() << base.write_formatted() << std::endl;
		
		}
	}

	return	RET_VALUE_OK;
}

