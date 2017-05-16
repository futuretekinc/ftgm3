#include <fstream>
#include <libjson/libjson.h>
#include "defined.h"
#include "trace.h"
#include "object_manager.h"
#include "endpoint.h"
#include "endpoint.h"
#include "property.h"
#include "remote_message_server.h"

bool	RemoteMessageServer::EndpointMessageService::Service(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		std::string	command = _request.at_nocase(RMC_FIELD_COMMAND).as_string();

		_response.push_back(JSONNode(RMC_FIELD_COMMAND, command));
		if (command == "add")
		{
			result = Add(_rms, _request, _response, _error_message);
		}
		else if (command == "del")
		{
			result = Del(_rms, _request, _response, _error_message);
		}
		else if (command == "list")
		{
			result = List(_rms, _request, _response, _error_message);
		}
		else if (command == "get")
		{	
			result = Get(_rms, _request, _response, _error_message);
		}
		else if (command == "set")
		{	
			result = Set(_rms, _request, _response, _error_message);
		}
		else if (command == "start")
		{	
			result = Start(_rms, _request, _response, _error_message);
		}
		else if (command == "stop")
		{	
			result = Stop(_rms, _request, _response, _error_message);
		}
		else if (command == "enable")
		{	
			result = Enable(_rms, _request, _response, _error_message);
		}
		else if (command == "disable")
		{	
			result = Disable(_rms, _request, _response, _error_message);
		}
		else
		{
			_error_message << "Failed to call service becuase command[" << command <<"] is unknown!";
			TRACE_ERROR(_error_message.str());
			result = false;
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to call service becuase command is not exist!";
		TRACE_ERROR(_error_message.str());
		result = false;
	}
	
	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::Add(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	endpoint_node = _request.at_nocase(RMC_FIELD_DEVICE);

			if (endpoint_node.type() != JSON_NODE)
			{
				throw std::invalid_argument("Endpoint is not node!");
			}

			Properties	properties;

			properties.Append(_request);

			Endpoint* new_endpoint = _rms.object_manager_->CreateEndpoint(properties);
			if (new_endpoint == NULL)
			{
				throw std::invalid_argument("Failed to add endpoint because endpoint properties is invalid");
			}

			Properties	new_properties;

			new_endpoint->GetProperties(new_properties);
			JSONNode	new_endpoint_node = ToJSON(new_properties);
			new_endpoint_node.set_name(RMC_FIELD_DEVICE);
			_response.push_back(new_endpoint_node);
		}
		catch(std::out_of_range)
		{
			result =ArrayAdd(_rms, _request, _response, _error_message);
		}
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::ArrayAdd(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	array_node = _request.at_nocase(RMC_FIELD_DEVICE_ARRAY);
			JSONNode	new_endpoint_array_node(JSON_ARRAY);

			if (array_node.type() != JSON_ARRAY)
			{
				throw std::invalid_argument("Endpoints is not array!");
			}

			for(auto it = array_node.begin() ; it != array_node.end(); it++)
			{
				if (it->type() != JSON_NODE)
				{
					throw std::invalid_argument("Endpoints element is not node!");
				}
			}

			for(auto it = array_node.begin() ; it != array_node.end(); it++)
			{
				Properties	properties;

				properties.Append(it->as_node());

				Endpoint* new_endpoint = _rms.object_manager_->CreateEndpoint(properties);
				if (new_endpoint == NULL)
				{
					throw std::invalid_argument("Failed to add endpoint because endpoint properties is invalid");
				}

				Properties	new_properties;

				new_endpoint->GetProperties(new_properties);
				JSONNode	new_endpoint_node = ToJSON(new_properties);

				new_endpoint_array_node.push_back(new_endpoint_node);
			}

			new_endpoint_array_node.set_name(RMC_FIELD_DEVICE_ARRAY);
			_response.push_back(new_endpoint_array_node);
		}
		catch(std::out_of_range)
		{
			_error_message << "Failed to add endpoint because endpoint porperties is invalid.";
			TRACE_ERROR(_error_message.str());
			result = false;
		}
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::Del(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(RMC_FIELD_ID);
		if (id_node.type() != JSON_STRING)
		{
			throw std::invalid_argument("id value is string.");
		}

		result = _rms.object_manager_->DestroyEndpoint(id_node.as_string());
	}
	catch(std::out_of_range)
	{
		result = ArrayDel(_rms, _request, _response, _error_message);
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::ArrayDel(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	ids_node = _request.at_nocase(RMC_FIELD_ID_ARRAY);
		if (ids_node.type() != JSON_ARRAY)
		{
			throw std::invalid_argument("ids value is array.");
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			if (it->type() != JSON_STRING)
			{
				throw std::invalid_argument("id value is string.");
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			Endpoint* endpoint = _rms.object_manager_->GetEndpoint(it->as_string());
			if (endpoint == NULL)
			{
				std::ostringstream	oss;

				oss << "Failed to delete endpoint because endpoint[" << it->as_string() << "] is not found!";
				throw std::invalid_argument(oss.str());
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			result = _rms.object_manager_->DestroyEndpoint(it->as_string());
			if (result == false)
			{
				_error_message << "Failed to del endpoint[" << it->as_string() << "]";
				TRACE_ERROR(_error_message.str());
			}
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del endpoint because endpoint porperties is invalid.";
		TRACE_ERROR(_error_message.str());
		result = false;
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::List(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;

	JSONNode	endpoint_array(JSON_ARRAY);

	endpoint_array.set_name(RMC_FIELD_DEVICE_ARRAY);

	std::list<Endpoint*>	endpoint_list;

	_rms.object_manager_->GetEndpointList(endpoint_list);

	for(auto endpoint_it = endpoint_list.begin() ; endpoint_it != endpoint_list.end(); endpoint_it++)
	{
		endpoint_array.push_back(JSONNode("", (*endpoint_it)->GetID()));
	}

	_response.push_back(endpoint_array);
	
	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::Get(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	id_node = _request.at_nocase(RMC_FIELD_ID);
			if (id_node.type() != JSON_STRING)
			{
				throw std::invalid_argument("id value is string.");
			}

			std::string id = id_node.as_string();

			Endpoint*	endpoint = _rms.object_manager_->GetEndpoint(id);
			if (endpoint != NULL)
			{
				Properties	properties;

				endpoint->GetProperties(properties);				

				JSONNode	properties_node = ToJSON(properties);
				properties_node.set_name(RMC_FIELD_DEVICE);
				_response.push_back(properties_node);
			}
			else
			{
				_error_message << "Failed to get endpoint information because endpoint[" << id << "] is not found!";	
				TRACE_ERROR(_error_message.str());
				result = false;
			}
		}
		catch(std::out_of_range)
		{
			result = ArrayGet(_rms, _request, _response, _error_message);
		}
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::ArrayGet(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	id_node = _request.at_nocase(RMC_FIELD_ID_ARRAY);
			if (id_node.type() != JSON_ARRAY)
			{
				throw	std::invalid_argument("id properties value is array!");
			}

			JSONNode	endpoint_array(JSON_ARRAY);
			for(auto it = id_node.begin() ; it != id_node.end() ; it++)
			{
				std::string id = it->as_string();

				Endpoint*	endpoint = _rms.object_manager_->GetEndpoint(id);
				if (endpoint != NULL)
				{
					Properties	properties;

					endpoint->GetProperties(properties);				

					JSONNode	properties_node = ToJSON(properties);
					endpoint_array.push_back(properties_node);
				}
			}

			if (endpoint_array.size() != 0)
			{
				endpoint_array.set_name(RMC_FIELD_DEVICE_ARRAY);
				_response.push_back(endpoint_array);
			}
			else
			{
				_error_message << "Failed to get endpoint information because endpoint is not found!";	
				TRACE_ERROR(_error_message.str());
				result = false;
			}
		}
		catch(std::out_of_range)
		{
			_error_message << "Failed to get endpoint because id field is not found";
			TRACE_ERROR(_error_message.str());
			result = false;
		}
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::Set(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	endpoint_node = _request.at_nocase(RMC_FIELD_DEVICE);

			if (endpoint_node.type() != JSON_NODE)
			{
				throw std::invalid_argument("Endpoint is not node!");
			}

			JSONNode	id_node = endpoint_node["id"];
			std::string	id = id_node.as_string();

			Properties	properties;
			properties.Append(_request);

		}
		catch(std::out_of_range)
		{
			result = ArrayAdd(_rms, _request, _response, _error_message);
		}
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::Start(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(RMC_FIELD_ID);
		if (id_node.type() != JSON_STRING)
		{
			throw std::invalid_argument("id value is string.");
		}

		Endpoint*	endpoint = _rms.object_manager_->GetEndpoint(id_node.as_string());
		if (endpoint != NULL)
		{
			endpoint->Start();
		}
		else
		{
			_error_message << "Endpoint[" << id_node.as_string() << "] not found!";
		}
	}
	catch(std::out_of_range)
	{
		result = ArrayStart(_rms, _request, _response, _error_message);
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::ArrayStart(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	ids_node = _request.at_nocase(RMC_FIELD_ID_ARRAY);
		if (ids_node.type() != JSON_ARRAY)
		{
			throw std::invalid_argument("ids value is array.");
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			if (it->type() != JSON_STRING)
			{
				throw std::invalid_argument("id value is string.");
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			Endpoint* endpoint = _rms.object_manager_->GetEndpoint(it->as_string());
			if (endpoint == NULL)
			{
				std::ostringstream	oss;

				oss << "Failed to delete endpoint because endpoint[" << it->as_string() << "] is not found!";
				throw std::invalid_argument(oss.str());
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			Endpoint* endpoint = _rms.object_manager_->GetEndpoint(it->as_string());

			endpoint->Start();
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del endpoint because endpoint porperties is invalid.";
		TRACE_ERROR(_error_message.str());
		result = false;
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::Stop(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(RMC_FIELD_ID);
		if (id_node.type() != JSON_STRING)
		{
			throw std::invalid_argument("id value is string.");
		}

		Endpoint*	endpoint = _rms.object_manager_->GetEndpoint(id_node.as_string());
		if (endpoint != NULL)
		{
			endpoint->Stop();
		}
		else
		{
			_error_message << "Endpoint[" << id_node.as_string() << "] not found!";
		}
	}
	catch(std::out_of_range)
	{
		result = ArrayStop(_rms, _request, _response, _error_message);
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::ArrayStop(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	ids_node = _request.at_nocase(RMC_FIELD_ID_ARRAY);
		if (ids_node.type() != JSON_ARRAY)
		{
			throw std::invalid_argument("ids value is array.");
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			if (it->type() != JSON_STRING)
			{
				throw std::invalid_argument("id value is string.");
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			Endpoint* endpoint = _rms.object_manager_->GetEndpoint(it->as_string());
			if (endpoint == NULL)
			{
				std::ostringstream	oss;

				oss << "Failed to delete endpoint because endpoint[" << it->as_string() << "] is not found!";
				throw std::invalid_argument(oss.str());
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			Endpoint* endpoint = _rms.object_manager_->GetEndpoint(it->as_string());

			endpoint->Stop();
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del endpoint because endpoint porperties is invalid.";
		TRACE_ERROR(_error_message.str());
		result = false;
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::Enable(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(RMC_FIELD_ID);
		if (id_node.type() != JSON_STRING)
		{
			throw std::invalid_argument("id value is string.");
		}

		Endpoint*	endpoint = _rms.object_manager_->GetEndpoint(id_node.as_string());
		if (endpoint != NULL)
		{
			endpoint->SetEnable(true);
		}
		else
		{
			_error_message << "Endpoint[" << id_node.as_string() << "] not found!";
		}
	}
	catch(std::out_of_range)
	{
		result = ArrayEnable(_rms, _request, _response, _error_message);
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::ArrayEnable(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	ids_node = _request.at_nocase(RMC_FIELD_ID_ARRAY);
		if (ids_node.type() != JSON_ARRAY)
		{
			throw std::invalid_argument("ids value is array.");
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			if (it->type() != JSON_STRING)
			{
				throw std::invalid_argument("id value is string.");
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			Endpoint* endpoint = _rms.object_manager_->GetEndpoint(it->as_string());
			if (endpoint == NULL)
			{
				std::ostringstream	oss;

				oss << "Failed to delete endpoint because endpoint[" << it->as_string() << "] is not found!";
				throw std::invalid_argument(oss.str());
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			Endpoint* endpoint = _rms.object_manager_->GetEndpoint(it->as_string());

			endpoint->SetEnable(true);
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del endpoint because endpoint porperties is invalid.";
		TRACE_ERROR(_error_message.str());
		result = false;
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::Disable(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(RMC_FIELD_ID);
		if (id_node.type() != JSON_STRING)
		{
			throw std::invalid_argument("id value is string.");
		}

		Endpoint*	endpoint = _rms.object_manager_->GetEndpoint(id_node.as_string());
		if (endpoint != NULL)
		{
			endpoint->SetEnable(false);
		}
		else
		{
			_error_message << "Endpoint[" << id_node.as_string() << "] not found!";
		}
	}
	catch(std::out_of_range)
	{
		result = ArrayDisable(_rms, _request, _response, _error_message);
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::EndpointMessageService::ArrayDisable(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	ids_node = _request.at_nocase(RMC_FIELD_ID_ARRAY);
		if (ids_node.type() != JSON_ARRAY)
		{
			throw std::invalid_argument("ids value is array.");
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			if (it->type() != JSON_STRING)
			{
				throw std::invalid_argument("id value is string.");
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			Endpoint* endpoint = _rms.object_manager_->GetEndpoint(it->as_string());
			if (endpoint == NULL)
			{
				std::ostringstream	oss;

				oss << "Failed to delete endpoint because endpoint[" << it->as_string() << "] is not found!";
				throw std::invalid_argument(oss.str());
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			Endpoint* endpoint = _rms.object_manager_->GetEndpoint(it->as_string());

			endpoint->SetEnable(false);
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del endpoint because endpoint porperties is invalid.";
		TRACE_ERROR(_error_message.str());
		result = false;
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

