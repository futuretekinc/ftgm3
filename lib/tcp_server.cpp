#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "defined.h"
#include "tcp_server.h"
#include "tcp_session.h"
#include "trace.h"
#include "message.h"
#include "object_manager.h"
#include "rcs_session.h"


			
/////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////
TCPServer::TCPServer(ObjectManager* _manager)
: ProcessObject(), manager_(_manager), port_(8888), max_session_count_(10), timeout_(60), onMessageCallback_(NULL)
{
	trace.SetClassName(GetClassName());
	enable_	= true;
	name_ 	= "tcp_server";
}

TCPServer::~TCPServer()
{
	session_map_locker_.Lock();
	for(std::map<uint16_t, TCPSession*>::iterator it = session_map_.begin(); it != session_map_.end(); it++)
	{
		delete it->second;
	}
	session_map_locker_.Unlock();
}

TCPServer::operator JSONNode() const
{
	JSONNode	root;
	
	root.push_back(JSONNode(TITLE_NAME_PORT, port_));
	root.push_back(JSONNode(TITLE_NAME_MAX_SESSION, max_session_count_));
	root.push_back(JSONNode(TITLE_NAME_TIMEOUT, timeout_));

	JSONNode	trace_config = trace;

	if (trace_config.size() != 0)
	{
		trace_config.set_name(TITLE_NAME_TRACE);
		root.push_back(trace_config);
	}

	return	root;
}

uint16_t	TCPServer::GetPort()
{
	return	port_;
}

bool		TCPServer::SetPort(uint16_t _port, bool _check)
{
	if (!_check)
	{
		port_ = _port;
	}

	return	true;
}

bool		TCPServer::SetPort(std::string const& _port, bool _check)
{
	if (!_check)
	{
		port_ = strtoul(_port.c_str(), 0, 10);
	}

	return	true;
}

uint32_t	TCPServer::GetMaxSessionCount()
{
	return	max_session_count_;
}

bool		TCPServer::SetMaxSessionCount(uint32_t _count, bool _check)
{
	if (!_check)
	{
		max_session_count_ = _count;
	}

	return	true;
}

bool		TCPServer::SetMaxSessionCount(std::string const& _count, bool _check)
{
	if (!_check)
	{
		max_session_count_ = strtoul(_count.c_str(), 0, 10);
	}

	return	true;
}

uint32_t	TCPServer::GetTimeout()
{
	return	timeout_;
}

bool		TCPServer::SetTimeout(uint32_t	_timeout, bool _check)
{
	if (!_check)
	{
		timeout_ = _timeout;
	}

	return	true;
}

bool		TCPServer::SetTimeout(std::string const& _timeout, bool _check)
{
	if (!_check)
	{
		timeout_ = strtoul(_timeout.c_str(), 0, 10);
	}

	return	true;
}

bool	TCPServer::SetProperty(JSONNode const& _property, bool _check)
{

	bool	ret_value = true;

	if (_property.name() == TITLE_NAME_PORT)
	{
		ret_value = SetPort(_property.as_string(), _check);
	}
	else if (_property.name() == TITLE_NAME_MAX_SESSION)
	{
		ret_value = SetMaxSessionCount(_property.as_string(), _check);
	}
	else if (_property.name() == TITLE_NAME_TIMEOUT)
	{
		ret_value = SetTimeout(_property.as_string(), _check);
	}
	else 
	{
		return	ProcessObject::SetProperty(_property, _check);
	}

	return	true;
}

void	TCPServer::Preprocess()
{
	int	ret_value;

	struct sockaddr_in	server;

	//socket_ = socket(AF_INET, SOCK_STREAM, 0);
	socket_ = socket(PF_INET, SOCK_STREAM, 0);
	if (socket_ == -1)
	{
		TRACE_ERROR("Failed to create socket.");
		return;
	}

 	if( fcntl(socket_, F_SETFL, O_NONBLOCK) == -1 )
	{
       	TRACE_ERROR("Failed to set nonblocking socket.");
       	return;
	}

	server.sin_family 		= AF_INET;
	server.sin_addr.s_addr	= htonl(INADDR_ANY);
	server.sin_port 		= htons(port_);

	ret_value = bind( socket_, (struct sockaddr *)&server, sizeof(server));
	if (ret_value < 0)
	{
		TRACE_ERROR("Failed to socket binding");
		return;
	}
	TRACE_INFO("TCP Server opened[" << port_ << "]");

	listen(socket_, 3);
}

void	TCPServer::Process()
{
	bool		ret_value;
	time_t		current_time;


	int		client_socket;
	const int		client_len = sizeof(struct sockaddr_in);	
	struct sockaddr_in	client;

	if (session_map_.size() < max_session_count_)
	{
		client_socket = accept(socket_, (struct sockaddr *)&client, (socklen_t *)&client_len);
		if (client_socket > 0)
		{
			try
			{
				uint16_t	port;

				TCPSession* session = CreateSession(client_socket, &client, timeout_);

				port = ntohs(client.sin_port);

				session_map_locker_.Lock();
				session_map_[port] = session;
				TRACE_INFO("Session Port : " << port);
				TRACE_INFO("Session Count : " << session_map_.size());
				session_map_locker_.Unlock();


				if (!session->Start(10))
				{
					TRACE_ERROR("TCP session start initialization timeout!");	
				}

				TRACE_INFO("New session created[" << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << "]");
			}
			catch(std::bad_alloc &e)
			{
				TRACE_ERROR("Failed to create message!");
			}
		}
	}

	ProcessObject::Process();
}

void	TCPServer::Postprocess()
{
	session_map_locker_.Lock();

	for(std::map<uint16_t, TCPSession*>::iterator it = session_map_.begin(); it != session_map_.end() ; it++)
	{
		delete it->second;
	}

	session_map_locker_.Unlock();
}

TCPSession*	TCPServer::CreateSession(int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout)
{
	return	new TCPSession(this, _socket, _addr_info, _timeout);
}

bool	TCPServer::SessionDisconnected
(
	uint16_t port
)
{
	try
	{
		Message	*message = new MessageSessionDisconnected(port);
		Post(message);
	}
	catch(std::bad_alloc &e)
	{
		TRACE_ERROR("Failed to create message!");
		return	false;
	}

	return	true;
}


uint32_t	TCPServer::GetSessionCount()
{
	return	session_map_.size();
}

bool	TCPServer::GetSessionInformationList
(
	std::list<TCPSession::Information>& _information_list
)
{
	session_map_locker_.Lock();

	for(std::map<uint16_t, TCPSession*>::iterator it = session_map_.begin() ; it != session_map_.end() ; it++)
	{
		_information_list.push_back(it->second->GetInformation());
	}

	session_map_locker_.Unlock();

	return	true;
}

bool	TCPServer::OnMessage
(
	Message *_base_message
)
{
	switch(_base_message->GetType())
	{
	case	MSG_TYPE_SESSION_DISCONNECTED:
		{
			MessageSessionDisconnected* message = (MessageSessionDisconnected*)_base_message;

			if (message != NULL)
			{
				session_map_locker_.Lock();
				
				TCPSession *session = session_map_[message->port];		
				if (session != NULL)
				{
					TRACE_INFO("Session[" << message->port <<"] closed.");
					session_map_.erase(message->port);
					session->Stop();
					delete session;
				}
				else
				{
					TRACE_ERROR("Session[" << message->port <<"] not found!");
				}

				session_map_locker_.Unlock();
			
			}
		}
		break;

	default:
		return	ProcessObject::OnMessage(_base_message);
	}

	return	true;
}

