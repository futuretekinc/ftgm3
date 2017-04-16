#include "device.h"
#include "shell.h"
#include "shell_ftgm.h"
#include "object_manager.h"

int	main(int argc, char *argv[])
{
	ObjectManager	object_manager;


	Shell	shell(shell_ftgm_commands, shell_ftgm_command_count, NULL);

	shell.Start(true);

	return	0;
}
