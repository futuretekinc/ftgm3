#include <iostream>
#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "utils.h"
#include "gateway.h"
#include "device.h"
#include "device_snmp.h"
#include "object_manager.h"

bool	ShellCommandGatewayList(ObjectManager* object_manager);
bool	ShellCommandDeviceList(ObjectManager* object_manager);;
bool	ShellCommandEndpointList(ObjectManager* object_manager);

bool	ShellCommandNode_showNodeList(ObjectManager* object_manager);
bool	ShellCommandNode_nodeEnable(ObjectManager* object_manager, std::string const& id, bool enable);
bool	ShellCommandNode_nodeStart(ObjectManager* object_manager, std::string const& id, bool start);

RetValue	ShellCommandNode
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
			std::cout << "Node manager not attached!" << std::endl;	
		}
		if (_count < 2)
		{
			ShellCommandNode_showNodeList(object_manager);
		}
		else if (_count == 2)
		{
			if (_arguments[1] == "list") 
			{
				ShellCommandNode_showNodeList(object_manager);
			}
			else
			{
				Node*	node = object_manager->GetNode(_arguments[1]);
				if (node == NULL)
				{
					std::cout << "Node [" << _arguments[1] << "] not found!" << std::endl;	
				}
			}
		}
		else if (_arguments[1] == "enable")
		{
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				ShellCommandNode_nodeEnable(object_manager, _arguments[i], true);
			}
		}
		else if (_arguments[1] == "disable")
		{
			if (_arguments[2] == "all")
			{
				for(uint32_t i = 2 ; i < _count ; i++)
				{
					ShellCommandNode_nodeEnable(object_manager, _arguments[i], false);
				}
			}
		}
		else if (_arguments[1] == "start")
		{
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				ShellCommandNode_nodeStart(object_manager, _arguments[i], true);
			}
		}
		else if (_arguments[1] == "stop")
		{
			for(uint32_t i = 2 ; i < _count ; i++)
			{
				ShellCommandNode_nodeStart(object_manager, _arguments[i], false);
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
				Node	*node = object_manager->GetNode(_arguments[2]);

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

				node->SetProperties(properties);
			}

		}
		else if (_arguments[1] == "get")
		{
			if (_count < 4)
			{
				throw std::invalid_argument("Invalid arguments!");
			}
			else
			{
				JSONNode	properties;

				for(uint32_t i = 2 ; i  < _count ; i ++)
				{
					Node	*node = object_manager->GetNode(_arguments[i]);
					if (node != NULL)
					{
						std::cout << *node;
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
		std::cout << "Invalid arguments!" << std::endl;	
		ret_value = RET_VALUE_INVALID_ARGUMENTS;
	}

	return	ret_value;
}

bool	ShellCommandNode_showNodeList(ObjectManager* object_manager)
{
	ShellCommandGatewayList(object_manager);
	ShellCommandDeviceList(object_manager);
	ShellCommandEndpointList(object_manager);

	return	true;
}

bool	ShellCommandNode_nodeEnable(ObjectManager* object_manager, std::string const& id, bool enable)
{
	if (id == "all")
	{
		std::list<Node*>	node_list;

		object_manager->GetNodeList(node_list);

		for(auto it = node_list.begin(); it != node_list.end() ; it++)
		{
			(*it)->SetEnable(enable);
			std::cout << "The object[" << (*it)->GetTraceName() << "] is " << ((enable)?"enabled":"disabled") << std::endl;
		}
	}
	else
	{
		Node *node = object_manager->GetNode(id);
		if (node == NULL)
		{
			std::cout << "Node [" << id << "] not found!" << std::endl;
			return	false;
		}
		else
		{
			node->SetEnable(enable);
			std::cout << "The object[" << id << "] is " << ((enable)?"enabled":"disabled") << std::endl;
		}
	}

	return	true;
}

bool	ShellCommandNode_nodeStart(ObjectManager* object_manager, std::string const& id, bool start)
{
	if (id == "all")
	{
		std::list<Node*>	node_list;

		object_manager->GetNodeList(node_list);

		for(auto it = node_list.begin(); it != node_list.end() ; it++)
		{
			if (start)
			{
				if ((*it)->Start())
				{
					std::cout << "The (*it)[" << (*it)->GetID() << "] is " << "started!" << std::endl;
				}
				else
				{
					std::cout << "The (*it)[" << (*it)->GetID() << "] failed to start!" << std::endl;
				}
			}
			else
			{
				if ((*it)->Stop())
				{
					std::cout << "The (*it)[" << (*it)->GetID() << "] is " << "stopped!" << std::endl;
				}
				else
				{
					std::cout << "The (*it)[" << (*it)->GetID() << "] failed to stop!" << std::endl;
				}
			}
		}
	}
	else
	{
		Node *node  = object_manager->GetNode(id);
		if (node == NULL)
		{
			std::cout << "Node [" << id << "] not found!" << std::endl;
			return	false;
		}
		else
		{
			if (start)
			{
				if (node->Start())
				{
					std::cout << "The node[" << id << "] is " << "started!" << std::endl;
				}
				else
				{
					std::cout << "The node[" << id << "] failed to start!" << std::endl;
				}
			}
			else
			{
				if (node->Stop())
				{
					std::cout << "The node[" << id << "] is " << "stopped!" << std::endl;
				}
				else
				{
					std::cout << "The node[" << id << "] failed to stop!" << std::endl;
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
