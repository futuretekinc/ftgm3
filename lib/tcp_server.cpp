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
#include "remote_message_server.h"


/////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////
TCPServer::TCPServer()
: port_(8888), max_session_count_(10), timeout_(60 * TIME_SECOND), onMessageCallback_(NULL), object_manager_(NULL)
{
	trace.SetClassName(GetClassName());
	enable_	= true;
	name_ 	= "tcp_server";
}

TCPServer::~TCPServer()
{
	session_map_locker_.Lock();
	for(auto it = session_map_.begin(); it != session_map_.end(); it++)
	{
		delete it->second;
	}
	session_map_locker_.Unlock();
}

void	TCPServer::Attach(ObjectManager* _object_manager)
{
	object_manager_ = _object_manager;
}

bool	TCPServer::SetPropertyInternal(Property const& _property, bool create)
{

	if (_property.GetName() == "port")
	{
		const ValueUInt32* value = dynamic_cast<const ValueUInt32*>(_property.GetValue());
		if (value == NULL)
		{
			TRACE_ERROR("Failed to set port property because value type is invalid.");
			return	false;
		}	

		port_ = value->Get();
	}
	else if (_property.GetName() == "max_session")
	{
		const ValueUInt32* value = dynamic_cast<const ValueUInt32*>(_property.GetValue());
		if (value == NULL)
		{
			TRACE_ERROR("Failed to set max_session property because value type is invalid.");
			return	false;
		}	

		max_session_count_ = value->Get();
	}
	else if (_property.GetName() == "timeout")
	{
		const ValueUInt32* value = dynamic_cast<const ValueUInt32*>(_property.GetValue());
		if (value == NULL)
		{
			TRACE_ERROR("Failed to set max_session property because value type is invalid.");
			return	false;
		}	

		timeout_ = value->Get();
	}
	else 
	{
		return	ActiveObject::SetPropertyInternal(_property, create);
	}

	return	true;
}

void	TCPServer::Preprocess()
{
	int	ret_value;

	struct sockaddr_in	server;

	socket_ = socket(AF_INET, SOCK_STREAM, 0);
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
	server.sin_addr.s_addr	= INADDR_ANY;
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
				RemoteMessageServer*	rms = new RemoteMessageServer(this,client_socket, &client, timeout_);

				object_manager_->Attach(rms);
				rms->SetTrace(true);
				rms->Start();

				session_map_locker_.Lock();
				session_map_[ntohs(client.sin_port)] = rms;
				session_map_locker_.Unlock();

				TRACE_INFO("New session created[" << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << "]");
			}
			catch(std::bad_alloc &e)
			{
				TRACE_ERROR("Failed to create message!");
			}
		}
	}
}

void	TCPServer::Postprocess()
{
	session_map_locker_.Lock();

	for(auto it = session_map_.begin(); it != session_map_.end() ; it++)
	{
		delete it->second;
	}

	session_map_locker_.Unlock();
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

	for(auto it = session_map_.begin() ; it != session_map_.end() ; it++)
	{
		_information_list.push_back(it->second->GetInformation());
	}

	session_map_locker_.Unlock();

	return	true;
}

void	TCPServer::OnMessage
(
	Message *_base_message
)
{
	switch(_base_message->type)
	{
	case	Message::SESSION_DISCONNECTED:
		{
			MessageSessionDisconnected* message = (MessageSessionDisconnected*)_base_message;

			if (message != NULL)
			{
				session_map_locker_.Lock();

				TCPSession *session = session_map_[message->port];		
				session_map_.erase(message->port);

				session_map_locker_.Unlock();
				
				session->Stop();

				delete session;

			}
		}
		break;
	}
}

