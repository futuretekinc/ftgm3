#include <iostream>
#include <iomanip>
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "device.h"

using namespace std;

/*RetValue	ShellCommandQuit
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* _shell
)*/
RetValue	ShellCommandQuit
(std::vector<std::string> const& _arguments, Shell* _shell, Shell::Command* _this)
{
	_shell->Stop();

	return	RET_VALUE_OK;
}

Shell::Command	object_manager_command_quit 
(
	"quit",
	"quit",
	"quit",
	ShellCommandQuit
);
