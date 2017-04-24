#include <fstream>
#include <libjson/libjson.h>
#include "defined.h"
#include "trace.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint.h"
#include "property.h"
#include "remote_message_server.h"

bool	RemoteMessageServer::DeviceMessageService::Service(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
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
		else
		{
			_error_message << "Failed to call service becuase command[" << command <<"] is unknown!";
			TRACE_ERROR << _error_message.str() << Trace::End;
			result = false;
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to call service becuase command is not exist!";
		TRACE_ERROR << _error_message.str() << Trace::End;
		result = false;
	}
	
	return	result;
}

bool	RemoteMessageServer::DeviceMessageService::Add(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	device_node = _request.at_nocase(RMC_FIELD_DEVICE);

			if (device_node.type() != JSON_NODE)
			{
				throw std::invalid_argument("Device is not node!");
			}

			Properties	properties;

			properties.Append(_request);

			Device* new_device = _rms.object_manager_->CreateDevice(properties);
			if (new_device == NULL)
			{
				throw std::invalid_argument("Failed to add device because device properties is invalid");
			}

			Properties	new_properties;

			new_device->GetProperties(new_properties);
			JSONNode	new_device_node = ToJSON(new_properties);
			new_device_node.set_name(RMC_FIELD_DEVICE);
			_response.push_back(new_device_node);
		}
		catch(std::out_of_range)
		{
			result = ArrayAdd(_rms, _request, _response, _error_message);
		}
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR << _error_message.str() << Trace::End;
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::DeviceMessageService::ArrayAdd(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	array_node = _request.at_nocase(RMC_FIELD_DEVICE_ARRAY);
			JSONNode	new_device_array_node(JSON_ARRAY);

			if (array_node.type() != JSON_ARRAY)
			{
				throw std::invalid_argument("Devices is not array!");
			}

			for(auto it = array_node.begin() ; it != array_node.end(); it++)
			{
				if (it->type() != JSON_NODE)
				{
					throw std::invalid_argument("Devices element is not node!");
				}
			}

			for(auto it = array_node.begin() ; it != array_node.end(); it++)
			{
				Properties	properties;

				properties.Append(it->as_node());

				Device* new_device = _rms.object_manager_->CreateDevice(properties);
				if (new_device == NULL)
				{
					throw std::invalid_argument("Failed to add device because device properties is invalid");
				}

				Properties	new_properties;

				new_device->GetProperties(new_properties);
				JSONNode	new_device_node = ToJSON(new_properties);

				new_device_array_node.push_back(new_device_node);
			}

			new_device_array_node.set_name(RMC_FIELD_DEVICE_ARRAY);
			_response.push_back(new_device_array_node);
		}
		catch(std::out_of_range)
		{
			_error_message << "Failed to add device because device porperties is invalid.";
			TRACE_ERROR << _error_message.str() << Trace::End;
			result = false;
		}
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR << _error_message.str() << Trace::End;
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::DeviceMessageService::Del(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(RMC_FIELD_ID);
		if (id_node.type() != JSON_STRING)
		{
			throw std::invalid_argument("id value is string.");
		}

		result = _rms.object_manager_->DestroyDevice(id_node.as_string());
	}
	catch(std::out_of_range)
	{
		result = ArrayDel(_rms, _request, _response, _error_message);
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR << _error_message.str() << Trace::End;
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::DeviceMessageService::ArrayDel(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
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
			Device* device = _rms.object_manager_->GetDevice(it->as_string());
			if (device == NULL)
			{
				std::ostringstream	oss;

				oss << "Failed to delete device because device[" << it->as_string() << "] is not found!";
				throw std::invalid_argument(oss.str());
			}
		}

		for(auto it = ids_node.begin() ; it != ids_node.end() ; it++)
		{
			result = _rms.object_manager_->DestroyDevice(it->as_string());
			if (result == false)
			{
				_error_message << "Failed to del device[" << it->as_string() << "]";
				TRACE_ERROR << _error_message.str() << Trace::End;
			}
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del device because device porperties is invalid.";
		TRACE_ERROR << _error_message.str() << Trace::End;
		result = false;
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR << _error_message.str() << Trace::End;
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::DeviceMessageService::List(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;

	JSONNode	device_array(JSON_ARRAY);

	device_array.set_name(RMC_FIELD_DEVICE_ARRAY);

	std::list<Device*>	device_list;
	_rms.object_manager_->GetDeviceList(device_list);

	for(auto device_it = device_list.begin() ; device_it != device_list.end(); device_it++)
	{
		device_array.push_back(JSONNode("", (*device_it)->GetID()));
	}

	_response.push_back(device_array);
	
	return	result;
}

bool	RemoteMessageServer::DeviceMessageService::Get(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
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

			Device*	device = _rms.object_manager_->GetDevice(id);
			if (device != NULL)
			{
				Properties	properties;

				device->GetProperties(properties);				

				JSONNode	properties_node = ToJSON(properties);
				properties_node.set_name(RMC_FIELD_DEVICE);
				_response.push_back(properties_node);
			}
			else
			{
				_error_message << "Failed to get device information because device[" << id << "] is not found!";	
				TRACE_ERROR << _error_message.str() << Trace::End;
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
		TRACE_ERROR << _error_message.str() << Trace::End;
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::DeviceMessageService::ArrayGet(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
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

			JSONNode	device_array(JSON_ARRAY);
			for(auto it = id_node.begin() ; it != id_node.end() ; it++)
			{
				std::string id = it->as_string();

				Device*	device = _rms.object_manager_->GetDevice(id);
				if (device != NULL)
				{
					Properties	properties;

					device->GetProperties(properties);				

					JSONNode	properties_node = ToJSON(properties);
					device_array.push_back(properties_node);
				}
			}

			if (device_array.size() != 0)
			{
				device_array.set_name(RMC_FIELD_DEVICE_ARRAY);
				_response.push_back(device_array);
			}
			else
			{
				_error_message << "Failed to get device information because device is not found!";	
				TRACE_ERROR << _error_message.str() << Trace::End;
				result = false;
			}
		}
		catch(std::out_of_range)
		{
			_error_message << "Failed to get device because id field is not found";
			TRACE_ERROR << _error_message.str() << Trace::End;
			result = false;
		}
	}
	catch(std::invalid_argument& e)
	{
		_error_message << e.what();
		TRACE_ERROR << _error_message.str() << Trace::End;
		result = false;
	}

	return	result;
}

bool	RemoteMessageServer::DeviceMessageService::Set(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	device_node = _request.at_nocase(RMC_FIELD_DEVICE);

			if (device_node.type() != JSON_NODE)
			{
				throw std::invalid_argument("Device is not node!");
			}

			JSONNode	id_node = device_node["id"];
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
		TRACE_ERROR << _error_message.str() << Trace::End;
		result = false;
	}

	return	result;
}


