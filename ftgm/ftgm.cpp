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

	try
	{
		trace_master.SetEnable(true);

		if (!object_manager.LoadFromFile("./ftgm.conf"))
		{
			std::cerr << "Failed to load config!" << std::endl;	
		}

		TRACE_INFO2(NULL, "####################################");
		TRACE_INFO2(NULL, "##                                ##");
		TRACE_INFO2(NULL, "####################################");

		object_manager.Start();
		while(!object_manager.IsRunning())
		{
			usleep(1000);
		}

		//	signal(SIGINT, sigterm);
		// 	signal(SIGTERM, sigterm);

		Shell	shell(shell_ftgm_commands, shell_ftgm_command_count, &object_manager);

		shell.Run();

		object_manager.Stop(true);
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception occurred : " << e.what() << std::endl;	
	}

	return	0;
}
