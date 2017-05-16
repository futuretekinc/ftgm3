#include "shell_ftgm.h"

extern	Shell::Command	shell_ftgm_command_device;
extern	Shell::Command	shell_ftgm_command_endpoint;
extern	Shell::Command	shell_ftgm_command_object;

Shell::Command*	shell_ftgm_commands[] = 
{
	&shell_ftgm_command_device,
	&shell_ftgm_command_endpoint,
	&shell_ftgm_command_object
};

uint32_t	shell_ftgm_command_count = sizeof(shell_ftgm_commands) / sizeof(Shell::Command*);

