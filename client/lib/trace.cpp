#include <iostream>
#include <iomanip>
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "device.h"

using namespace std;

RetValue	ShellCommandTrace
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* _shell
)
{
	RetValue	ret_value;
	size_t		title_width = 16;

	return	ret_value;
}

Shell::Command	object_manager_command_trace 
(
	"trace",
	"trace",
	"trace",
	ShellCommandTrace
);
