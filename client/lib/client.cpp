#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <map>
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "tcp_client.h"

using namespace std;

RetValue	ShellCommandConnect
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* 		_shell
)
{
	TCPClient*	client = dynamic_cast<TCPClient*>(_shell->GetObject());
	RetValue	ret_value = RET_VALUE_OK;
	string		server_ip = "127.0.0.1";
	uint16_t	port;

	if (_count == 2)	
	{
		port = strtoul(_arguments[1].c_str(), 0, 10);
	}
	else if (_count == 3)
	{
		server_ip = _arguments[1];	
		port = strtoul(_arguments[2].c_str(), 0, 10);
	}
	else 
	{
		_shell->Out() << "Error : Invalid arguments" << endl;	
		ret_value = RET_VALUE_INVALID_ARGUMENTS;
		goto finished;
	}

	if (client->Connect(server_ip, port))
	{
		_shell->Out() << "Client started" << std::endl;
		client->Start();
	}
	else
	{
		_shell->Out() << "Failed to connect to server." << std::endl;
	}

finished:
	return	ret_value;
}

RetValue	ShellCommandDisconnect
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* _shell
)
{
	RetValue	ret_value = RET_VALUE_OK;
	TCPClient*	client = dynamic_cast<TCPClient*>(_shell->GetObject());

	client->Disconnect();

	return	ret_value;
}

RetValue	ShellCommandSend
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* _shell
)
{
	RetValue	ret_value = RET_VALUE_OK;
	TCPClient*	client = dynamic_cast<TCPClient*>(_shell->GetObject());

	if (_count == 2)
	{
		client->Send(_arguments[1].c_str(), _arguments[1].length());	
	}
	else 
	{
		_shell->Out() << "Error : Invalid arguments" << endl;	
		ret_value = RET_VALUE_INVALID_ARGUMENTS;
		goto finished;
	}

finished:
	return	ret_value;
}

Shell::Command	object_manager_command_connect=
{
	.command	= "connect",
	.help		= "Connect to server",
	.short_help	= "Connect to server",
	.function	= ShellCommandConnect
};

Shell::Command	object_manager_command_disconnect=
{
	.command	= "disconnect",
	.help		= "Disconnect from server",
	.short_help	= "Disconnect from server",
	.function	= ShellCommandDisconnect
};

Shell::Command	object_manager_command_send=
{
	.command	= "send",
	.help		= "Send message to server",
	.short_help	= "Send message to server",
	.function	= ShellCommandSend
};
