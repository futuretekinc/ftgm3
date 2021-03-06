#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <libjson/libjson.h>
#include "defined.h"
#include "tcp_server.h"
#include "tcp_client.h"
#include "tcp_session.h"
#include "trace.h"

using namespace std;

TCPClient::Frame::Frame
(
	uint8_t *_data, 
	uint32_t _len
)
:	data(NULL), len(_len)
{
	if (len != 0)
	{
		data = new uint8_t[len];
		memcpy(data, _data, len);
	}
}

TCPClient::Frame::~Frame()
{
	if (data != NULL)
	{
		delete data;	
	}
}

TCPClient::TCPClient()
:ActiveObject(), server_ip_(DEFAULT_CONST_RCS_SERVER_IP), server_port_(DEFAULT_CONST_RCS_SERVER_PORT), socket_(0)
{
	trace.SetClassName(GetClassName());
	enable_	= true;
	
	TRACE_INFO("TCP Client created");
}

TCPClient::TCPClient(std::string const& _ip, uint16_t _port)
:ActiveObject(), server_ip_(_ip), server_port_(_port), socket_(0)
{
	trace.SetClassName(GetClassName());
	enable_	= true;
	
	TRACE_INFO("TCP Client created");
}

TCPClient::~TCPClient()
{
	locker_.Lock();

	while(receive_packet_list_.size())
	{
		Frame* frame = receive_packet_list_.front()	;

		delete frame;

		receive_packet_list_.pop_front();
	}

	locker_.Unlock();

}

bool	TCPClient::Connect()
{
	int		ret_value;
	int		client_socket;
	struct sockaddr_in	server;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1)
	{
		TRACE_ERROR("Failed to create socket.");
		return	false;
	}

#if 0
 	if( fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1 )
	{
       	TRACE_ERROR("Failed to set nonblocking socket.");
       	return false;
	}
#endif			 

	server.sin_family 		= AF_INET;
	server.sin_addr.s_addr	= inet_addr(server_ip_.c_str());
	server.sin_port 		= htons(server_port_);

	ret_value = connect( client_socket, (struct sockaddr *)&server, sizeof(server));
	if (ret_value < 0)
	{
		close(client_socket);
		TRACE_ERROR("Failed to connect to " << server_ip_ << ":" << server_port_);
		return false;
	}

	TRACE_INFO("The client was connected to the server[" << server_ip_ << ":" << server_port_ << "] using port ");
	server.sin_port 		= htons(server_port_);
	socket_ = client_socket;

	return	true;
}

bool	TCPClient::Connect(const string&	_server_ip, uint16_t		_server_port)
{

	server_ip_ 	= _server_ip;
	server_port_= _server_port;

	return	Connect();
}

bool	TCPClient::Disconnect()
{
	if (socket_ > 0)
	{
		shutdown(socket_, 0);
		socket_ = 0;
	}

	return	false;
}

bool	TCPClient::IsConnected()
{
	return	(socket_ != 0);
}

bool	TCPClient::Send(std::string const& _message)
{
	return	Send(_message.c_str(), _message.size());	
}

bool	TCPClient::Send
(
	const void*	_frame,
	uint32_t	_frame_len
)
{
	uint32_t	send_len;


	TRACE_INFO("Send Message : " << std::oct << _frame_len);
	TRACE_INFO_DUMP((const char *)_frame, _frame_len);		

	send_len = send(socket_, _frame, _frame_len, MSG_DONTWAIT);
	if (send_len != _frame_len)
	{
		TRACE_ERROR("Failed to send frame!");
		return	false;	
	}

	return	true;
}

bool	TCPClient::Receive
(
	uint32_t&	_buffer_len
)
{
	locker_.Lock();

	if (receive_packet_list_.size() != 0)
	{
		Frame* frame = receive_packet_list_.front()	;

		_buffer_len = frame->len;

		locker_.Unlock();

		return	true;
	}

	locker_.Unlock();

	return	false;
}

bool	TCPClient::Receive
(
	void*		_buffer, 
	uint32_t&	_buffer_len
)
{
	uint32_t	received_len = 0;

	locker_.Lock();

	if (receive_packet_list_.size() != 0)
	{
		Frame* frame = receive_packet_list_.front()	;

		if (frame->len <= _buffer_len)
		{
			memcpy(_buffer, frame->data, frame->len);	
			_buffer_len = frame->len;

			delete frame;


			receive_packet_list_.pop_front();

			locker_.Unlock();

			return	true;
		}
		else
		{
			TRACE_ERROR("Buffer too small[ < " << frame->len << "]");
			_buffer_len = frame->len;	
		}
	}
	else
	{
		_buffer_len  = 0;	
	}

	locker_.Unlock();

	return	false;
}

bool		TCPClient::RequestAndReply(std::string const& _request, std::string& _reply, uint32_t _timeout)
{
	if (_request.size() == 0)
	{
		TRACE_ERROR("Failed to send because content is null!");
	}
	else if (Send(_request))
	{   
		Timer   timer;

		timer.Add(_timeout);
		while(timer.RemainTime() > 0)
		{   
			if (GetReceivedPacketCount() > 0)
			{   
				uint32_t	frame_len = 0;
				
				if (Receive(frame_len))
				{
					try
					{
						char *frame = new char[frame_len + 1];

						if (Receive(frame, frame_len))
						{   
							frame[frame_len] = 0;

							_reply = frame;

							delete frame;

							return	true;
						}   

						delete frame;
					}
					catch(exception& e)
					{
						TRACE_ERROR("Failed to new char[" << frame_len + 1<< "]");
						return	false;
					}
				
				}
				else
				{
					TRACE_ERROR("Failed to get receive buffer size!");	
				}
			}   
			usleep(TIME_MILLISECOND);
		}   
	}   
	else
	{   
		TRACE_ERROR("Failed to send request!");
	}   

	return	false;
}

uint32_t	TCPClient::GetReceivedPacketCount()
{
	return	receive_packet_list_.size();
}

void	TCPClient::SetMessageProcessID(std::string const& _process_id)
{
	message_process_id_ = _process_id;
}

void	TCPClient::Preprocess()
{
	TRACE_INFO("TCP client started!");
}

void	TCPClient::Process()
{
	bool	ret_value;

	char	buffer[4096];
	int32_t	receive_len;

	if (socket_ > 0)
	{
		receive_len = recv(socket_, buffer, sizeof(buffer), MSG_DONTWAIT);
		if (receive_len < 0)
		{
			if (errno != EAGAIN)
			{
				stop_ = true;

				TRACE_ERROR("The socket has terminated abnormally.");
			}
		}
		else if (receive_len > 0)
		{
			bool	binary_message = false;

			buffer[receive_len] = 0;
			TRACE_INFO("Receive Message : " << std::oct << receive_len);
			TRACE_INFO_DUMP(buffer, receive_len);		

			locker_.Lock();
			if (receive_packet_list_.size() < 10)
			{
				try
				{
					if (std::string(message_process_id_).size() != 0)
					{
						TRACE_INFO("Send message to " << message_process_id_);
						Message::SendPacket(message_process_id_, id_, buffer, receive_len);
					}
					else
					{
						Frame	*frame = new Frame((uint8_t *)buffer, receive_len);

						TRACE_INFO("Push message to list");
						receive_packet_list_.push_back(frame);
					}
				}
				catch(std::bad_alloc &e)
				{
					TRACE_ERROR("Failed to create frame[receive_len = " <<  receive_len << "]");	
				
				}
			}
			else
			{
				TRACE_INFO("Receive packet buffer full!");
			}
			locker_.Unlock();
		}
	}
}

void	TCPClient::Postprocess()
{
	for(std::list<Frame*>::iterator it = receive_packet_list_.begin(); it != receive_packet_list_.end() ; it++)
	{
		delete *it;
	}
}


