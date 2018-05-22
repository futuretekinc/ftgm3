#include <iostream>
#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "utils.h"
#include "gateway.h"
#include "device.h"
#include "device_snmp.h"
#include "object_manager.h"

RetValue	ShellCommandRule
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
			_shell->Out() << "Rule manager not attached!" << std::endl;	
		}
		if (_arguments.size() < 2)
		{
			for(int i = 0 ; i < object_manager->GetRuleManager().GetRuleCount() ; i++)
			{
				JSONNode	json;

				Rule* rule = object_manager->GetRuleManager().GetRule(i);

				json = JSONNode(*rule);
				_shell->Out() << json.write_formatted() << std::endl;
			}
		}
	}
	catch(std::exception& e)
	{
		_shell->Out() << "Invalid arguments!" << std::endl;	
		ret_value = RET_VALUE_INVALID_ARGUMENTS;
	}

	return	ret_value;
}


Shell::Command	shell_ftgm_command_rule = Shell::Command(
	"rule",
	"<command> \n"
	"  Management of rule.\n"
	"COMMANDS:\n"
	"  list\n"
	"    List of rule\n"
	"PARAMETERS:\n"
	"  ID      Rule ID\n"
	"  NAME    Rule Name\n",
	"Management of object",
	ShellCommandRule
);
