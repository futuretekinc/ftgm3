#include <iostream>
#include <iostream>
#include <fstream>
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
	ValueID			gateway_id("xtra1234");
	ObjectManager	object_manager(gateway_id);

	try
	{
		trace_master.SetEnable(true);

		std::fstream	fs("./ftgm.conf", std::fstream::in);
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

				if (!object_manager.Load(json))
				{
					std::cerr << "Failed to load config!" << std::endl;	
				}
			}
			delete buffer;
		}


		TRACE_INFO2(NULL, "####################################");
		TRACE_INFO2(NULL, "##                                ##");
		TRACE_INFO2(NULL, "####################################");

		object_manager.SetTrace(true);

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
