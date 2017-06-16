#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
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

void	LoadConfig(std::string _file_name);

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

	LoadConfig(config_file_name);

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


void	LoadConfig(std::string _file_name)
{
	std::fstream	fs(_file_name.c_str(), std::fstream::in);
	if (fs)
	{
		fs.seekg (0, fs.end);
		int length = fs.tellg();
		fs.seekg (0, fs.beg);

		char * buffer = new char [length + 1];
		fs.read(buffer, length);
		buffer[length] = 0;
		fs.close();

		if (libjson::is_valid(buffer))
		{
			JSONNode	json = libjson::parse(buffer);


			for(auto it = json.begin() ; it != json.end() ; it++)
			{
				if (it->name() == TITLE_NAME_TRACE)
				{
					trace_master.Load(*it);
				}
			}
		}
		delete buffer;
	}

}
