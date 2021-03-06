#include <iostream>
#include <iomanip>
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "device.h"

using namespace std;

extern	Shell::Command	object_manager_command_connect;
extern	Shell::Command	object_manager_command_disconnect;
extern	Shell::Command	object_manager_command_device;
extern	Shell::Command	object_manager_command_data;
extern	Shell::Command	object_manager_command_endpoint;
extern	Shell::Command	object_manager_command_ep;
extern	Shell::Command	object_manager_command_gateway;
extern	Shell::Command	object_manager_command_load;
extern	Shell::Command	object_manager_command_rcs;
extern	Shell::Command	object_manager_command_send;
extern	Shell::Command	object_manager_command_trace;
extern	Shell::Command	object_manager_command_quit;

Shell::Command*	object_manager_shell_commands[] = 
{
	&object_manager_command_connect,
	&object_manager_command_disconnect,
	&object_manager_command_data,
	&object_manager_command_device,
	&object_manager_command_endpoint,
	&object_manager_command_ep,
	&object_manager_command_gateway,
	&object_manager_command_load,
	&object_manager_command_rcs,
	&object_manager_command_send,
	&object_manager_command_trace,
	&object_manager_command_quit
};

int	object_manager_shell_command_count = sizeof(object_manager_shell_commands) / sizeof(Shell::Command *);

