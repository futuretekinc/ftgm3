#include <iostream>
#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "device.h"
#include "device_snmp.h"
#include "object_manager.h"


RetValue	ShellCommandObject
(
	std::string*	_arguments,
	uint32_t		_count,
	Shell*			_shell
)
{
	RetValue	ret_value = RET_VALUE_OK;

	if (_count < 2)
	{
	}
	else if (_count == 2)
	{
		if (_arguments[1] == "list") 
		{
			int 	count = Object::GetCount();

			std::cout << "Object Count : " << count << std::endl;
			std::cout << std::setw(32) << "ID" << std::setw(16) << "Name" <<  std::setw(32) << "Class" << std::endl;
			for(int i = 0 ; i < count ; i++)
			{
				Object* object = Object::GetAt(i);
				if (object != NULL)
				{
					std::cout << std::setw(32) << object->GetID() << std::setw(16) << object->GetName() <<  std::setw(32) << object->GetClassName() << std::endl;
				}
			}
		}
		else
		{
			Object*	object = Object::Get(_arguments[1]);
			if (object == NULL)
			{
				std::cout << "Object[" << _arguments[1] << "] not found!" << std::endl;	
			}
		}
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


Shell::Command	shell_ftgm_command_object = 
{
	.name		=	"object",
	.help		=	"<command> \n"
					"  Management of object.\n"
					"COMMANDS:\n"
					"  list\n"
					"    List of object\n"
					"PARAMETERS:\n"
					"  ID      Object ID\n"
					"  NAME    Object Name\n",
	.short_help	=	"Management of object",
	.function	=	ShellCommandObject
};
