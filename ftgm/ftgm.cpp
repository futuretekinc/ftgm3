#include <iostream>
#include <iostream>
#include <csignal>
#include <exception>
#include "device.h"
#include "shell.h"
#include "shell_ftgm.h"
#include "object_manager.h"
#include "data_manager.h"
#include "tcp_server.h"


static void sigterm (int sig) 
{
}


int	main(int argc, char *argv[])
{
	ObjectManager	object_manager;
	TCPServer		tcp_server;

	try
	{
		trace.Enable(true);
		trace.SetOut("/var/log/ftgm.log");

		TRACE_INFO2(NULL, "####################################");
		TRACE_INFO2(NULL, "##                                ##");
		TRACE_INFO2(NULL, "####################################");
		object_manager.SetTrace(true);
		tcp_server.SetTrace(true);

		object_manager.Start();
		while(!object_manager.IsRunning())
		{
			usleep(1000);
		}

		tcp_server.Attach(&object_manager);

		tcp_server.Start();
		while(!tcp_server.IsRunning())
		{
			usleep(1000);
		}


		//	signal(SIGINT, sigterm);
		// 	signal(SIGTERM, sigterm);

		Shell	shell(shell_ftgm_commands, shell_ftgm_command_count, &object_manager);

		shell.Run();

		tcp_server.Stop(true);
		object_manager.Stop(true);
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception occurred : " << e.what() << std::endl;	
	}

	return	0;
}
