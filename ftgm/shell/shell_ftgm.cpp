#include "shell_ftgm.h"

extern	Shell::Command	shell_ftgm_command_config;
extern	Shell::Command	shell_ftgm_command_device;
extern	Shell::Command	shell_ftgm_command_endpoint;
extern	Shell::Command	shell_ftgm_command_gateway;
extern	Shell::Command	shell_ftgm_command_object;
extern	Shell::Command	shell_ftgm_command_node;
extern	Shell::Command	shell_ftgm_command_load;
extern	Shell::Command	shell_ftgm_command_slc;
extern	Shell::Command	shell_ftgm_command_trace;

Shell::Command*	shell_ftgm_commands[] = 
{
	&shell_ftgm_command_config,
	&shell_ftgm_command_device,
	&shell_ftgm_command_endpoint,
	&shell_ftgm_command_gateway,
	&shell_ftgm_command_load,
	&shell_ftgm_command_node,
	&shell_ftgm_command_object,
	&shell_ftgm_command_slc,
	&shell_ftgm_command_trace
};

uint32_t	shell_ftgm_command_count = sizeof(shell_ftgm_commands) / sizeof(Shell::Command*);

