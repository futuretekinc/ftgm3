#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "tcp_session.h"
#include "tcp_server.h"
#include "trace.h"


TCPSession::TCPSession
(
	TCPServer*		_server,
	int				_socket,
	struct sockaddr_in *_addr_info,
	uint32_t		_timeout
)
: ActiveObject(), server_(_server), process_id_(""), socket_(_socket)

{
	trace.SetClassName(GetClassName());
	enable_	= true;

	information_.addr_info = *_addr_info;
	information_.timeout = _timeout;

	receive_buffer_len_ = 4096;
	receive_buffer_ = new uint8_t[receive_buffer_len_];

	send_buffer_len_ = 4096;
	send_buffer_ = new uint8_t[send_buffer_len_];

	TRACE_INFO("TCP Session[" << id_ << "] created.");
}

TCPSession::~TCPSession()
{
	delete receive_buffer_;
	delete send_buffer_;
}	

bool	TCPSession::Disconnect()
{
	if (socket_ != 0)
	{
		shutdown(socket_, 0);
		socket_ = 0;
		if (server_ != NULL)
		{
			server_->SessionDisconnected(information_.addr_info.sin_port);
		}
	}

	return	true;
}

void	TCPSession::OnMessage(Message* _base_message)
{
	switch(_base_message->type)
	{
	case	Message::Type::PACKET:
		{
			MessagePacket *message_packet = dynamic_cast<MessagePacket*>(_base_message);
			if (message_packet != NULL)
			{
				bool	printable = true;
				for(uint32_t i = 0 ; i < message_packet->length; i++)
				{
					if (!isprint(message_packet->data[i]))
					{
						TRACE_INFO("Not printable character : " << message_packet->data[i]);
						printable = false;
						break;
					}
				}

				if(printable)
				{
					TRACE_INFO("Received Packet : " << message_packet->length);
					TRACE_INFO((char*)message_packet->data);
				}
				else
				{
					TRACE_INFO(*message_packet);
				}

				if (socket_)
				{
					int	send_len = send(socket_, message_packet->data, message_packet->length, MSG_DONTWAIT);
					if (send_len <= 0)
					{
						Disconnect();
						TRACE_ERROR("The socket has terminated.");
					}
					else if (send_len != message_packet->length)
					{
						TRACE_ERROR("Failed to send packet.[ packet_size = " << message_packet->length << ", send_size = " << send_len << " ]");
					}
					else
					{
						TRACE_INFO("The packet sent to client!");
					}
				}
			}
			else
			{
				TRACE_ERROR("Invalid message!");	
			}
		}
		break;

	default:
		{
			TRACE_ERROR("Not supported message received!");	
		}
	}
}

void	TCPSession::Preprocess()
{
	TRACE_INFO("TCP Session preprocess!");
	ActiveObject::Preprocess();
}

void	TCPSession::Process()
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
			TRACE_INFO("Received packet send to [ " << id_ << " -> " << process_id_ << " ]");
			Message::SendPacket(process_id_, id_, receive_buffer_, receive_len_);
		}
	}

	ActiveObject::Process();
}

void	TCPSession::Postprocess()
{
	Disconnect();
	ActiveObject::Postprocess();
}

bool	TCPSession::Send(std::string const& _message)
{
	uint32_t	send_len;

	send_len = send(socket_, _message.c_str(), _message.size(), MSG_DONTWAIT);
	if (_message.size() != send_len)
	{
		TRACE_ERROR("The socket failed to send data.");
		return	false;
	}

	return	true;
}

bool	TCPSession::Send
(
	void*		data, 
	uint32_t 	len
)
{
	uint32_t	send_len;

	send_len = send(socket_, data, len, MSG_DONTWAIT);
	if (len != send_len)
	{
		TRACE_ERROR("The socket failed to send data.");
		return	false;
	}

	return	true;
}

const
TCPSession::Information&	TCPSession::GetInformation()
{
	return	information_;
}
