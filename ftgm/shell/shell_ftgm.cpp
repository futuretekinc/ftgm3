#include "shell_ftgm.h"

extern	Shell::Command	shell_ftgm_command_device;

Shell::Command*	shell_ftgm_commands[] = 
{
	&shell_ftgm_command_device
};

uint32_t	shell_ftgm_command_count = sizeof(shell_ftgm_commands) / sizeof(Shell::Command*);

