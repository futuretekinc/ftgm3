#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <map>
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "rcs_client.h"

using namespace std;
/*
RetValue	ShellCommandConnect
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* 		_shell
)*/
RetValue	ShellCommandConnect(std::vector<std::string> const& _arguments, Shell* _shell, Shell::Command* _this)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	RetValue	ret_value = RET_VALUE_OK;
	string		server_ip = client->GetServerIP();
	uint16_t	port = client->GetServerPort();
	uint32_t	_count = _arguments.size();
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

	client->SetServerIP(server_ip);
	client->SetServerPort(port);
	if (client->Connect())
	{
		_shell->Out() << "Client started" << std::endl;
		client->Start(100);
	}
	else
	{
		_shell->Out() << "Failed to connect to server." << std::endl;
	}

finished:
	return	ret_value;
}

/*RetValue	ShellCommandDisconnect
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* _shell
)*/
RetValue	ShellCommandDisconnect(std::vector<std::string> const& _arguments, Shell* _shell, Shell::Command* _this)
{
	RetValue	ret_value = RET_VALUE_OK;
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	client->Disconnect();

	return	ret_value;
}

/*RetValue	ShellCommandSend
(
	std::string* _arguments, 
	uint32_t	_count,
	Shell* _shell
)*/
RetValue	ShellCommandSend(std::vector<std::string> const& _arguments, Shell* _shell, Shell::Command* _this)
{
	RetValue	ret_value = RET_VALUE_OK;
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	uint32_t	_count = _arguments.size();
	if (_count == 2)
	{
		//client->Send(_arguments[1].c_str(), _arguments[1].length());	
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

Shell::Command	object_manager_command_connect
(
	"connect",
	"Connect to server",
	"Connect to server",
	ShellCommandConnect
);

Shell::Command	object_manager_command_disconnect
(
	"disconnect",
	"Disconnect from server",
	"Disconnect from server",
	ShellCommandDisconnect
);

Shell::Command	object_manager_command_send
(
	"send",
	"Send message to server",
	"Send message to server",
	ShellCommandSend
);
