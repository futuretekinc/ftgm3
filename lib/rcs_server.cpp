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
#include "exception.h"
#include "property.h"

/////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////
RCSServer::RCSServer(ObjectManager* _manager)
: TCPServer(_manager), max_data_get_count_(DEFAULT_CONST_MAX_DATA_GET_COUNT)
{
	trace.SetClassName(GetClassName());
	enable_	= true;
	name_ 	= "rcs_server";
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

bool	RCSServer::SetProperty(JSONNode const& _property, bool _check)
{
	return	TCPServer::SetProperty(_property, _check);
}

TCPSession*	RCSServer::CreateSession(int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout)
{
	RCSSession* rcs = new RCSSession(this, _socket, _addr_info, _timeout);

	manager_->Attach(rcs);
	rcs->SetTrace(true);
	if (!rcs->Start(10))
	{
		
		TRACE_ERROR("RCS server start initialization timeout!");	
		delete rcs;
		rcs = NULL;
	}

	return	rcs;
}

bool	RCSServer::ServiceCall(RCSMessage& _request, RCSMessage& _response)
{
	bool	ret_value = false;

	try
	{
		if (_request.GetMsgType() == MSG_TYPE_RCS_ADD)
		{
			ret_value = Add(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_DEL)
		{
			ret_value = Del(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_GET)
		{
			ret_value = Get(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_SET)
		{
			ret_value = Set(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_START)
		{
			ret_value = CmdStart(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_STOP)
		{
			ret_value = CmdStop(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_LIST)
		{
			ret_value = List(_request, _response);	
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

bool	RCSServer::Add(RCSMessage& _request, RCSMessage& _response)
{
	JSONNode	payload  = _request.GetPayload();
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);

	for(JSONNode::iterator it = payload.begin(); it != payload.end() ; it++)
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
				gateway->GetProperties(result);
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
				device->GetProperties(result);
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
				endpoint->GetProperties(result);
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

	for(JSONNode::iterator it = payload.begin(); it != payload.end() ; it++)
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

	response.SetReqID(_request.GetMsgID());

	for(JSONNode::iterator it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			if (it->type() == JSON_NODE)
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
				else 
				{
					if (!gateway->SetProperties(*it, false, false))
					{
						result.push_back(JSONNode(TITLE_NAME_ID, id));
						result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
					}
					else
					{
						gateway->SetProperties(*it, false, false);

						Fields	fields;

						for(JSONNode::iterator it2 = it->begin(); it2 != it->end() ; it2++)
						{
							fields.Set(it2->name());
						}

						gateway->GetProperties(result, fields);
						result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OK));
					}
				}

				response.AddGateway(result);
			}
			else if (it->type() == JSON_ARRAY)
			{
				for (JSONNode::iterator it2 = it->begin() ; it2 != it->end() ; it2++)
				{
					JSONNode	result;
					std::string	id;

					id = JSONNodeGetID(*it2);
					Gateway*	gateway = manager_->GetGateway(id);
					if (gateway == NULL)
					{
						TRACE_ERROR("Object[" << id << "not found!");
						result.push_back(JSONNode(TITLE_NAME_ID, id));
						result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
					}
					else
					{		
						if (!gateway->SetProperties(*it2, false, false))
						{
							result.push_back(JSONNode(TITLE_NAME_ID, id));
							result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
						}
						else
						{
							gateway->SetProperties(*it2, false, false);

							Fields	fields;

							for(JSONNode::iterator it3 = it2->begin(); it3 != it2->end() ; it3++)
							{
								fields.Set(it3->name());
							}

							gateway->GetProperties(result, fields);
							result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OK));
						}
					}

					response.AddGateway(result);
				}
			}
		}
		else if (it->name() == TITLE_NAME_DEVICE)
		{
			if (it->type() == JSON_NODE)
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
				else
				{
					if (!device->SetProperties(*it, false, false))
					{
						result.push_back(JSONNode(TITLE_NAME_ID, id));
						result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
					}
					else
					{
						device->SetProperties(*it, false, false);

						Fields	fields;

						for(JSONNode::iterator it2 = it->begin(); it2 != it->end() ; it2++)
						{
							fields.Set(it2->name());
						}

						device->GetProperties(result, fields);
					}
				}

				response.AddDevice(result);
			}
			else if (it->type() == JSON_ARRAY)
			{
				for(JSONNode::iterator it2 = it->begin() ; it2 != it->end() ; it2++)
				{
					JSONNode	result;
					std::string	id;

					id = JSONNodeGetID(*it2);
					Device*	device = manager_->GetDevice(id);
					if (device == NULL)
					{
						TRACE_ERROR("Object[" << id << "not found!");
						result.push_back(JSONNode(TITLE_NAME_ID, id));
						result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
					}
					else 
					{
						if (!device->SetProperties(*it2, false, false))
						{
							result.push_back(JSONNode(TITLE_NAME_ID, id));
							result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
						}
						else
						{
							device->SetProperties(*it2, false, false);

							Fields	fields;

							for(JSONNode::iterator it3 = it2->begin(); it3 != it2->end() ; it3++)
							{
								fields.Set(it3->name());
							}

							device->GetProperties(result, fields);
						}
					}

					response.AddDevice(result);
				}
			}
		}
		else if (it->name() == TITLE_NAME_ENDPOINT)
		{
			if (it->type() == JSON_NODE)
			{
				JSONNode	result;
				std::string	id;

				id = JSONNodeGetID(*it);

				result.push_back(JSONNode(TITLE_NAME_ID, id));

				Endpoint*	endpoint= manager_->GetEndpoint(id);
				if (endpoint == NULL)
				{
					TRACE_ERROR("Object[" << id << "not found!");
					result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
				}
				else
				{
					if (JSONNodeIsExistField(*it, TITLE_NAME_VALUE))
					{
						try
						{
							std::string	value = JSONNodeGetValue(*it);
							time_t	expire_time	= JSONNodeGetTimeOfExpire(*it);
							time_t	current_time= time_t(Date::GetCurrent());	

							if (current_time > expire_time)
							{
								result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_TIME_EXPIRED));
							}
							else
							{
								EndpointActuator*	actuator = dynamic_cast<EndpointActuator*>(endpoint);

								if (actuator == NULL)
								{
									result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_NOT_ACTUATOR));
								}
								else
								{
									if (actuator->SetValue(value))
									{
										result.push_back(JSONNode(TITLE_NAME_VALUE, actuator->GetValue()));
										result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OK));
									}
									else
									{
										result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_NOT_ACTUATOR));
									}
								}
							}

						}
						catch(ObjectNotFound& e)
						{
							if (!endpoint->SetProperties(*it, false, false))
							{
								result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
							}
							else
							{
								endpoint->GetProperties(result);
							}
						}
					}
					else
					{
						if (endpoint->SetProperties(*it, true, false))
						{
							endpoint->SetProperties(*it, false, false);

							Fields	fields;

							for(JSONNode::iterator it2 = it->begin(); it2 != it->end() ; it2++)
							{
								fields.Set(it2->name());
							}

							endpoint->GetProperties(result, fields);
							result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OK));
						}
						else
						{
							result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
						}
					}
				}

				response.AddEndpoint(result);
			}
			else
			{
				for(JSONNode::iterator it2 = it->begin() ; it2 != it->end() ; it2++)
				{
					JSONNode	result;
					std::string	id;

					id = JSONNodeGetID(*it2);

					result.push_back(JSONNode(TITLE_NAME_ID, id));

					Endpoint*	endpoint= manager_->GetEndpoint(id);
					if (endpoint == NULL)
					{
						TRACE_ERROR("Object[" << id << "not found!");
						result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
					}
					else
					{
						if (JSONNodeIsExistField(*it2, TITLE_NAME_VALUE))
						{
							try
							{
								std::string	value = JSONNodeGetValue(*it2);
								time_t	expire_time	= JSONNodeGetTimeOfExpire(*it2);
								time_t	current_time= time_t(Date::GetCurrent());	

								if (current_time > expire_time)
								{
									result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_TIME_EXPIRED));
								}
								else
								{
									EndpointActuator*	actuator = dynamic_cast<EndpointActuator*>(endpoint);

									if (actuator == NULL)
									{
										result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_NOT_ACTUATOR));
									}
									else
									{
										if (actuator->SetValue(value))
										{
											result.push_back(JSONNode(TITLE_NAME_VALUE, actuator->GetValue()));
											result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OK));
										}
										else
										{
											result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_NOT_ACTUATOR));
										}
									}
								}

							}
							catch(ObjectNotFound& e)
							{
								if (!endpoint->SetProperties(*it2, false, false))
								{
									result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
								}
								else
								{
									endpoint->GetProperties(result);
								}
							}
						}
						else
						{
							if (endpoint->SetProperties(*it2, true, false))
							{
								endpoint->SetProperties(*it2, false, false);

								Fields	fields;

								for(JSONNode::iterator it3 = it2->begin(); it3 != it2->end() ; it3++)
								{
									fields.Set(it3->name());
								}

								endpoint->GetProperties(result, fields);
								result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OK));
							}
							else
							{
								result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_INVALID_ARGUMENTS));
							}
						}
					}

					response.AddEndpoint(result);
				}
			}
		}
		else if (it->name() == TITLE_NAME_DATA)
		{
			JSONNode	result;
			std::string	id;
			std::string	value;
			time_t	time_of_expire;

			TRACE_ENTRY;
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
					TRACE_ENTRY;
					EndpointActuator* actuator = dynamic_cast<EndpointActuator*>(endpoint);
					if (actuator == NULL)
					{
						TRACE_ERROR("Endpoint[" << id << "] is not actuator!");
						result.push_back(JSONNode(TITLE_NAME_ID, id));
						result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_PERMISSION_DENY));
					}
					else
					{
						if (actuator->SetValue(value))
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
	uint32_t	gateway_fields 	= _request.GetGatewayFields();
	uint32_t	device_fields 	= _request.GetDeviceFields();
	uint32_t	endpoint_fields = _request.GetEndpointFields();

	TRACE_INFO("<RCSServer::Get>" << std::endl <<  payload.write_formatted() );
	for(JSONNode::iterator it = payload.begin(); it != payload.end() ; it++)
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
				gateway->GetProperties(result, gateway_fields);
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
				device->GetProperties(result, device_fields);
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
				endpoint->GetProperties(result, endpoint_fields);
			}

			response.AddEndpoint(result);
		}
		else if (it->name() == TITLE_NAME_DATA)
		{
			JSONNode	result;
			std::string	id = JSONNodeGetID(*it);

			Endpoint*	endpoint = manager_->GetEndpoint(id);
			if (endpoint == NULL)
			{
				TRACE_ERROR("The endpoint[" << id << "] not found.");
				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_OBJECT_NOT_FOUND));
			}
			else
			{	
				uint32_t	count = JSONNodeGetCount(*it, 0);
				time_t		start_time = JSONNodeGetStartTime(*it, 0);
				time_t		end_time = JSONNodeGetEndTime(*it, time_t(Date::GetCurrent()));

				if (count > max_data_get_count_)
				{
					count = max_data_get_count_;	
				}

				Endpoint::ValueMap value_map(count);

				if (count == 0)
				{
					endpoint->GetDataForPeriod(start_time, end_time, value_map);
				}
				else
				{
					endpoint->GetData(count, value_map);
				}

				JSONNode	array(JSON_ARRAY);
				for(Endpoint::ValueMap::iterator value_it = value_map.begin() ; value_it != value_map.end() ; value_it++)
				{
					JSONNode	value;

					value.push_back(JSONNode(TITLE_NAME_TIME, ToString(time_t(value_it->first))));
					value.push_back(JSONNode(TITLE_NAME_VALUE, value_it->second));

					array.push_back(value);
				}

				result.push_back(JSONNode(TITLE_NAME_ID, id));
				result.push_back(JSONNode(TITLE_NAME_COUNT, value_map.size()));
				array.set_name(TITLE_NAME_VALUE);
				result.push_back(array);

				response.AddEPData(result);
			}

		}
	}

	_response = response;

	return	true;
}

bool	RCSServer::CmdStart(RCSMessage& _request, RCSMessage& _response)
{
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);
	JSONNode	payload  = _request.GetPayload();

	for(JSONNode::iterator it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			JSONNode	result(JSON_NODE);
			std::string id = JSONNodeGetID(*it);
			
			result.push_back(JSONNode(TITLE_NAME_ID, id));	

			if (!manager_->StartGateway(id))
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}

			response.AddGateway(result);
		}
		else if (it->name() == TITLE_NAME_DEVICE)
		{
			JSONNode	result(JSON_NODE);
			std::string id = JSONNodeGetID(*it);

			result.push_back(JSONNode(TITLE_NAME_ID, id));	

			if (!manager_->StartDevice(id))
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}

			response.AddDevice(result);
		}
		else if (it->name() == TITLE_NAME_ENDPOINT)
		{
			JSONNode	result(JSON_NODE);
			std::string id = JSONNodeGetID(*it);

			result.push_back(JSONNode(TITLE_NAME_ID, id));	
			if (!manager_->StartEndpoint(id))
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}

			response.AddEndpoint(result);
		}
	}	

	_response = response;
	
	return	true;
}

bool	RCSServer::CmdStop(RCSMessage& _request, RCSMessage& _response)
{
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);
	JSONNode	payload  = _request.GetPayload();

	for(JSONNode::iterator it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			JSONNode	result(JSON_NODE);
			std::string id = JSONNodeGetID(*it);
			
			result.push_back(JSONNode(TITLE_NAME_ID, id));	
			if (!manager_->StopGateway(id))
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}

			response.AddGateway(result);
		}
		else if (it->name() == TITLE_NAME_DEVICE)
		{
			JSONNode	result(JSON_NODE);
			std::string id = JSONNodeGetID(*it);

			result.push_back(JSONNode(TITLE_NAME_ID, id));	
			if (!manager_->StopDevice(id))
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
			}

			response.AddDevice(result);
		}
		else if (it->name() == TITLE_NAME_ENDPOINT)
		{
			JSONNode	result(JSON_NODE);
			std::string id = JSONNodeGetID(*it);

			result.push_back(JSONNode(TITLE_NAME_ID, id));	
			if (!manager_->StopEndpoint(id))
			{
				result.push_back(JSONNode(TITLE_NAME_RESULT, RET_CONST_FAILED));
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

	for(JSONNode::iterator it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			if (manager_->GetGatewayCount() != 0)
			{
				std::list<Gateway*> list;

				manager_->GetGatewayList(list);

				for(std::list<Gateway*>::iterator gateway = list.begin(); gateway != list.end() ; gateway++)
				{
					JSONNode	result;

					(*gateway)->GetProperties(result, PROPERTY_ID);

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

				for(std::list<Device*>::iterator device = list.begin(); device != list.end() ; device++)
				{
					JSONNode	result;

					(*device)->GetProperties(result, PROPERTY_ID);

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

				for(std::list<Endpoint*>::iterator endpoint = list.begin(); endpoint != list.end() ; endpoint++)
				{
					JSONNode	result;

					(*endpoint)->GetProperties(result, PROPERTY_ID);

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
		for(JSONNode::iterator it = reply_payload.begin(); it != reply_payload.end() ; it++)
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
		for(JSONNode::iterator it = reply_payload.begin(); it != reply_payload.end() ; it++)
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
		for(JSONNode::iterator it = reply_payload.begin(); it != reply_payload.end() ; it++)
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
		std::string	id = JSONNodeGetID(_node);

		if (_req_type == MSG_TYPE_RCS_ADD)
		{
			Gateway*	gateway = manager_->GetGateway(id);
			if (gateway == NULL)
			{
				THROW_OBJECT_NOT_FOUND("The gateway[ " << id << "] not found!");
			}

			gateway->SetRegistered(true);
//			gateway->SetProperties(_node, false, false);
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
		for(JSONNode::iterator it = _node.begin() ; it != _node.end() ; it++)
		{
			ConfirmGateway(*it, _req_type);
		}
	}
	else
	{
		THROW_INVALID_ARGUMENT("Invalid message format!");
	}
}

bool	RCSServer::ConfirmDevice(JSONNode& _node, std::string& _req_type)
{
	if (_node.type() == JSON_NODE)
	{
		std::string	id = JSONNodeGetID(_node);

		if (_req_type == MSG_TYPE_RCS_ADD)
		{
			Device*	device = manager_->GetDevice(id);
			if (device == NULL)
			{
				TRACE_ERROR("Object[" << id << "] not found!");
				return	false;
			}

			device->SetRegistered(true);
//			device->SetProperties(_node, false, false);
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
		for(JSONNode::iterator it = _node.begin() ; it != _node.end() ; it++)
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
		std::string	id = JSONNodeGetID(_node);

		if (_req_type == MSG_TYPE_RCS_ADD)
		{
			Endpoint*	endpoint = manager_->GetEndpoint(id);
			if (endpoint == NULL)
			{
				TRACE_ERROR("Object[" << id << "] not found!");
				return	false;
			}

			endpoint->SetRegistered(true);
//			endpoint->SetProperties(_node, false, false);
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
		for(JSONNode::iterator it = _node.begin() ; it != _node.end() ; it++)
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
	std::string	id = JSONNodeGetID(_node);
	if (_req_type == MSG_TYPE_RCS_REPORT)
	{
		time_t		last_time;
		Endpoint*	endpoint = manager_->GetEndpoint(id);
		if (endpoint == NULL)
		{
			TRACE_ERROR("Object[" << id << "] not found!");
			return	false;
		}

		last_time = JSONNodeGetLastTime(_node);

		endpoint->SetLastConfirmTime(Date(last_time));
		TRACE_INFO("The data for the endpoint[" << id << "] has been transferred.");
	}

	return	true;
}


