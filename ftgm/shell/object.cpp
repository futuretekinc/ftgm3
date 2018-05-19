#include <iostream>
#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "utils.h"
#include "gateway.h"
#include "device.h"
#include "device_snmp.h"
#include "object_manager.h"

RetValue	ShellCommandObject
(
	const std::vector<std::string>&	_arguments,
	Shell*			_shell
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
			uint32_t	count = Object::GetCount();
			uint32_t	name_len = 8;
			uint32_t	id_len = 8;
			uint32_t	class_len = 8;

			for(uint32_t i = 0 ; i < count ; i++)
			{
				Object*	object = Object::GetAt(i);

				if (object != NULL)
				{
					if (name_len < object->GetName().size())
					{
						name_len = 	object->GetName().size();
					}

					if (id_len < object->GetID().size())
					{
						id_len = 	object->GetID().size();
					}

					if (class_len < strlen(object->GetClassName()))
					{
						class_len =	strlen(object->GetClassName());
					}
				}
			}

			id_len += 1;
			name_len += 1;
			class_len += 1;

			_shell->Out() << std::setw(id_len) << "ID" << std::setw(name_len) << "Name" << std::setw(class_len) << "Class" << std::endl;
			for(uint32_t i = 0 ; i < count ; i++)
			{
				Object*	object = Object::GetAt(i);

				if (object != NULL)
				{
					_shell->Out() << std::setw(id_len) << object->GetID() << std::setw(name_len) << object->GetName() << std::setw(class_len) << object->GetClassName() << std::endl;
				}
			}
		}
		else if (_arguments.size() == 2)
		{
			if (_arguments[1] == "list") 
			{
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

Shell::Command	shell_ftgm_command_object = Shell::Command(
	"object",
	"<command> \n"
	"  Management of object.\n"
	"COMMANDS:\n"
	"  list\n"
	"    List of object\n"
	"PARAMETERS:\n"
	"  ID      Object ID\n"
	"  NAME    Object Name\n",
	"Management of object",
	ShellCommandObject
);
