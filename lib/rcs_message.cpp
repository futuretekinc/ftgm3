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

	gateway_vector_	=	_message.gateway_vector_;
	device_vector_	=	_message.device_vector_;
	endpoint_vector_=	_message.endpoint_vector_;
	epdata_vector_	=	_message.epdata_vector_;
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
			gateway_vector_.push_back(*it);
		}
		else if (it->type() == JSON_ARRAY)
		{
			for(JSONNode::const_iterator item = it->begin() ; item != it->end() ; item++)
			{
				gateway_vector_.push_back(*item);	
			}
		}
		else
		{
			throw InvalidArgument("RCS message gateway is invalid!");
		}
	}

	it = _payload.find(TITLE_NAME_DEVICE);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			device_vector_.push_back(*it);
		}
		else if (it->type() == JSON_ARRAY)
		{
			for(JSONNode::const_iterator item = it->begin() ; item != it->end() ; item++)
			{
				device_vector_.push_back(*item);	
			}
		}
		else
		{
			throw InvalidArgument("RCS message device is invalid!");
		}
	}

	it = _payload.find(TITLE_NAME_ENDPOINT);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			endpoint_vector_.push_back(*it);
		}
		else if (it->type() == JSON_ARRAY)
		{
			for(JSONNode::const_iterator item = it->begin() ; item != it->end() ; item++)
			{
				endpoint_vector_.push_back(*item);	
			}
		}
		else
		{
			throw InvalidArgument("RCS message endpoint is invalid!");
		}
	}

	it = _payload.find(TITLE_NAME_DATA);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			epdata_vector_.push_back(*it);
		}
		else if (it->type() == JSON_ARRAY)
		{
			for(JSONNode::const_iterator item = it->begin() ; item != it->end() ; item++)
			{
				epdata_vector_.push_back(*item);	
			}
		}
		else
		{
			throw InvalidArgument("RCS message endpoint is invalid!");
		}
	}

	Make();
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
			if (gateway_vector_.size() == 1)
			{
				JSONNode	node = gateway_vector_[0];

				node.set_name(TITLE_NAME_GATEWAY);

				payload_.push_back(node);
			}
			else if (gateway_vector_.size() > 1)
			{
				JSONNode	array(JSON_ARRAY);

				for(std::vector<JSONNode>::iterator it = gateway_vector_.begin() ; it != gateway_vector_.end() ; it++)
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
			if (device_vector_.size() == 1)
			{
				JSONNode	node = device_vector_[0];
				node.set_name(TITLE_NAME_DEVICE);

				payload_.push_back(node);
			}
			else if (device_vector_.size() > 1)
			{
				JSONNode	array(JSON_ARRAY);

				for(std::vector<JSONNode>::iterator it = device_vector_.begin() ; it != device_vector_.end() ; it++)
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
			if (endpoint_vector_.size() == 1)
			{
				JSONNode	node = endpoint_vector_[0];
				node.set_name(TITLE_NAME_ENDPOINT);

				payload_.push_back(node);
			}
			else if (endpoint_vector_.size() > 1)
			{
				JSONNode	array(JSON_ARRAY);

				for(std::vector<JSONNode>::iterator it = endpoint_vector_.begin() ; it != endpoint_vector_.end() ; it++)
				{
					array.push_back(*it);
				}

				array.set_name(TITLE_NAME_ENDPOINT);

				payload_.push_back(array);
			}
		}

		if (epdata_vector_.size() == 1)
		{
			JSONNode	node = epdata_vector_[0];
			node.set_name(TITLE_NAME_DATA);

			payload_.push_back(node);
		}
		else if (epdata_vector_.size() > 1)
		{
			JSONNode	array(JSON_ARRAY);

			for(std::vector<JSONNode>::iterator it = epdata_vector_.begin(); it != epdata_vector_.end() ; it++)
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
	gateway_vector_.push_back(properties);

	return	true;
}

bool	RCSMessage::AddGatewayID(std::string const& _id)
{
	JSONNode	node;

	node.push_back(JSONNode(TITLE_NAME_ID, _id));

	gateway_vector_.push_back(node);

	return	true;
}

bool	RCSMessage::AddGatewayNull()
{
	gateway_vector_.clear();

	return	true;
}

bool	RCSMessage::AddGatewayFields(Fields _fields)
{
	gateway_fields_ = _fields;

	return	true;
}

bool	RCSMessage::AddDevice(JSONNode const& properties)
{
	device_vector_.push_back(properties);

	return	true;
}

bool	RCSMessage::AddDeviceID(std::string const& _id)
{
	JSONNode	node;

	node.push_back(JSONNode(TITLE_NAME_ID, _id));

	device_vector_.push_back(node);

	return	true;
}

bool	RCSMessage::AddDeviceNull()
{
	device_vector_.clear();

	return	true;
}

bool	RCSMessage::AddDeviceFields(Fields _fields)
{
	device_fields_ = _fields;

	return	true;
}
bool	RCSMessage::AddEndpoint(JSONNode const& properties)
{
	endpoint_vector_.push_back(properties);

	return	true;
}

bool	RCSMessage::AddEndpointID(std::string const& _id)
{
	JSONNode	node;

	node.push_back(JSONNode(TITLE_NAME_ID, _id));

	endpoint_vector_.push_back(node);

	return	true;
}

bool	RCSMessage::AddEndpointNull()
{
	endpoint_vector_.clear();

	return	true;
}

bool	RCSMessage::AddEndpointFields(Fields _fields)
{
	endpoint_fields_ = _fields;

	return	true;
}

bool	RCSMessage::AddEPData(JSONNode const& properties)
{
	epdata_vector_.push_back(properties);

	return	true;
}


uint32_t	RCSMessage::GetGatewayCount()
{
	return	gateway_vector_.size();
}

JSONNode	RCSMessage::GetGateway(uint32_t index)
{
	return	gateway_vector_[index];
}

uint32_t	RCSMessage::GetDeviceCount()
{
	return	device_vector_.size();
}

JSONNode	RCSMessage::GetDevice(uint32_t index)
{
	return	device_vector_[index];
}

uint32_t	RCSMessage::GetEndpointCount()
{
	return	endpoint_vector_.size();
}

JSONNode	RCSMessage::GetEndpoint(uint32_t index)
{
	return	endpoint_vector_[index];
}

uint32_t	RCSMessage::GetEPDataCount()
{
	return	epdata_vector_.size();
}

JSONNode	RCSMessage::GetEPData(uint32_t index)
{
	return	epdata_vector_[index];
}

