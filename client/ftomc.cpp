#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <mutex>
#include <unistd.h>
#include "trace.h"
#include "defined.h"
#include "device.h"
#include "device_fte.h"
#include "endpoint.h"
#include "shell.h"
#include "rcs_client.h"

using namespace std;

extern	Shell::Command*	object_manager_shell_commands[];
extern	int	object_manager_shell_command_count;

int main
(
	int		argc,
	char*	argv[]
)
{
	int		option;
	bool	start_shell = true;
	bool	show_usage = false;
	bool	load_from_file = false;
	string	config_file_name = string(program_invocation_short_name) + string(".conf");
	std::string	rcs_server_ip = DEFAULT_CONST_RCS_SERVER_IP;
	uint16_t	rcs_server_port=DEFAULT_CONST_RCS_SERVER_PORT;
	Date	date;

	trace_master.SetEnable(true);

	TRACE_INFO(setw(80) << std::setfill('#') << "");
	TRACE_INFO("Start : " << program_invocation_short_name << " - " << date);
	while((option = getopt(argc, argv, "i?")) != -1) 
	{   
		switch(option)
		{   
		case    'i':
			{   
				load_from_file = true;
				config_file_name = optarg;
			}   
			break;

		case    '?':
		default:
			show_usage = true;
		}   
	}   

	RCSClient	client;

	client.SetDumpPacket(true);

	client.Start();
	Shell	shell(object_manager_shell_commands, object_manager_shell_command_count, &client);
	shell.Run();

	return	0;	
}


