#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <mutex>
#include <unistd.h>
#include "trace.h"
#include "device.h"
#include "device_fte.h"
#include "endpoint.h"
#include "shell.h"
#include "tcp_client.h"

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
	Date	date;

	TRACE_INFO << setw(80) << std::setfill('#') << "" << Trace::End;
	TRACE_INFO << "Start : " << program_invocation_short_name << " - " << date << Trace::End;
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

	TCPClient	client;

	client.Start();
	Shell	shell(object_manager_shell_commands, object_manager_shell_command_count, &client);
	shell.Run();

	return	0;	
}


