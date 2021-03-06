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
#include "json_utils.h"
#include "exception.h"


static void sigterm (int sig) 
{
}

extern char* program_invocation_short_name;

std::string	config_file_name(std::string("/etc/") + std::string(program_invocation_short_name) + ".conf");
bool		debug_mode = false;
bool		version_print_mode = false;

void	SetOptions( int	argc, char* argv[])
{
	char opt;

    while ((opt = getopt(argc, argv, "c:dv")) != (char)(-1))
    {
        switch (opt)
        {
		case 'c':
			config_file_name = optarg;
			break;
		case 'v':
			version_print_mode = true;
			break;
		case 'd':
			debug_mode = true;
			break;
		default:
			std::cerr << "Failed to get option[" << opt << "]!" << std::endl;
			return;
        }
    }
}

int	main(int argc, char *argv[])
{

	SetOptions(argc,argv);

	try
	{
		if(version_print_mode == true)
		{
			std::cout << FTGM_VERSION << std::endl;
			return 0;
		}

		JSONNode	config = JSONNodeLoadFromFile(config_file_name);

		JSONNode	trace_config = JSONNodeGetNode(config, TITLE_NAME_TRACE);
		JSONNode	manager_config = JSONNodeGetNode(config, TITLE_NAME_OBJECT_MANAGER);

		trace_master.Load(trace_config);

		ObjectManager	object_manager;

		if (!object_manager.SetProperties(manager_config, false, false))
		{
			std::cerr << "Failed to load config!" << std::endl;	
			return	0;
		}

		TRACE_INFO2(NULL, "####################################");
		TRACE_INFO2(NULL, "## " << object_manager.GetID());
		TRACE_INFO2(NULL, "####################################");


		if (debug_mode)
		{
			object_manager.Start(5000);

			//	signal(SIGINT, sigterm);
			// 	signal(SIGTERM, sigterm);

			Shell	shell(shell_ftgm_commands, shell_ftgm_command_count, &object_manager);
			shell.SetTrace(true);
			shell.Run(1000);

			object_manager.Stop(true);
		}
		else
		{
			if (!fork())
			{
				object_manager.Run(5000);
#if 0
				object_manager.Start();
					
				sleep(1);

				while(object_manager.IsRunning())
				{
					sleep(1);	
				}
#endif
			}
			else
			{
				exit(0);	
			}
		}
	}
	catch(ObjectNotFound& e)
	{
		std::cerr << "Invalid configuration!" << std::endl;
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception occurred : " << e.what() << std::endl;	
	}

	return	0;
}


