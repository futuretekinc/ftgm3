#include <iostream>
#include <iomanip>
#include <fstream>
#include "exception.h"
#include "shell_ftgm.h"
#include "device.h"
#include "device_snmp.h"
#include "object_manager.h"


RetValue	ShellCommandTrace
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
		THROW_INVALID_ARGUMENT("The object is null!");
	}

	if (_count < 2)
	{
		_shell->Out() << JSONNode(trace).write_formatted() << std::endl;
	}
	else if (_count == 2)
	{
		
		if (_arguments[1] == "master")
		{
			_shell->Out() << JSONNode(trace).write_formatted() << std::endl;
		}
		else 
		{
			Object*	object = Object::Get(_arguments[1]);
			if (object == NULL)
			{
				_shell->Out() << "Object[" << object->GetID() << "] not found.";
			}
			else
			{
				_shell->Out() << JSONNode(object->GetTrace()).write_formatted() << std::endl;
			}
		}
	}
	else 
	{
		if (_arguments[1] == "mode")
		{
			if (_count < 4)
			{
				THROW_INVALID_ARGUMENT("too few arguments!");
			}

			Trace::State	state;

			if (_arguments[2] == "enable")
			{
				state = Trace::ENABLE;
			}
			else if (_arguments[2] == "disable")
			{
				state = Trace::DISABLE;
			}
			else if (_arguments[2] == "undefined")
			{
				state = Trace::UNDEFINED;
			}
			else
			{
				_shell->Out() << "Trace mode has one of enable/disable/undefined" << std::endl;  
				ret_value = RET_VALUE_INVALID_ARGUMENTS;
			}

			if (ret_value == RET_VALUE_OK)
			{
				if ((_count == 4) && (_arguments[3] == "all"))
				{
					::trace.SetState(state);
				}
				else
				{
					for(uint32_t i = 4 ; i < _count ; i++)
					{
						if (_arguments[i] == "master")
						{
							trace.SetState(state);
						}
						else 
						{
							Object*	object = Object::Get(_arguments[i]);
							if (object == NULL)
							{
								_shell->Out() << "Object[" << object->GetID() << "] not found.";
								break;
							}

							object->GetTrace().SetState(state);
						}
					}
				}
			}
		}
		else
		{
			ret_value = RET_VALUE_INVALID_ARGUMENTS;
		}
	}

	switch(ret_value)
	{
	case	RET_VALUE_INVALID_ARGUMENTS: std::cout << "Invalid arguments!" << std::endl;	break;
	}

	return	ret_value;
}


Shell::Command	shell_ftgm_command_trace
(
	"trace",
	"<command> \n"
	"  Trace settings.\n"
	"COMMANDS:\n"
	"  enable\n"
	"    Object trace enable\n"
	"  disable\n"
	"    Object trace disable\n"
	"PARAMETERS:\n",
	"Manage settings",
	ShellCommandTrace
);
