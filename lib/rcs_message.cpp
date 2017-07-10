#include <list>
#include <string>
#include <iomanip>
#include "defined.h"
#include "exception.h"
#include "endpoint.h"
#include "server_linker.h"
#include "object_manager.h"
#include "trace.h"
#include "time2.h"
#include "sha1.h"
#include "md5.h"
#include "sha256.h"
#include "utils.h"
#include "property.h"
#include "json_utils.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class RCSMessage
/////////////////////////////////////////////////////////////////////////////////////////////
RCSMessage::RCSMessage(RCSMessage const& _message)
{
	msg_id_			=	_message.msg_id_;
	msg_type_		=	_message.msg_type_;
	secret_key_		=	_message.secret_key_;
	secret_code_	=	_message.secret_code_;
	req_id_			=	_message.req_id_;
	time_			=	_message.time_;
	payload_		=	_message.payload_;

	gateway_fields_	=	_message.gateway_fields_;
	device_fields_	=	_message.device_fields_;
	endpoint_fields_=	_message.endpoint_fields_;

	gateway_is_null_=	_message.gateway_is_null_;
	device_is_null_	=	_message.device_is_null_;
	endpoint_is_null_=	_message.endpoint_is_null_;

	for(std::list<JSONNode>::const_iterator it = _message.gateway_list_.begin() ; it != _message.gateway_list_.end() ; it ++)
	{
		gateway_list_.push_back(it->duplicate());
	}

	for(std::list<JSONNode>::const_iterator it = _message.device_list_.begin() ; it != _message.device_list_.end() ; it ++)
	{
		device_list_.push_back(it->duplicate());
	}

	for(std::list<JSONNode>::const_iterator it = _message.endpoint_list_.begin() ; it != _message.endpoint_list_.end() ; it ++)
	{
		endpoint_list_.push_back(it->duplicate());
	}

	for(std::list<JSONNode>::const_iterator it = _message.epdata_list_.begin() ; it != _message.epdata_list_.end() ; it ++)
	{
		epdata_list_.push_back(it->duplicate());
	}
}

RCSMessage::RCSMessage(std::string const& _msg_type)
:	msg_type_(_msg_type), gateway_is_null_(false), device_is_null_(false), endpoint_is_null_(false)
{
	time_ = Date::GetCurrent();

	msg_id_ = ToString(time_.GetMicroSecond());
}

RCSMessage::RCSMessage(JSONNode const& _payload)
:	gateway_is_null_(false), device_is_null_(false), endpoint_is_null_(false)
{
	JSONNode	node;

	if (!GetValue(_payload, TITLE_NAME_MSG_ID, msg_id_, false))
	{
		TRACE_ERROR("RCS message id is invalid!");
		TRACE_ERROR("Paylod : " << _payload.write_formatted());
		throw InvalidArgument("RCS message id is invalid!");
	}

	if (!GetValue(_payload, TITLE_NAME_MSG_TYPE, msg_type_, false))
	{
		throw InvalidArgument("RCS message type is invalid!");
	}

	if (!GetValue(_payload, TITLE_NAME_TIME, time_))
	{
		throw InvalidArgument("RCS message time is invalid!");
	}

	GetValue(_payload, TITLE_NAME_SECRET, secret_code_, true);
	GetValue(_payload, TITLE_NAME_REQ_ID, req_id_, true);

	JSONNode::const_iterator it = _payload.find(TITLE_NAME_GATEWAY);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			gateway_list_.push_back(*it);
		}
		else if (it->type() == JSON_ARRAY)
		{
			for(JSONNode::const_iterator item = it->begin() ; item != it->end() ; item++)
			{
				gateway_list_.push_back(*item);	
			}
		}
		else
		{
			throw InvalidArgument("RCS message gateway is invalid!");
		}
	}
	gateway_fields_ = JSONNodeGet(_payload, TITLE_NAME_GATEWAY_FIELDS, PROPERTY_STATIC_FLAG);

	it = _payload.find(TITLE_NAME_DEVICE);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			device_list_.push_back(*it);
		}
		else if (it->type() == JSON_ARRAY)
		{
			for(JSONNode::const_iterator item = it->begin() ; item != it->end() ; item++)
			{
				device_list_.push_back(*item);	
			}
		}
		else
		{
			throw InvalidArgument("RCS message device is invalid!");
		}
	}
	device_fields_ = JSONNodeGet(_payload, TITLE_NAME_DEVICE_FIELDS, PROPERTY_STATIC_FLAG);

	it = _payload.find(TITLE_NAME_ENDPOINT);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			endpoint_list_.push_back(*it);
		}
		else if (it->type() == JSON_ARRAY)
		{
			for(JSONNode::const_iterator item = it->begin() ; item != it->end() ; item++)
			{
				endpoint_list_.push_back(*item);	
			}
		}
		else
		{
			throw InvalidArgument("RCS message endpoint is invalid!");
		}
	}

	uint32_t	value = JSONNodeGet(_payload, TITLE_NAME_ENDPOINT_FIELDS, PROPERTY_STATIC_FLAG);

	endpoint_fields_ = value;

	it = _payload.find(TITLE_NAME_DATA);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			epdata_list_.push_back(*it);
		}
		else if (it->type() == JSON_ARRAY)
		{
			for(JSONNode::const_iterator item = it->begin() ; item != it->end() ; item++)
			{
				epdata_list_.push_back(*item);	
			}
		}
		else
		{
			throw InvalidArgument("RCS message endpoint is invalid!");
		}
	}

	Make();
}

RCSMessage::~RCSMessage()
{
}

bool	RCSMessage::SetMsgType(std::string const& _type)
{
	msg_type_ = _type;

	return	true;
}

bool	RCSMessage::SetReqID(std::string const& _req_id)
{
	req_id_ = _req_id;

	return	true;
}

JSONNode& RCSMessage::GetPayload() 
{
	Make();

	return	payload_;
}

bool	RCSMessage::Make()
{
	if (payload_.size() == 0)
	{
		secret_code_ = md5(secret_key_ + msg_id_);

		payload_.push_back(JSONNode(TITLE_NAME_MSG_ID, msg_id_));
		payload_.push_back(JSONNode(TITLE_NAME_MSG_TYPE, msg_type_));
		payload_.push_back(JSONNode(TITLE_NAME_SECRET, secret_code_));
		payload_.push_back(JSONNode(TITLE_NAME_TIME, ToString(time_t(time_))));
		if (req_id_.size() != 0)
		{
			payload_.push_back(JSONNode(TITLE_NAME_REQ_ID, req_id_));
		}

		if (uint32_t(gateway_fields_) != 0)
		{
			payload_.push_back(JSONNode(TITLE_NAME_GATEWAY_FIELDS, uint32_t(gateway_fields_)));	
		}

		if (gateway_is_null_)
		{
			JSONNode	node(JSON_NODE);

			node.set_name(TITLE_NAME_GATEWAY);

			payload_.push_back(node);
		}
		else
		{
			if (gateway_list_.size() == 1)
			{
				JSONNode	node = gateway_list_.front();

				node.set_name(TITLE_NAME_GATEWAY);

				payload_.push_back(node);
			}
			else if (gateway_list_.size() > 1)
			{
				JSONNode	array(JSON_ARRAY);

				for(std::list<JSONNode>::iterator it = gateway_list_.begin() ; it != gateway_list_.end() ; it++)
				{
					JSONNode	node;

					array.push_back(*it);
				}

				array.set_name(TITLE_NAME_GATEWAY);

				payload_.push_back(array);
			}
		}

		if (uint32_t(device_fields_) != 0)
		{
			payload_.push_back(JSONNode(TITLE_NAME_DEVICE_FIELDS, uint32_t(device_fields_)));	
		}

		if (device_is_null_)
		{
			JSONNode	node(JSON_NODE);

			node.set_name(TITLE_NAME_DEVICE);

			payload_.push_back(node);
		}
		else
		{
			if (device_list_.size() == 1)
			{
				JSONNode	node = device_list_.front();
				node.set_name(TITLE_NAME_DEVICE);

				payload_.push_back(node);
			}
			else if (device_list_.size() > 1)
			{
				JSONNode	array(JSON_ARRAY);

				for(std::list<JSONNode>::iterator it = device_list_.begin() ; it != device_list_.end() ; it++)
				{
					array.push_back(*it);
				}

				array.set_name(TITLE_NAME_DEVICE);

				payload_.push_back(array);
			}
		}

		if (uint32_t(endpoint_fields_) != 0)
		{
			payload_.push_back(JSONNode(TITLE_NAME_ENDPOINT_FIELDS, uint32_t(endpoint_fields_)));	
		}

		if (endpoint_is_null_)
		{
			JSONNode	node(JSON_NODE);

			node.set_name(TITLE_NAME_ENDPOINT);

			payload_.push_back(node);
		}
		else
		{
			if (endpoint_list_.size() == 1)
			{
				JSONNode	node = endpoint_list_.front();
				node.set_name(TITLE_NAME_ENDPOINT);

				payload_.push_back(node);
			}
			else if (endpoint_list_.size() > 1)
			{
				JSONNode	array(JSON_ARRAY);

				for(std::list<JSONNode>::iterator it = endpoint_list_.begin() ; it != endpoint_list_.end() ; it++)
				{
					array.push_back(*it);
				}

				array.set_name(TITLE_NAME_ENDPOINT);

				payload_.push_back(array);
			}
		}

		if (epdata_list_.size() == 1)
		{
			JSONNode	node = epdata_list_.front();
			node.set_name(TITLE_NAME_DATA);

			payload_.push_back(node);
		}
		else if (epdata_list_.size() > 1)
		{
			JSONNode	array(JSON_ARRAY);

			for(std::list<JSONNode>::iterator it = epdata_list_.begin(); it != epdata_list_.end() ; it++)
			{
				array.push_back(*it);
			}

			array.set_name(TITLE_NAME_DATA);

			payload_.push_back(array);
		}
	}
	return	true;
}

bool	RCSMessage::AddGateway(JSONNode const& properties)
{
	gateway_list_.push_back(properties);

	return	true;
}

bool	RCSMessage::AddGatewayID(std::string const& _id)
{
	JSONNode	node;

	node.push_back(JSONNode(TITLE_NAME_ID, _id));

	gateway_list_.push_back(node);

	return	true;
}

bool	RCSMessage::AddGatewayNull()
{
	gateway_list_.clear();

	return	true;
}

bool	RCSMessage::AddGatewayFields(Fields _fields)
{
	gateway_fields_ = _fields;

	return	true;
}

bool	RCSMessage::AddDevice(JSONNode const& properties)
{
	device_list_.push_back(properties);

	return	true;
}

bool	RCSMessage::AddDeviceID(std::string const& _id)
{
	JSONNode	node;

	node.push_back(JSONNode(TITLE_NAME_ID, _id));

	device_list_.push_back(node);

	return	true;
}

bool	RCSMessage::AddDeviceNull()
{
	device_list_.clear();

	return	true;
}

bool	RCSMessage::AddDeviceFields(Fields _fields)
{
	device_fields_ = _fields;

	return	true;
}
bool	RCSMessage::AddEndpoint(JSONNode const& properties)
{
	endpoint_list_.push_back(properties);

	return	true;
}

bool	RCSMessage::AddEndpointID(std::string const& _id)
{
	JSONNode	node;

	node.push_back(JSONNode(TITLE_NAME_ID, _id));

	endpoint_list_.push_back(node);

	return	true;
}

bool	RCSMessage::AddEndpointNull()
{
	endpoint_list_.clear();

	return	true;
}

bool	RCSMessage::AddEndpointFields(Fields _fields)
{
	endpoint_fields_ = _fields;

	return	true;
}

bool	RCSMessage::AddEPData(JSONNode const& properties)
{
	epdata_list_.push_back(properties);

	return	true;
}


uint32_t	RCSMessage::GetGatewayCount()
{
	return	gateway_list_.size();
}

JSONNode	RCSMessage::GetGateway(uint32_t index)
{
	for(std::list<JSONNode>::iterator it = gateway_list_.begin() ; it != gateway_list_.end() ; it++)
	{
		if (index == 0)
		{
			return	*it;	
		}

		index --;
	}

	return	JSONNode();
}

uint32_t	RCSMessage::GetDeviceCount()
{
	return	device_list_.size();
}

JSONNode	RCSMessage::GetDevice(uint32_t index)
{
	for(std::list<JSONNode>::iterator it = device_list_.begin() ; it != device_list_.end() ; it++)
	{
		if (index == 0)
		{
			return	*it;	
		}

		index --;
	}

	return	JSONNode();
}

uint32_t	RCSMessage::GetEndpointCount()
{
	return	endpoint_list_.size();
}

JSONNode	RCSMessage::GetEndpoint(uint32_t index)
{
	for(std::list<JSONNode>::iterator it = endpoint_list_.begin() ; it != endpoint_list_.end() ; it++)
	{
		if (index == 0)
		{
			return	*it;	
		}

		index --;
	}

	return	JSONNode();
}

uint32_t	RCSMessage::GetEPDataCount()
{
	return	epdata_list_.size();
}

JSONNode	RCSMessage::GetEPData(uint32_t index)
{
	for(std::list<JSONNode>::iterator it = epdata_list_.begin() ; it != epdata_list_.end() ; it++)
	{
		if (index == 0)
		{
			return	*it;	
		}

		index --;
	}

	return	JSONNode();
}

