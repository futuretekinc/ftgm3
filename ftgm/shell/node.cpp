#include <iostream>
#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "utils.h"
#include "gateway.h"
#include "device.h"
#include "device_snmp.h"
#include "object_manager.h"

bool	ShellCommandGatewayList(Shell*_shell);
bool	ShellCommandDeviceList(Shell* _shell);;
bool	ShellCommandEndpointList(Shell* _shell);

bool	ShellCommandNode_showNodeList(Shell* _shell);
bool	ShellCommandNode_nodeEnable(Shell* _shell, std::string const& id, bool enable);
bool	ShellCommandNode_nodeStart(Shell* _shell, std::string const& id, bool start);

RetValue	ShellCommandNode
(
	const std::vector<std::string>&	_arguments,
	Shell*			_shell,
	Shell::Command*	_this
)
{
	RetValue	ret_value = RET_VALUE_OK;
	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();

	try
	{
		if (object_manager == NULL)
		{
			_shell->Out() << "Node manager not attached!" << std::endl;	
		}
		if (_arguments.size() < 2)
		{
			ShellCommandNode_showNodeList(_shell);
		}
		else if (_arguments.size() == 2)
		{
			if (_arguments[1] == "list") 
			{
				ShellCommandNode_showNodeList(_shell);
			}
			else
			{
				Node*	node = object_manager->GetNode(_arguments[1]);
				if (node == NULL)
				{
					_shell->Out() << "Node [" << _arguments[1] << "] not found!" << std::endl;	
				}
			}
		}
		else if (_arguments[1] == "enable")
		{
			for(uint32_t i = 2 ; i < _arguments.size() ; i++)
			{
				ShellCommandNode_nodeEnable(_shell, _arguments[i], true);
			}
		}
		else if (_arguments[1] == "disable")
		{
			if (_arguments[2] == "all")
			{
				for(uint32_t i = 2 ; i < _arguments.size() ; i++)
				{
					ShellCommandNode_nodeEnable(_shell, _arguments[i], false);
				}
			}
		}
		else if (_arguments[1] == "start")
		{
			for(uint32_t i = 2 ; i < _arguments.size() ; i++)
			{
				ShellCommandNode_nodeStart(_shell, _arguments[i], true);
			}
		}
		else if (_arguments[1] == "stop")
		{
			for(uint32_t i = 2 ; i < _arguments.size() ; i++)
			{
				ShellCommandNode_nodeStart(_shell, _arguments[i], false);
			}
		}
		else if (_arguments[1] == "set")
		{
			if (_arguments.size() < 4)
			{
				throw std::invalid_argument("Invalid arguments!");
			}
			else
			{
				JSONNode	properties;
				Node	*node = object_manager->GetNode(_arguments[2]);

				for(uint32_t i = 3 ; i + 1 < _arguments.size() ; i += 2)
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

				node->SetProperties(properties, false, false);
			}

		}
		else if (_arguments[1] == "get")
		{
			if (_arguments.size() < 4)
			{
				throw std::invalid_argument("Invalid arguments!");
			}
			else
			{
				JSONNode	properties;

				for(uint32_t i = 2 ; i  < _arguments.size() ; i ++)
				{
					Node	*node = object_manager->GetNode(_arguments[i]);
					if (node != NULL)
					{
						_shell->Out() << *node;
					}
				}	
			}

		}
		else
		{
			throw std::invalid_argument(_arguments[1]);
		}
	}
	catch(std::exception& e)
	{
		_shell->Out() << "Invalid arguments!" << std::endl;	
		ret_value = RET_VALUE_INVALID_ARGUMENTS;
	}

	return	ret_value;
}

bool	ShellCommandNode_showNodeList(Shell* _shell)
{
	ShellCommandGatewayList(_shell);
	ShellCommandDeviceList(_shell);
	ShellCommandEndpointList(_shell);

	return	true;
}

bool	ShellCommandNode_nodeEnable(Shell* _shell, std::string const& id, bool enable)
{
	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();

	if (id == "all")
	{
		std::list<Node*>	node_list;

		object_manager->GetNodeList(node_list);

		for(std::list<Node*>::iterator it = node_list.begin(); it != node_list.end() ; it++)
		{
			(*it)->SetEnable(enable);
			_shell->Out() << "The object[" << (*it)->GetTraceName() << "] is " << ((enable)?"enabled":"disabled") << std::endl;
		}
	}
	else
	{
		Node *node = object_manager->GetNode(id);
		if (node == NULL)
		{
			_shell->Out() << "Node [" << id << "] not found!" << std::endl;
			return	false;
		}
		else
		{
			node->SetEnable(enable);
			_shell->Out() << "The object[" << id << "] is " << ((enable)?"enabled":"disabled") << std::endl;
		}
	}

	return	true;
}

bool	ShellCommandNode_nodeStart(Shell* _shell, std::string const& id, bool start)
{
	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();

	if (id == "all")
	{
		std::list<Node*>	node_list;

		object_manager->GetNodeList(node_list);

		for(std::list<Node*>::iterator it = node_list.begin(); it != node_list.end() ; it++)
		{
			if (start)
			{
				if ((*it)->Start(1000))
				{
					_shell->Out() << "The (*it)[" << (*it)->GetID() << "] is " << "started!" << std::endl;
				}
				else
				{
					_shell->Out() << "The (*it)[" << (*it)->GetID() << "] failed to start!" << std::endl;
				}
			}
			else
			{
				if ((*it)->Stop())
				{
					_shell->Out() << "The (*it)[" << (*it)->GetID() << "] is " << "stopped!" << std::endl;
				}
				else
				{
					_shell->Out() << "The (*it)[" << (*it)->GetID() << "] failed to stop!" << std::endl;
				}
			}
		}
	}
	else
	{
		Node *node  = object_manager->GetNode(id);
		if (node == NULL)
		{
			_shell->Out() << "Node [" << id << "] not found!" << std::endl;
			return	false;
		}
		else
		{
			if (start)
			{
				if (node->Start(1000))
				{
					_shell->Out() << "The node[" << id << "] is " << "started!" << std::endl;
				}
				else
				{
					_shell->Out() << "The node[" << id << "] failed to start!" << std::endl;
				}
			}
			else
			{
				if (node->Stop())
				{
					_shell->Out() << "The node[" << id << "] is " << "stopped!" << std::endl;
				}
				else
				{
					_shell->Out() << "The node[" << id << "] failed to stop!" << std::endl;
				}
			}
		}
	}

	return	true;
}

Shell::Command	shell_ftgm_command_node = Shell::Command(
	"node",
	"<command> \n"
	"  Management of object.\n"
	"COMMANDS:\n"
	"  list\n"
	"    List of object\n"
	"PARAMETERS:\n"
	"  ID      Node ID\n"
	"  NAME    Node Name\n",
	"Management of object",
	ShellCommandNode
);
