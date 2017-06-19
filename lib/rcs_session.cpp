#include <fstream>
#include <sstream>
#include <errno.h>
#include <libjson/libjson.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "defined.h"
#include "trace.h"
#include "property.h"
#include "rcs_session.h"

RCSSession::RCSSession(RCSServer *_server, int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout)
: TCPSession(_server, _socket, _addr_info, _timeout)
{
		
	trace.SetClassName(GetClassName());
	name_ 	= std::string("rms-") + inet_ntoa(_addr_info->sin_addr);
}

RCSSession::~RCSSession()
{
}

bool	RCSSession::OnMessage(Message* _base_message)
{
	switch(_base_message->GetType())
	{
	case	MSG_TYPE_PACKET:
		{
		}
		break;

	default:
		return	TCPSession::OnMessage(_base_message);
	}

	return	true;
}

void	RCSSession::Process()
{
	if (socket_)
	{
		receive_len_ = recv(socket_, receive_buffer_, receive_buffer_len_, MSG_DONTWAIT);
		if (receive_len_ < 0)
		{
			if (errno != EAGAIN)
			{
				Disconnect();
				TRACE_ERROR("The socket has terminated abnormally.");
			}
		}
		else if (receive_len_ == 0)
		{
			Disconnect();
			TRACE_ERROR("The socket has terminated.");
		}
		else
		{
			receive_buffer_[receive_len_] = 0;


			if (libjson::is_valid((char *)receive_buffer_))
			{
				RCSMessage	request(libjson::parse((char *)receive_buffer_));
				RCSMessage	response;

				dynamic_cast<RCSServer*>(server_)->ServiceCall(request, response);

				if (!Send(response.GetPayload().write()))
				{
					TRACE_ERROR("Failed to send reply!");	
				}
			}
			else
			{
				TRACE_ERROR("Received message invalid.");
				TRACE_INFO("Packet Received : " << receive_len_);
				TRACE_INFO((char *)receive_buffer_);
			}
		}
	}

	ActiveObject::Process();
}

