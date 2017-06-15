#include <fstream>
#include <libjson/libjson.h>
#include "defined.h"
#include "trace.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint.h"
#include "property.h"
#include "rcs_session.h"
#if 0
bool	RCSSession::DeviceMessageService::Service(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		std::string	command = _request.at_nocase(TITLE_NAME_COMMAND).as_string();

		_response.push_back(JSONNode(TITLE_NAME_COMMAND, command));
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

bool	RCSSession::DeviceMessageService::Add(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	device_node = _request.at_nocase(TITLE_NAME_DEVICE);

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

			JSONNode	new_properties;

			new_device->GetProperties(new_properties);
			new_properties.set_name(TITLE_NAME_DEVICE);
			_response.push_back(new_properties);
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

bool	RCSSession::DeviceMessageService::ArrayAdd(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	array_node = _request.at_nocase(TITLE_NAME_DEVICE);
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

				JSONNode new_properties;

				new_device->GetProperties(new_properties);
				new_device_array_node.push_back(new_properties);
			}

			new_device_array_node.set_name(TITLE_NAME_DEVICE);
			_response.push_back(new_device_array_node);
		}
		catch(std::out_of_range)
		{
			_error_message << "Failed to add device because device porperties is invalid.";
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

bool	RCSSession::DeviceMessageService::Del(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(TITLE_NAME_ID);
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
		TRACE_ERROR(_error_message.str());
		result = false;
	}

	return	result;
}

bool	RCSSession::DeviceMessageService::ArrayDel(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	ids_node = _request.at_nocase(TITLE_NAME_ID);
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
				TRACE_ERROR(_error_message.str());
			}
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del device because device porperties is invalid.";
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

bool	RCSSession::DeviceMessageService::List(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;

	JSONNode	device_array(JSON_ARRAY);

	device_array.set_name(TITLE_NAME_DEVICE);

	std::list<Device*>	device_list;
	_rms.object_manager_->GetDeviceList(device_list);

	for(auto device_it = device_list.begin() ; device_it != device_list.end(); device_it++)
	{
		device_array.push_back(JSONNode("", (*device_it)->GetID()));
	}

	_response.push_back(device_array);
	
	return	result;
}

bool	RCSSession::DeviceMessageService::Get(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	id_node = _request.at_nocase(TITLE_NAME_ID);
			if (id_node.type() != JSON_STRING)
			{
				throw std::invalid_argument("id value is string.");
			}

			std::string id = id_node.as_string();

			Device*	device = _rms.object_manager_->GetDevice(id);
			if (device != NULL)
			{
				JSONNode	properties;

				device->GetProperties(properties);				

				properties.set_name(TITLE_NAME_DEVICE);
				_response.push_back(properties);
			}
			else
			{
				_error_message << "Failed to get device information because device[" << id << "] is not found!";	
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

bool	RCSSession::DeviceMessageService::ArrayGet(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	id_node = _request.at_nocase(TITLE_NAME_ID);
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
					JSONNode	properties;

					device->GetProperties(properties);				

					device_array.push_back(properties);
				}
			}

			if (device_array.size() != 0)
			{
				device_array.set_name(TITLE_NAME_DEVICE);
				_response.push_back(device_array);
			}
			else
			{
				_error_message << "Failed to get device information because device is not found!";	
				TRACE_ERROR(_error_message.str());
				result = false;
			}
		}
		catch(std::out_of_range)
		{
			_error_message << "Failed to get device because id field is not found";
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

bool	RCSSession::DeviceMessageService::Set(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		try
		{
			JSONNode	device_node = _request.at_nocase(TITLE_NAME_DEVICE);

			if (device_node.type() != JSON_NODE)
			{
				throw std::invalid_argument("Device is not node!");
			}

			JSONNode	id_node = device_node[TITLE_NAME_ID];
			std::string	id = id_node.as_string();

			Device *device = _rms.object_manager_->GetDevice(id);
			if(device != NULL)
			{
				Properties	properties;
				properties.Append(_request);

				properties.Delete(TITLE_NAME_ID);

				result = device->SetProperties(properties);
			}
			else
			{
				_error_message << "Failed to get device[" << id << "]";
				result = false;
			}	
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


bool	RCSSession::DeviceMessageService::Start(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(TITLE_NAME_ID);
		if (id_node.type() != JSON_STRING)
		{
			throw std::invalid_argument("id value is string.");
		}

		Device*	device = _rms.object_manager_->GetDevice(id_node.as_string());
		if (device != NULL)
		{
			device->Start();
		}
		else
		{
			_error_message << "Device[" << id_node.as_string() << "] not found!";
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

bool	RCSSession::DeviceMessageService::ArrayStart(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	ids_node = _request.at_nocase(TITLE_NAME_ID);
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
			Device* device = _rms.object_manager_->GetDevice(it->as_string());

			device->Start();
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del device because device porperties is invalid.";
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

bool	RCSSession::DeviceMessageService::Stop(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(TITLE_NAME_ID);
		if (id_node.type() != JSON_STRING)
		{
			throw std::invalid_argument("id value is string.");
		}

		Device*	device = _rms.object_manager_->GetDevice(id_node.as_string());
		if (device != NULL)
		{
			device->Stop();
		}
		else
		{
			_error_message << "Device[" << id_node.as_string() << "] not found!";
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

bool	RCSSession::DeviceMessageService::ArrayStop(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	ids_node = _request.at_nocase(TITLE_NAME_ID);
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
			Device* device = _rms.object_manager_->GetDevice(it->as_string());

			device->Stop();
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del device because device porperties is invalid.";
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

bool	RCSSession::DeviceMessageService::Enable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(TITLE_NAME_ID);
		if (id_node.type() != JSON_STRING)
		{
			throw std::invalid_argument("id value is string.");
		}

		Device*	device = _rms.object_manager_->GetDevice(id_node.as_string());
		if (device != NULL)
		{
			device->SetEnable(true);
		}
		else
		{
			_error_message << "Device[" << id_node.as_string() << "] not found!";
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

bool	RCSSession::DeviceMessageService::ArrayEnable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	ids_node = _request.at_nocase(TITLE_NAME_ID);
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
			Device* device = _rms.object_manager_->GetDevice(it->as_string());

			device->SetEnable(true);
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del device because device porperties is invalid.";
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

bool	RCSSession::DeviceMessageService::Disable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	id_node = _request.at_nocase(TITLE_NAME_ID);
		if (id_node.type() != JSON_STRING)
		{
			throw std::invalid_argument("id value is string.");
		}

		Device*	device = _rms.object_manager_->GetDevice(id_node.as_string());
		if (device != NULL)
		{
			device->SetEnable(false);
		}
		else
		{
			_error_message << "Device[" << id_node.as_string() << "] not found!";
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

bool	RCSSession::DeviceMessageService::ArrayDisable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message)
{
	bool	result = true;
	try
	{
		JSONNode	ids_node = _request.at_nocase(TITLE_NAME_ID);
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
			Device* device = _rms.object_manager_->GetDevice(it->as_string());

			device->SetEnable(false);
		}
	}
	catch(std::out_of_range)
	{
		_error_message << "Failed to del device because device porperties is invalid.";
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

#endif
