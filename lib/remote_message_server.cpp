#include <fstream>
#include <sstream>
#include <libjson/libjson.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "defined.h"
#include "trace.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint.h"
#include "property.h"
#include "remote_message_server.h"

RemoteMessageServer::RemoteMessageServer(TCPServer *_server, int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout)
: TCPSession(_server, _socket, _addr_info, _timeout), object_manager_(NULL)
{
		
	trace.SetClassName(GetClassName());
	name_ 	= std::string("rms-") + inet_ntoa(_addr_info->sin_addr);
}

RemoteMessageServer::~RemoteMessageServer()
{
}

void	RemoteMessageServer::OnMessage(Message* _base_message)
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
					if (libjson::is_valid((char*)message_packet->data))
					{
						TRACE_INFO("Received Packet : " << message_packet->length);
						JSONNode json = libjson::parse((char*)message_packet->data);

						TRACE_INFO(json.write_formatted());
					}
					else
					{
						TRACE_INFO("Received Packet : " << message_packet->length);
						TRACE_INFO((char*)message_packet->data);
					}
				}
				else
				{
					TRACE_INFO(*message_packet);
				}

				try
				{
					JSONNode	request = RequestParse((const char*)message_packet->data);
					JSONNode	response;
			
					if (Call(request, response))
					{
						Send(response.write());
					}
				}
				catch(std::invalid_argument)
				{
					TRACE_ERROR("Invalid JSON format!");
				}
			}
		}
	}
}

JSONNode	RemoteMessageServer::RequestParse(const char* _message)
{
	if (!libjson::is_valid_unformatted(_message))
	{
		throw std::invalid_argument("Invlaid json format!");
	}

	JSONNode	node = libjson::parse_unformatted(_message);
	
	for(auto it = node.begin() ; it != node.end() ; it++)
	{
		if (it->name() == RMC_FIELD_SECTION)
		{
			if (it->type() != JSON_STRING)
			{
				throw std::invalid_argument("Invalid section type!");
			}
		}

		if (it->name() == RMC_FIELD_COMMAND)
		{
			if (it->type() != JSON_STRING)
			{
				throw std::invalid_argument("Invalid command type!");
			}
		}

		if (it->name() == RMC_FIELD_DEVICE)
		{
			if (it->type() != JSON_NODE)
			{
				throw std::invalid_argument("Invalid device type!");
			}
		}

		if (it->name() == RMC_FIELD_DEVICE_ARRAY)
		{
			if (it->type() != JSON_ARRAY)
			{
				throw std::invalid_argument("Invalid devices type!");
			}
		}

		if (it->name() == RMC_FIELD_ID)
		{
			if (it->type() != JSON_STRING)
			{
				throw std::invalid_argument("Invalid id type!");
			}
		}

		if (it->name() == RMC_FIELD_NAME)
		{
			if (it->type() != JSON_STRING)
			{
				throw std::invalid_argument("Invalid name type!");
			}
		}
	}

	return	node;
}

bool	RemoteMessageServer::Call(JSONNode& _request, JSONNode& _response)
{
	bool	result = false;
	std::ostringstream	error_message;
	try
	{
		std::string section = _request.at_nocase(RMC_FIELD_SECTION).as_string();

		_response.push_back(JSONNode(RMC_FIELD_SECTION, section));

		if (object_manager_ != NULL)
		{
			if (section == "device")
			{
				result = dms_.Service(*this, _request, _response, error_message);
			}
			else if (section == "endpoint")
			{
				result = ems_.Service(*this, _request, _response, error_message);
			}
			else
			{
				error_message << "Failed to call service becuase section is uknown!";
				TRACE_ERROR(error_message.str());
				result = false;
			}
		}
		else
		{
			error_message << "Failed to call service becuase object manager is not attached!";
			TRACE_ERROR(error_message.str());
			result = false;
		}
	}
	catch(std::out_of_range)
	{
		error_message << "Failed to call service becuase section is not found!";
		TRACE_ERROR(error_message.str());
	}

	if (result)
	{
		_response.push_back(JSONNode(RMC_FIELD_RESULT, RMC_RESULT_OK));
	}
	else
	{
		_response.push_back(JSONNode(RMC_FIELD_ERROR, error_message.str()));
		_response.push_back(JSONNode(RMC_FIELD_RESULT, RMC_RESULT_FAILED));
	}

	return	true;
}

void	RemoteMessageServer::Preprocess()
{
	ActiveObject::Preprocess();
}

void	RemoteMessageServer::Process()
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
			Message*	message = new MessagePacket(id_, receive_buffer_, receive_len_);

			Post(message);
		}
	}

	ActiveObject::Process();
}

void	RemoteMessageServer::Postprocess()
{
	ActiveObject::Postprocess();
}


void	RemoteMessageServer::SetObjectManager(ObjectManager*	_object_manager)
{
	object_manager_ = _object_manager;
}
