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
#include "rcs_server.h"
#include "rcs_session.h"
#include "trace.h"
#include "message.h"
#include "object_manager.h"
#include "endpoint_actuator.h"
#include "utils.h"
#include "json.h"

/////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////
RCSServer::RCSServer(ObjectManager* _manager)
: TCPServer(_manager)
{
	trace.SetClassName(GetClassName());
	enable_	= true;
	name_ 	= "rcs_server";
}

bool	RCSServer::Load(JSONNode const& _json)
{
	bool	ret_value = true;

	if ((_json.name() == TITLE_NAME_RCS_SERVER) || (_json.name().size() == 0))
	{
		if (_json.type() == JSON_NODE)
		{
			for(auto it = _json.begin(); it != _json.end() ; it++)
			{
				ret_value = Load(*it);
				if (!ret_value)
				{
					std::cout << "Invalid format" << std::endl;
				}
			}
		}
	}
	else
	{
		ret_value = TCPServer::Load(_json);
	}

	return	ret_value;
}

RCSServer::operator JSONNode() const
{
	JSONNode	root;
	
	root.push_back(JSONNode(TITLE_NAME_PORT, port_));
	root.push_back(JSONNode(TITLE_NAME_TIMEOUT, timeout_));
	root.push_back(JSONNode(TITLE_NAME_ENABLE, ((enable_)?"yes":"no")));

	JSONNode	trace_config = trace;

	if (trace_config.size() != 0)
	{
		trace_config.set_name(TITLE_NAME_TRACE);
		root.push_back(trace_config);
	}

	return	root;
}

bool	RCSServer::SetProperty(Property const& _property, Properties::Fields const& _fields)
{
	return	TCPServer::SetProperty(_property, _fields);
}

TCPSession*	RCSServer::CreateSession(int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout)
{
	RCSSession* rcs = new RCSSession(this, _socket, _addr_info, _timeout);

	manager_->Attach(rcs);
	rcs->SetTrace(true);
	rcs->Start();

	return	rcs;
}

bool	RCSServer::ServiceCall(RCSMessage& _request, RCSMessage& _response)
{
	bool	result = false;

	try
	{
		if (_request.GetMsgType() == MSG_TYPE_RCS_ADD)
		{
			result = Add(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_DEL)
		{
			result = Del(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_GET)
		{
			result = Get(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_SET)
		{
			result = Set(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_LIST)
		{
			result = List(_request, _response);	
		}
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
	}
	catch(InvalidArgument& e)
	{
		TRACE_ERROR(e.what());
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
	}

	return	result;	
}

bool	RCSServer::Add(RCSMessage& _request, RCSMessage& _response)
{
	JSONNode	payload  = _request.GetPayload();
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);

	for(auto it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			JSONNode	result(JSON_NODE);

			Gateway*	gateway = manager_->CreateGateway(*it);
			if (gateway == NULL)
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}
			else
			{
				gateway->GetProperties(result, PROPERTY_ALL);
			}

			response.AddGateway(result);	
		}
		else if (it->name() == TITLE_NAME_DEVICE)
		{
			JSONNode	result(JSON_NODE);

			Device*	device = manager_->CreateDevice(*it);
			if (device == NULL)
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}
			else
			{
				device->GetProperties(result, PROPERTY_ALL);
			}

			response.AddDevice(result);	
		}
		else if (it->name() == TITLE_NAME_ENDPOINT)
		{
			JSONNode	result(JSON_NODE);

			Endpoint*	endpoint = manager_->CreateEndpoint(*it);
			if (endpoint == NULL)
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}
			else
			{
				endpoint->GetProperties(result, PROPERTY_ALL);
			}

			response.AddEndpoint(result);	
		}
	}	

	_response = response;

	return	true;
}

bool	RCSServer::Del(RCSMessage& _request, RCSMessage& _response)
{
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);
	JSONNode	payload  = _request.GetPayload();

	for(auto it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			JSONNode	result(JSON_NODE);
			std::string id = JSONNodeGetID(*it);
			
			if (manager_->DestroyGateway(id))
			{
				result.push_back(JSONNode(TITLE_NAME_ID, id));	
			}
			else
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}

			response.AddGateway(result);
		}
		else if (it->name() == TITLE_NAME_DEVICE)
		{
			JSONNode	result(JSON_NODE);
			std::string id = JSONNodeGetID(*it);

			if (manager_->DestroyDevice(id))
			{
				result.push_back(JSONNode(TITLE_NAME_ID, id));	
			}
			else
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}

			response.AddDevice(result);
		}
		else if (it->name() == TITLE_NAME_ENDPOINT)
		{
			JSONNode	result(JSON_NODE);
			std::string id = JSONNodeGetID(*it);

			if (manager_->DestroyEndpoint(id))
			{
				result.push_back(JSONNode(TITLE_NAME_ID, id));	
			}
			else
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}

			response.AddEndpoint(result);
		}
		else if (it->name() == TITLE_NAME_DATA)
		{
			JSONNode	result;
			std::string	id;

			time_t	start_time;
			time_t	end_time;
			try
			{
				start_time = JSONNodeGetStartTime(*it, 0);
				end_time   = JSONNodeGetEndTime(*it, time_t(Date::GetCurrent()));

				id = JSONNodeGetID(*it);
				Endpoint*	endpoint = manager_->GetEndpoint(id);
				if (endpoint == NULL)
				{
					throw ObjectNotFound(id);
				}


				endpoint->DelDataForPeriod(Date(start_time), Date(end_time));
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_START_TIME, start_time));
				result.push_back(JSONNode(TITLE_NAME_END_TIME, end_time));
			}
			catch(ObjectNotFound& e)
			{
				TRACE_ERROR("Object[" << id << "] not found!");
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
			}
			catch(InvalidArgument& e)
			{
				TRACE_ERROR("Invalid message fromat!");
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
			}

			response.AddEPData(result);
		}
	}	

	_response = response;
	
	return	true;
}

bool	RCSServer::Set(RCSMessage& _request, RCSMessage& _response)
{
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);
	JSONNode	payload  = _request.GetPayload();

	for(auto it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			JSONNode	result;
			std::string	id;

			id = JSONNodeGetID(*it);
			Gateway*	gateway = manager_->GetGateway(id);
			if (gateway == NULL)
			{
				TRACE_ERROR("Object[" << id << "not found!");
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
			}
			else if (!gateway->SetProperties(*it))
			{
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
			}
			else
			{
				gateway->GetProperties(result, PROPERTY_ALL);
			}

			response.AddGateway(result);
		}
		else if (it->name() == TITLE_NAME_DEVICE)
		{
			JSONNode	result;
			std::string	id;

			id = JSONNodeGetID(*it);
			Device*	device = manager_->GetDevice(id);
			if (device == NULL)
			{
				TRACE_ERROR("Object[" << id << "not found!");
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
			}
			else if (!device->SetProperties(*it))
			{
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
			}
			else
			{
				device->GetProperties(result, PROPERTY_ALL);
			}

			response.AddDevice(result);
		}
		else if (it->name() == TITLE_NAME_ENDPOINT)
		{
			JSONNode	result;
			std::string	id;

			id = JSONNodeGetID(*it);
			Endpoint*	endpoint= manager_->GetEndpoint(id);
			if (endpoint == NULL)
			{
				TRACE_ERROR("Object[" << id << "not found!");
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
			}
			else if (!endpoint->SetProperties(*it))
			{
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
			}
			else
			{
				endpoint->GetProperties(result, PROPERTY_ALL);
			}

			response.AddEndpoint(result);
		}
		else if (it->name() == TITLE_NAME_DATA)
		{
			JSONNode	result;
			std::string	id;
			std::string	value;
			time_t	time_of_expire;

			try
			{
				id = JSONNodeGetID(*it);
				time_of_expire = JSONNodeGetTimeOfExpire(*it);
				value = JSONNodeGetValue(*it);

				Endpoint*	endpoint = manager_->GetEndpoint(id);
				if (endpoint == NULL)
				{
					TRACE_ERROR("Object[" << id << "not found!");
					result.push_back(JSONNode(TITLE_NAME_ID, id));
					result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
				}
				else
				{
					EndpointActuator* actuator = dynamic_cast<EndpointActuator*>(endpoint);
					if (actuator == NULL)
					{
						TRACE_ERROR("Endpoint[" << id << "] is not actuator!");
						result.push_back(JSONNode(TITLE_NAME_ID, id));
						result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_PERMISSION_DENY));
					}
					else
					{
						if (actuator->Set(value))
						{
							result.push_back(JSONNode(TITLE_NAME_ID, id));
							result.push_back(JSONNode(TITLE_NAME_VALUE, value));
						}
						else
						{
							TRACE_ERROR("Failed to set value!");
							result.push_back(JSONNode(TITLE_NAME_ID, id));
							result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
						}
					}
				}
			}
			catch(ObjectNotFound& e)
			{
				TRACE_ERROR("Property[" << e.what() << "] not found!");
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
			}

			response.AddEPData(result);
		}
	}

	_response = response;

	return	true;
}

bool	RCSServer::Get(RCSMessage& _request, RCSMessage& _response)
{
	JSONNode	payload  = _request.GetPayload();
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);

	for(auto it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			JSONNode	result;
			std::string	id = JSONNodeGetID(*it);

			Gateway*	gateway = manager_->GetGateway(id);
			if (gateway == NULL)
			{
				TRACE_ERROR("The gateway[" << id << "] not found.");
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
			}
			else
			{
				gateway->GetProperties(result, PROPERTY_ALL);
			}

			response.AddGateway(result);
		}
		else if (it->name() == TITLE_NAME_DEVICE)
		{
			JSONNode	result;
			std::string	id = JSONNodeGetID(*it);

			Device*	device = manager_->GetDevice(id);
			if (device == NULL)
			{
				TRACE_ERROR("The device[" << id << "] not found.");
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
			}
			else
			{
				device->GetProperties(result, PROPERTY_ALL);
			}

			response.AddDevice(result);
		}
		else if (it->name() == TITLE_NAME_ENDPOINT)
		{
			JSONNode	result;
			std::string	id = JSONNodeGetID(*it);

			Endpoint*	endpoint= manager_->GetEndpoint(id);
			if (endpoint == NULL)
			{
				TRACE_ERROR("The endpoint[" << id << "] not found.");
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
			}
			else
			{
				endpoint->GetProperties(result, PROPERTY_ALL);
			}

			response.AddEndpoint(result);
		}
	}

	_response = response;

	return	true;
}

bool	RCSServer::List(RCSMessage& _request, RCSMessage& _response)
{
	JSONNode	payload  = _request.GetPayload();
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);

	for(auto it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			if (manager_->GetGatewayCount() != 0)
			{
				std::list<Gateway*> list;

				manager_->GetGatewayList(list);

				for(auto gateway = list.begin(); gateway != list.end() ; gateway++)
				{
					JSONNode	result;

					(*gateway)->GetProperties(result, PROPERTY_ALL);

					response.AddGateway(result);
				}
			}
			else
			{
				response.AddGatewayNull();
			}
		}
		else if (it->name() == TITLE_NAME_DEVICE)
		{
			if (manager_->GetDeviceCount() != 0)
			{
				std::list<Device*>	list;

				manager_->GetDeviceList(list);

				for(auto device = list.begin(); device != list.end() ; device++)
				{
					JSONNode	result;

					(*device)->GetProperties(result, PROPERTY_ALL);

					response.AddDevice(result);
				}
			}
			else
			{
				response.AddDeviceNull();
			}
		}
		else if (it->name() == TITLE_NAME_ENDPOINT)
		{
			if (manager_->GetEndpointCount() != 0)
			{
				std::list<Endpoint*>	list;

				manager_->GetEndpointList(list);

				for(auto endpoint = list.begin(); endpoint != list.end() ; endpoint++)
				{
					JSONNode	result;

					(*endpoint)->GetProperties(result, PROPERTY_ALL);

					response.AddEndpoint(result);
				}
			}
			else
			{
				response.AddEndpointNull();
			}
		}
	}

	_response = response;

	return	true;
}

bool	RCSServer::Confirm(RCSMessage& _reply, std::string& _req_type)
{
	bool		ret_value = false;
	JSONNode	reply_payload  = _reply.GetPayload();

	try
	{
		for(auto it = reply_payload.begin(); it != reply_payload.end() ; it++)
		{
			if (it->name() == TITLE_NAME_GATEWAY)
			{
				ConfirmGateway(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_DEVICE)
			{
				ConfirmDevice(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_ENDPOINT)
			{
				ConfirmEndpoint(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_DATA)
			{
				ConfirmData(*it, _req_type);
			}
		}	
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
	}
	catch(InvalidArgument& e)
	{
		TRACE_ERROR(e.what());
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
	}

	return	ret_value;
}

bool	RCSServer::Error(RCSMessage& _reply)
{
	bool		ret_value = false;
	JSONNode	reply_payload  = _reply.GetPayload();

	try
	{
		for(auto it = reply_payload.begin(); it != reply_payload.end() ; it++)
		{
			if (it->name() == TITLE_NAME_GATEWAY)
			{
				TRACE_ERROR("Gateway Error");
			}
			else if (it->name() == TITLE_NAME_DEVICE)
			{
				TRACE_ERROR("Device Error");
			}
			else if (it->name() == TITLE_NAME_ENDPOINT)
			{
				TRACE_ERROR("Endpoint Error");
			}
			else if (it->name() == TITLE_NAME_DATA)
			{
				TRACE_ERROR("Data Error");
			}
		}	
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
	}
	catch(InvalidArgument& e)
	{
		TRACE_ERROR(e.what());
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
	}

	return	ret_value;
}

bool	RCSServer::Error(RCSMessage& _reply, std::string& _req_type)
{
	bool		ret_value = false;
	JSONNode	reply_payload  = _reply.GetPayload();

	try
	{
		for(auto it = reply_payload.begin(); it != reply_payload.end() ; it++)
		{
			if (it->name() == TITLE_NAME_GATEWAY)
			{
				ConfirmGateway(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_DEVICE)
			{
				ConfirmDevice(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_ENDPOINT)
			{
				ConfirmEndpoint(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_DATA)
			{
				ConfirmData(*it, _req_type);
			}
		}	
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
	}
	catch(InvalidArgument& e)
	{
		TRACE_ERROR(e.what());
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
	}

	return	ret_value;
}

bool	RCSServer::ConfirmGateway(JSONNode& _node, std::string& _req_type)
{
	if (_node.type() == JSON_NODE)
	{
		std::string	id;
		if (!GetValue(_node, TITLE_NAME_ID, id))
		{
			throw InvalidArgument("Invalid _message fromat!");
		}

		if (_req_type == MSG_TYPE_RCS_ADD)
		{
			Gateway*	gateway = manager_->GetGateway(id);
			if (gateway == NULL)
			{
				throw ObjectNotFound(id);
			}

			gateway->SetRegistered(true);
			gateway->SetProperties(_node, PROPERTY_ADD_CONFIRM);
			TRACE_INFO("The gateway[" << id << "] registeration confirmed.");
		}
		else if (_req_type == MSG_TYPE_RCS_DEL)
		{
			TRACE_INFO("The gateway[" << id << "] updated.");
		}
		else if (_req_type == MSG_TYPE_RCS_GET)
		{
			TRACE_INFO("The gateway[" << id << "] updated.");
		}
		else if (_req_type == MSG_TYPE_RCS_SET)
		{
			TRACE_INFO("The gateway[" << id << "] updated.");
		}
	}
	else if (_node.type() == JSON_ARRAY)
	{
		for(auto it = _node.begin() ; it != _node.end() ; it++)
		{
			ConfirmGateway(*it, _req_type);
		}
	}
	else
	{
		throw InvalidArgument("Invalid _message fromat!");
	}
}

bool	RCSServer::ConfirmDevice(JSONNode& _node, std::string& _req_type)
{
	if (_node.type() == JSON_NODE)
	{
		std::string	id;
		if (!GetValue(_node, TITLE_NAME_ID, id))
		{
			TRACE_ERROR("Invalid _message fromat!");
			return	false;
		}

		if (_req_type == MSG_TYPE_RCS_ADD)
		{
			Device*	device = manager_->GetDevice(id);
			if (device == NULL)
			{
				TRACE_ERROR("Object[" << id << "] not found!");
				return	false;
			}

			device->SetRegistered(true);
			device->SetProperties(_node);
			TRACE_INFO("The device[" << id << "] registeration confirmed.");
		}
		else if (_req_type == MSG_TYPE_RCS_DEL)
		{
			TRACE_INFO("The device[" << id << "] updated.");
		}
		else if (_req_type == MSG_TYPE_RCS_GET)
		{
			TRACE_INFO("The device[" << id << "] updated.");
		}
		else if (_req_type == MSG_TYPE_RCS_SET)
		{
			TRACE_INFO("The device[" << id << "] updated.");
		}
	}
	else if (_node.type() == JSON_ARRAY)
	{
		for(auto it = _node.begin() ; it != _node.end() ; it++)
		{
			ConfirmDevice(*it, _req_type);
		}
	}
	else
	{
		TRACE_ERROR("Invalid _message fromat!");
		return	false;
	}

	return	true;
}

bool	RCSServer::ConfirmEndpoint(JSONNode& _node, std::string& _req_type)
{
	if (_node.type() == JSON_NODE)
	{
		std::string	id;
		if (!GetValue(_node, TITLE_NAME_ID, id))
		{
			TRACE_ERROR("Invalid _message fromat!");
			return	false;
		}

		if (_req_type == MSG_TYPE_RCS_ADD)
		{
			Endpoint*	endpoint = manager_->GetEndpoint(id);
			if (endpoint == NULL)
			{
				TRACE_ERROR("Object[" << id << "] not found!");
				return	false;
			}

			endpoint->SetRegistered(true);
			endpoint->SetProperties(_node);
			TRACE_INFO("The endpoint[" << id << "] registeration confirmed.");
		}
		else if (_req_type == MSG_TYPE_RCS_DEL)
		{
			TRACE_INFO("The endpoint[" << id << "] updated.");
		}
		else if (_req_type == MSG_TYPE_RCS_GET)
		{
			TRACE_INFO("The endpoint[" << id << "] updated.");
		}
		else if (_req_type == MSG_TYPE_RCS_SET)
		{
			TRACE_INFO("The endpoint[" << id << "] updated.");
		}
	}
	else if (_node.type() == JSON_ARRAY)
	{
		for(auto it = _node.begin() ; it != _node.end() ; it++)
		{
			ConfirmEndpoint(*it, _req_type);
		}
	}
	else
	{
		TRACE_ERROR("Invalid _message fromat!");
		return	false;
	}

	return	true;
}


bool	RCSServer::ConfirmData(JSONNode& _node, std::string& _req_type)
{
	std::string	id;

	if (!GetValue(_node, TITLE_NAME_ID, id))
	{
		TRACE_ERROR("Invalid _message fromat!");
		return	false;
	}

	if (_req_type == MSG_TYPE_RCS_REPORT)
	{
		time_t		last_time;
		Endpoint*	endpoint = manager_->GetEndpoint(id);
		if (endpoint == NULL)
		{
			TRACE_ERROR("Object[" << id << "] not found!");
			return	false;
		}

		if (!GetValue(_node, TITLE_NAME_LAST_TIME, last_time))
		{
			TRACE_ERROR("Invalid _message fromat!");
			TRACE_ERROR("Node : " << _node.write_formatted());
			return	false;
		}

		endpoint->SetLastReportTime(Date(last_time));
		TRACE_INFO("The data for the endpoint[" << id << "] has been transferred.");
	}

	return	true;
}


