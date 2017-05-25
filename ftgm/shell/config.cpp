#include <iostream>
#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "device.h"
#include "device_snmp.h"
#include "object_manager.h"


RetValue	ShellCommandConfig
(
	std::string*	_arguments,
	uint32_t		_count,
	Shell*			_shell
)
{
	RetValue	ret_value = RET_VALUE_OK;
	ObjectManager	*manager = dynamic_cast<ObjectManager*>(_shell->GetObject());

	if (manager == NULL)
	{
		throw std::invalid_argument("The object is null!");
	}

	if (_count < 2)
	{
		JSONNode config = JSONNode(*manager);

		std::cout << config.write_formatted() << std::endl;
	}
	else if (_count == 2)
	{
	}
	else
	{
		ret_value = RET_VALUE_INVALID_ARGUMENTS;
	}

	switch(ret_value)
	{
	case	RET_VALUE_INVALID_ARGUMENTS: std::cout << "Invalid arguments!" << std::endl;	break;
	}

	return	ret_value;
}


Shell::Command	shell_ftgm_command_config = 
{
	.name		=	"config",
	.help		=	"<command> \n"
					"  Manage settings.\n"
					"COMMANDS:\n"
					"  auto_start\n"
					"    Automatically run the object\n"
					"PARAMETERS:\n",
	.short_help	=	"Manage settings",
	.function	=	ShellCommandConfig
};
