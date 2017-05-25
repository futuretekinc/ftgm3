#include "shell_ftgm.h"

extern	Shell::Command	shell_ftgm_command_config;
extern	Shell::Command	shell_ftgm_command_device;
extern	Shell::Command	shell_ftgm_command_endpoint;
extern	Shell::Command	shell_ftgm_command_object;
extern	Shell::Command	shell_ftgm_command_load;

Shell::Command*	shell_ftgm_commands[] = 
{
	&shell_ftgm_command_config,
	&shell_ftgm_command_device,
	&shell_ftgm_command_endpoint,
	&shell_ftgm_command_load,
	&shell_ftgm_command_object
};

uint32_t	shell_ftgm_command_count = sizeof(shell_ftgm_commands) / sizeof(Shell::Command*);

