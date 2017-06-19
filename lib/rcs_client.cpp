#include "defined.h"
#include "rcs_client.h"
#include "rcs_message.h"
#include "gateway.h"
#include "md5.h"
#include "json_utils.h"
#include "exception.h"
#include "utils.h"

RCSClient::RCSClient()
: tcp_client_(DEFAULT_CONST_RCS_SERVER_IP, DEFAULT_CONST_RCS_SERVER_PORT), secret_code_(""), dump_packet_(false)
{
	enable_ = true;
	tcp_client_.SetEnable(true);
	server_ip_	= DEFAULT_CONST_RCS_SERVER_IP;
	server_port_= DEFAULT_CONST_RCS_SERVER_PORT;
}


bool	RCSClient::SetServerIP(std::string const& _server_ip)
{
	server_ip_	= _server_ip;

	return	true;
}

const std::string&	RCSClient::GetServerIP()
{
	return	server_ip_;
}

bool	RCSClient::SetServerPort(uint16_t _server_port)
{
	server_port_ = _server_port;

	return	true;
}

uint16_t	RCSClient::GetServerPort()
{
	return	server_port_;
}

bool	RCSClient::SetDumpPacket(bool _dump_packet)
{
	dump_packet_ = _dump_packet;
	tcp_client_.SetTrace(dump_packet_);

	return	true;
}

bool	RCSClient::OnMessage(Message* _message)
{
	return	ProcessObject::OnMessage(_message);
}

bool	RCSClient::Start()
{
	tcp_client_.Start();

	if (!Connect())
	{
		TRACE_ERROR("Failed to connect!");	
	}

	return	ProcessObject::Start();
}

bool	RCSClient::Stop(bool _wait)
{
	if (tcp_client_.IsConnected())
	{
		tcp_client_.Disconnect();
	}

	tcp_client_.Stop();

	return	ProcessObject::Stop(_wait);
}

bool	RCSClient::Connect()
{
	return	tcp_client_.Connect();

}

bool	RCSClient::Disconnect()
{
	return	tcp_client_.Disconnect();
}

bool	RCSClient::IsConnected()
{
	return	tcp_client_.IsConnected();
}

void	RCSClient::Preprocess()
{
	ProcessObject::Preprocess();
}

void	RCSClient::Process()
{
	ProcessObject::Process();
}

void	RCSClient::Postprocess()
{
	ProcessObject::Postprocess();
}

bool	RCSClient::AddGateway(JSONNode const& _properties)
{
	RCSMessage	request(MSG_TYPE_RCS_ADD);
	RCSMessage	response;

	request.AddGateway(_properties);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received or invalid.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		JSONNode	gateway;

		gateway = response.GetGateway(0);
		return	true;
	}

	return	false;
}

bool	RCSClient::DelGateway(std::string const& _id)
{
	RCSMessage	request(MSG_TYPE_RCS_DEL);
	RCSMessage	response;

	request.AddGatewayID(_id);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received or invalid.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		return	true;
	}

	return	false;
}

bool	RCSClient::GetGateway(std::string const& _id, JSONNode& _properties)
{
	RCSMessage	request(MSG_TYPE_RCS_GET);
	RCSMessage	response;

	request.AddGatewayID(_id);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		_properties = response.GetGateway(0);
		return	true;
	}

	return	false;
}

bool	RCSClient::GetGateway(std::list<std::string>& _fields, std::vector<JSONNode>& _vector)
{
	RCSMessage	request(MSG_TYPE_RCS_LIST);
	RCSMessage	response;
	
	JSONNode	gateway(JSON_NODE);

	for(std::list<std::string>::iterator it = _fields.begin() ; it != _fields.end() ; it++)
	{
		gateway.push_back(JSONNode((*it),""));
	}

	request.AddGateway(gateway);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		for(uint32_t i = 0 ; i < response.GetGatewayCount(); i++)
		{
			_vector.push_back(response.GetGateway(i));
		}
		return	true;
	}

	return	false;
}

bool	RCSClient::GetGateway(std::vector<JSONNode>& _vector)
{
	RCSMessage	request(MSG_TYPE_RCS_LIST);
	RCSMessage	response;

	request.AddGatewayID("");

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		for(uint32_t i = 0 ; i < response.GetGatewayCount(); i++)
		{
			_vector.push_back(response.GetGateway(i));
		}
		return	true;
	}

	return	false;
}

bool	RCSClient::SetGateway(JSONNode const& _properties)
{
	RCSMessage	request(MSG_TYPE_RCS_SET);
	RCSMessage	response;

	request.AddGateway(_properties);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		return	true;
	}

	return	false;
}

bool	RCSClient::AddDevice(JSONNode const& _properties)
{
	RCSMessage	request(MSG_TYPE_RCS_ADD);
	RCSMessage	response;

	request.AddDevice(_properties);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received or invalid.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		JSONNode	device;

		device = response.GetDevice(0);
		return	true;
	}

	return	false;
}

bool	RCSClient::DelDevice(std::string const& _id)
{
	RCSMessage	request(MSG_TYPE_RCS_DEL);
	RCSMessage	response;

	request.AddDeviceID(_id);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received or invalid.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		return	true;
	}

	return	false;
}

bool	RCSClient::GetDevice(std::string const& _id, JSONNode& _properties)
{
	RCSMessage	request(MSG_TYPE_RCS_GET);
	RCSMessage	response;

	request.AddDeviceID(_id);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		_properties = response.GetDevice(0);
		return	true;
	}

	return	false;
}

bool	RCSClient::GetDevice(std::list<std::string>& _fields, std::vector<JSONNode>& _vector)
{
	RCSMessage	request(MSG_TYPE_RCS_LIST);
	RCSMessage	response;
	
	JSONNode	device(JSON_NODE);

	for(std::list<std::string>::iterator it = _fields.begin() ; it != _fields.end() ; it++)
	{
		device.push_back(JSONNode((*it),""));
	}

	request.AddDevice(device);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		for(uint32_t i = 0 ; i < response.GetDeviceCount(); i++)
		{
			_vector.push_back(response.GetDevice(i));
		}
		return	true;
	}

	return	false;
}

bool	RCSClient::GetDevice(std::vector<JSONNode>& _vector)
{
	RCSMessage	request(MSG_TYPE_RCS_LIST);
	RCSMessage	response;

	request.AddDeviceID("");

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		for(uint32_t i = 0 ; i < response.GetDeviceCount(); i++)
		{
			_vector.push_back(response.GetDevice(i));
		}
		return	true;
	}

	return	false;
}

bool	RCSClient::SetDevice(JSONNode const& _properties)
{
	RCSMessage	request(MSG_TYPE_RCS_SET);
	RCSMessage	response;

	request.AddDevice(_properties);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		return	true;
	}

	return	false;
}

bool	RCSClient::AddEndpoint(JSONNode const& _properties)
{
	RCSMessage	request(MSG_TYPE_RCS_ADD);
	RCSMessage	response;

	request.AddEndpoint(_properties);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received or invalid.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		JSONNode	endpoint;

		endpoint = response.GetEndpoint(0);
		return	true;
	}

	return	false;
}

bool	RCSClient::DelEndpoint(std::string const& _id)
{
	RCSMessage	request(MSG_TYPE_RCS_DEL);
	RCSMessage	response;

	request.AddEndpointID(_id);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received or invalid.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		return	true;
	}

	return	false;
}

bool	RCSClient::GetEndpoint(std::string const& _id, JSONNode& _properties)
{
	RCSMessage	request(MSG_TYPE_RCS_GET);
	RCSMessage	response;

	request.AddEndpointID(_id);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		_properties = response.GetEndpoint(0);
		return	true;
	}

	return	false;
}

bool	RCSClient::GetEndpoint(std::list<std::string>& _fields, std::vector<JSONNode>& _vector)
{
	RCSMessage	request(MSG_TYPE_RCS_LIST);
	RCSMessage	response;
	
	JSONNode	endpoint(JSON_NODE);

	for(std::list<std::string>::iterator it = _fields.begin() ; it != _fields.end() ; it++)
	{
		endpoint.push_back(JSONNode((*it),""));
	}

	request.AddEndpoint(endpoint);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		for(uint32_t i = 0 ; i < response.GetEndpointCount(); i++)
		{
			_vector.push_back(response.GetEndpoint(i));
		}
		return	true;
	}

	return	false;
}

bool	RCSClient::GetEndpoint(std::vector<JSONNode>& _vector)
{
	RCSMessage	request(MSG_TYPE_RCS_LIST);
	RCSMessage	response;

	request.AddEndpointID("");

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		for(uint32_t i = 0 ; i < response.GetEndpointCount(); i++)
		{
			_vector.push_back(response.GetEndpoint(i));
		}
		return	true;
	}

	return	false;
}

bool	RCSClient::SetEndpoint(JSONNode const& _properties)
{
	RCSMessage	request(MSG_TYPE_RCS_SET);
	RCSMessage	response;

	request.AddEndpoint(_properties);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		return	true;
	}

	return	false;
}


bool	RCSClient::DelEPData(std::string const& _id, uint32_t _count)
{
	RCSMessage	request(MSG_TYPE_RCS_DEL);
	RCSMessage	response;

	JSONNode	data;
	data.push_back(JSONNode(TITLE_NAME_ID, _id));
	data.push_back(JSONNode(TITLE_NAME_COUNT, ToString(_count)));

	request.AddEPData(data);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		return	true;
	}

	return	false;
}

bool	RCSClient::DelEPData(std::string const& _id, time_t _start, time_t _end)
{
	RCSMessage	request(MSG_TYPE_RCS_DEL);
	RCSMessage	response;

	JSONNode	data;
	data.push_back(JSONNode(TITLE_NAME_ID, _id));
	data.push_back(JSONNode(TITLE_NAME_START_TIME, ToString(_start)));
	data.push_back(JSONNode(TITLE_NAME_END_TIME, ToString(_end)));

	request.AddEPData(data);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		return	true;
	}

	return	false;
}

bool	RCSClient::GetEPData(std::string const& _id, uint32_t _count, std::multimap<time_t, std::string>& _value_map)
{
	RCSMessage	request(MSG_TYPE_RCS_GET);
	RCSMessage	response;

	JSONNode	data;
	data.push_back(JSONNode(TITLE_NAME_ID, _id));
	data.push_back(JSONNode(TITLE_NAME_COUNT, ToString(_count)));

	request.AddEPData(data);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	try
	{
		if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
		{
			if (response.GetEPDataCount() > 0)
			{
				JSONNode	data = response.GetEPData(0);	

				JSONNode	value_node = JSONNodeGetValueNode(data);
				if (value_node.type() == JSON_NODE)
				{
				}
				else if (value_node.type() == JSON_ARRAY)
				{
					for(JSONNode::iterator it = value_node.begin(); it != value_node.end() ; it++)
					{
						time_t		time = JSONNodeGetTime(*it);
						std::string	value = JSONNodeGetValue(*it);

						_value_map.insert(_value_map.end(), std::pair<time_t, std::string>(time, value));
					}	
				}
			}
			return	true;
		}
	}
	catch(InvalidArgument& e)
	{
		TRACE_ERROR(e.what());
	}
	return	false;
}

bool	RCSClient::GetEPData(std::string const& _id, time_t _start, time_t _end, std::multimap<time_t, std::string>& _value_map)
{
	RCSMessage	request(MSG_TYPE_RCS_GET);
	RCSMessage	response;

	JSONNode	data;
	data.push_back(JSONNode(TITLE_NAME_ID, _id));
	data.push_back(JSONNode(TITLE_NAME_START_TIME, ToString(_start)));
	data.push_back(JSONNode(TITLE_NAME_END_TIME, ToString(_end)));

	request.AddEPData(data);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	try
	{
		if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
		{
			if (response.GetEPDataCount() > 0)
			{
				JSONNode	data = response.GetEPData(0);	

				JSONNode	value_node = JSONNodeGetValueNode(data);
				if (value_node.type() == JSON_NODE)
				{
				}
				else if (value_node.type() == JSON_ARRAY)
				{
					for(JSONNode::iterator it = value_node.begin(); it != value_node.end() ; it++)
					{
						time_t		time = JSONNodeGetTime(*it);
						std::string	value = JSONNodeGetValue(*it);

						_value_map.insert(_value_map.end(), std::pair<time_t, std::string>(time, value));
					}	
				}
			}
			return	true;
		}
	}
	catch(InvalidArgument& e)
	{
		TRACE_ERROR(e.what());
	}
	return	false;
}

bool	RCSClient::SetEPData(std::string const& _id, time_t _time_of_expire, std::string& _value )
{
	RCSMessage	request(MSG_TYPE_RCS_SET);
	RCSMessage	response;

	JSONNode	data;
	data.push_back(JSONNode(TITLE_NAME_ID, _id));
	data.push_back(JSONNode(TITLE_NAME_TIME_OF_EXPIRE, _time_of_expire));
	data.push_back(JSONNode(TITLE_NAME_VALUE, _value));

	request.AddEndpoint(data);

	if (!RemoteCall(request, response))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (response.GetMsgType() == MSG_TYPE_RCS_CONFIRM)	
	{
		return	true;
	}

	return	false;
}

bool	RCSClient::RemoteCall(JSONNode& _request, JSONNode& _response)
{
	if (!IsConnected())
	{
		TRACE_ERROR("Client is connected!");	
		return	false;
	}

	std::string	receive_buffer;

	if (!tcp_client_.RequestAndReply(_request.write(), receive_buffer))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (!libjson::is_valid(receive_buffer))
	{
		TRACE_ERROR("The remote call response is invalid.");	
		TRACE_ERROR("Payload : " << receive_buffer);
		return	false;
	}

	_response = libjson::parse(receive_buffer);

	return	true;
}

bool	RCSClient::RemoteCall(RCSMessage& _request, RCSMessage& _response)
{
	if (!IsConnected())
	{
		TRACE_ERROR("Client is connected!");	
		return	false;
	}

	_request.Make();

	std::string	receive_buffer;

	if (!tcp_client_.RequestAndReply(_request.GetPayload().write(), receive_buffer))
	{
		TRACE_ERROR("The response was not received.");	
		return	false;
	}

	if (!libjson::is_valid(receive_buffer))
	{
		TRACE_ERROR("The remote call response is invalid.");	
		TRACE_ERROR("Payload : " << receive_buffer);
		return	false;
	}

	_response = libjson::parse(receive_buffer);

	return	true;
}
