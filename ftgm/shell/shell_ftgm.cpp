#include "shell_ftgm.h"

extern	Shell::Command	shell_ftgm_command_device;
extern	Shell::Command	shell_ftgm_command_endpoint;

Shell::Command*	shell_ftgm_commands[] = 
{
	&shell_ftgm_command_device,
	&shell_ftgm_command_endpoint
};

uint32_t	shell_ftgm_command_count = sizeof(shell_ftgm_commands) / sizeof(Shell::Command*);

