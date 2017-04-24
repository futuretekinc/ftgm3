#include "device.h"
#include "shell.h"
#include "shell_ftgm.h"
#include "object_manager.h"
#include "data_manager.h"
#include "tcp_server.h"


int	main(int argc, char *argv[])
{
	ObjectManager	object_manager;
	DataManager		data_manager("./ftgm.db");
	TCPServer		tcp_server;

	data_manager.Start();
	while(!data_manager.IsRunning())
	{
		usleep(1000);
	}

	object_manager.Attach(&data_manager);
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


	Shell	shell(shell_ftgm_commands, shell_ftgm_command_count, &object_manager);

	shell.Run();

	return	0;
}
