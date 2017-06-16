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

bool	LoadConfig(JSONNode& config, std::string const& _file_name)
{
	std::fstream	fs("./ftgm.conf", std::fstream::in);
	if (!fs.is_open())
	{
		return	false;
	}

	fs.seekg (0, fs.end);
	int length = fs.tellg();
	fs.seekg (0, fs.beg);

	char * buffer = new char [length + 1];
	fs.read(buffer, length);
	buffer[length] = 0;
	fs.close();

	if (!libjson::is_valid(buffer))
	{
		delete buffer;
		return	false;
	}

	config = libjson::parse(buffer);
	delete buffer;

	return	true;
}


int	main(int argc, char *argv[])
{

	try
	{
		JSONNode	config;
		std::string	file_name(std::string(program_invocation_short_name) + ".conf");

		if (!LoadConfig(config, file_name))
		{
			std::cerr << "Failed to load config!" << std::endl;	
			return	0;
		}

		ObjectManager	object_manager;

		if (!object_manager.Load(config))
		{
			std::cerr << "Failed to load config!" << std::endl;	
			return	0;
		}

		TRACE_INFO2(NULL, "####################################");
		TRACE_INFO2(NULL, "## " << object_manager.GetID());
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

