#include <fstream>
#include <libjson/libjson.h>
#include "defined.h"
#include "trace.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint.h"
#include "property.h"

ObjectManager::ObjectManager()
: ActiveObject()
{
	trace.SetClassName(GetClassName());
	name_ 	= "object_manager";
	enable_ = true;

	TRACE_INFO << "ObjectManager : " << id_ << Trace::End;
}

ObjectManager::~ObjectManager()
{
	for(auto it = device_map_.begin() ; it != device_map_.end() ; it++)
	{
		delete it->second;
	}
}

bool ObjectManager::Attach(DataManager* _data_manager)
{
	data_manager_ = _data_manager;

	
	return	true;
}

bool ObjectManager::Load(std::string const&  _file_name)
{
	std::fstream	fs(_file_name, std::fstream::in);
	if (fs)
	{
		fs.seekg (0, fs.end);
		int length = fs.tellg();
		fs.seekg (0, fs.beg);

		char * buffer = new char [length + 1];
		fs.read(buffer, length);
		buffer[length] = 0;
		fs.close();

		Load(buffer);

		delete buffer;
	}

	return	false;
}

bool ObjectManager::Load(const char *_buffer)
{
	if (!libjson::is_valid(_buffer))
	{
		std::cout << "Invalid json format" << Trace::End;
		return	false;
	}

	JSONNode	json = libjson::parse(_buffer);

	return	Load(json);
}

bool	ObjectManager::Load(JSONNode const& _json)
{
	if (_json.name() == "devices")
	{
		if (_json.type() != JSON_ARRAY)
		{
			return	false;
		}

		for(auto it = _json.begin(); it != _json.end() ; it++)
		{
			Properties	properties;

			properties.Append(*it);

			Device *device = CreateDevice(properties);
			if (device == NULL)
			{
				TRACE_ERROR << "Failed to create device!" << Trace::End;
			}
		}
	}
	else if (_json.type() != JSON_NODE)
	{
		std::cout << "Invalid json format" << Trace::End;
		return	false;
	}
	else
	{
		for(auto it = _json.begin(); it != _json.end() ; it++)
		{
			if (!Load(*it))
			{
				std::cout << "Invalid format" << Trace::End;
				return	false;
			}
		}
	}

	return	true;
}

Device*		ObjectManager::CreateDevice(Properties const& _properties, bool from_db)
{
	Device*	device = Device::Create(_properties);
	if (device != NULL)
	{
		if (!Attach(device))
		{	
			delete device;
			device = NULL;
		}
		else if ((!from_db) && (data_manager_ != NULL))
		{
			data_manager_->AddDevice(device);
		}
	}

	return	device;
}

bool		ObjectManager::DestroyDevice(std::string const& _id)
{
	Device* device = GetDevice(_id);
	if (device == NULL)
	{
		TRACE_ERROR << "Failed to delete device because the device[" << _id << "] coult not be found." << Trace::End;
		return	false;	
	}

	Detach(device);

	delete device;

	return	true;
}

uint32_t	ObjectManager::GetDeviceCount()
{
	return	device_map_.size();
}

uint32_t	ObjectManager::GetDeviceList(std::list<Device*>& _device_list)
{
	for(auto it = device_map_.begin(); it != device_map_.end() ; it++)
	{
		_device_list.push_back(it->second);	
	}

	return	_device_list.size();
}

uint32_t	ObjectManager::GetDeviceList(Device::Type _type, std::list<Device*>& _device_list)
{
	for(auto it = device_map_.begin(); it != device_map_.end() ; it++)
	{
		if (it->second->IsIncludedIn(_type))
		{
			_device_list.push_back(it->second);	
		}
	}

	return	_device_list.size();
}

Device*		ObjectManager::GetDevice(std::string const& _id)
{
	auto it = device_map_.find(_id);
	if (it != device_map_.end())
	{
		return	it->second;	
	}

	return	NULL;
}

Endpoint*	ObjectManager::CreateEndpoint(std::string const& _device_id, Properties const& _properties, bool from_db)
{
	Endpoint*	endpoint = NULL;
	Device*		device = GetDevice(_device_id);
	if (device != NULL)
	{
		Endpoint*	endpoint = device->CreateEndpoint(_properties);		
		if (endpoint != NULL)
		{
			if (Attach(endpoint) != true)
			{
				delete endpoint;
				endpoint = NULL;
			}
			else if ((!from_db) && (data_manager_ != NULL))
			{
				data_manager_->AddEndpoint(endpoint);
			}
		}
	}

	return	endpoint;
}

Endpoint*	ObjectManager::CreateEndpoint(Properties const& _properties, bool from_db)
{
	const Property* property = _properties.Get(std::string("device_id"));
	if (property == NULL)
	{
		TRACE_ERROR << "Failed to create endpoint!" << Trace::End;
		return	NULL;
	}

	const ValueString*	value = dynamic_cast<const ValueString*>(property->GetValue());
	if (value == NULL)
	{
		TRACE_ERROR << "Failed to create endpoint!" << Trace::End;
		return	NULL;
	}

	std::string	device_id = value->Get();

	return	CreateEndpoint(device_id, _properties, from_db);
}

bool		ObjectManager::DestroyEndpoint(std::string const& _id)
{
	Endpoint*	endpoint = GetEndpoint(_id);
	if (endpoint != NULL)
	{
		Device*	device = endpoint->GetDevice();
		if (device != NULL)
		{
			device->Detach(endpoint);	
		}
		
		auto it = endpoint_map_.find(endpoint->GetID());
		if (it != endpoint_map_.end())
		{
			endpoint_map_.erase(it);
		}

		delete endpoint;
		return	true;
	}

	return	false;
}

uint32_t	ObjectManager::GetEndpointCount()
{
	return	endpoint_map_.size();
}

uint32_t	ObjectManager::GetEndpointList(std::list<Endpoint*>& _endpoint_list)
{
	for(auto it = endpoint_map_.begin(); it != endpoint_map_.end() ; it++)
	{
		_endpoint_list.push_back(it->second);	
	}

	return	_endpoint_list.size();
}

Endpoint*		ObjectManager::GetEndpoint(std::string const& _id)
{
	auto it = endpoint_map_.find(_id);
	if (it != endpoint_map_.end())
	{
		return	it->second;	
	}

	return	NULL;
}

bool	ObjectManager::UpdateProperties(std::string const& _id)
{
	if (data_manager_ == NULL)
	{
		TRACE_ERROR << "Failed to update properties becasuse data manager is not attached." << Trace::End;
		return	false;	
	}

	auto device_it = device_map_.find(_id);
	if (device_it != device_map_.end())
	{
		Device* device = device_it->second;
	
		Properties	properties;
		if (device->GetProperties(properties))
		{
			properties.Delete("id");

			if (!data_manager_->SetDeviceProperties(_id, properties))
			{
				TRACE_ERROR << "Failed to set device[" << _id << "] properties!" << Trace::End;	
				return	false;
			}
		}
		else
		{
			TRACE_ERROR << "Failed to get device[" << _id << "] properties!" << Trace::End;
			return	false;
		}

		return	true;
	}

	auto endpoint_it = endpoint_map_.find(_id);
	if (endpoint_it != endpoint_map_.end())
	{
		Endpoint* endpoint = endpoint_it->second;
	
		Properties	properties;
		if (endpoint->GetProperties(properties))
		{
			properties.Delete("id");

			if (!data_manager_->SetDeviceProperties(_id, properties))
			{
				TRACE_ERROR << "Failed to set endpoint[" << _id << "] properties!" << Trace::End;	
				return	false;
			}
		}
		else
		{
			TRACE_ERROR << "Failed to get endpoint[" << _id << "] properties!" << Trace::End;
			return	false;
		}

		return	true;
	}

}

bool	ObjectManager::AddData(std::string const& _endpoint_id, Date const& _date, Value const* _value)
{
	if (data_manager_ != NULL)
	{
		return	data_manager_->AddValue(_endpoint_id, _date, _value);
	}
	return	false;
}

void	ObjectManager::Preprocess()
{
	if (data_manager_ != NULL)
	{
		uint32_t count = data_manager_->GetDeviceCount();
		TRACE_INFO << "Device Count : " << count << Trace::End;

		for(uint32_t i = 0 ; i < count ; i++)
		{
			std::list<Properties> properties_list;

			if (data_manager_->GetDeviceProperties(i, 1, properties_list))
			{
				for(auto it = properties_list.begin() ; it != properties_list.end() ; it++)
				{
					CreateDevice(*it, true);
				}
			}
		}

		count = data_manager_->GetEndpointCount();
		TRACE_INFO << "Endpoint Count : " << count << Trace::End;

		for(uint32_t i = 0 ; i < count ; i++)
		{
			std::list<Properties> properties_list;

			if (data_manager_->GetEndpointProperties(i, 1, properties_list))
			{
				for(auto it = properties_list.begin() ; it != properties_list.end() ; it++)
				{
					const Property*	id_property = it->Get("id");
					if (id_property != NULL)
					{
						CreateEndpoint(*it, true);						
					}
				}
			}
		}
	}

}

void	ObjectManager::OnMessage(Message* _base_message)
{
}

void	ObjectManager::Process()
{
	ActiveObject::Process();
}

void	ObjectManager::Postprocess()
{
	ActiveObject::Postprocess();
}



bool	ObjectManager::Attach(Device* _device)
{
	auto it = device_map_.find(_device->GetID());
	if (it != device_map_.end())
	{
		TRACE_ERROR << "The device[" << _device->GetID() << "] has been already attached." << Trace::End;
		return	false;	
	}

	device_map_[_device->GetID()] = _device;
	_device->parent_ = this;

	std::list<Endpoint*>	_endpoint_list;

	_device->GetEndpointList(_endpoint_list);
	for(auto it = _endpoint_list.begin(); it != _endpoint_list.end() ; it++)
	{
		Attach(*it);
	}

	TRACE_INFO << "The device[" << _device->GetID() << "] has been attached." << Trace::End; 
	return	true;
}

bool	ObjectManager::Detach(Device* _device)
{
	auto it = device_map_.find(_device->GetID());
	if (it == device_map_.end())
	{
		TRACE_ERROR << "The device[" << _device->GetID() << "] not attached." << Trace::End;
		return	false;	
	}

	device_map_.erase(it);
	_device->parent_ = NULL;

	std::list<Endpoint*>	_endpoint_list;

	_device->GetEndpointList(_endpoint_list);
	for(auto it = _endpoint_list.begin(); it != _endpoint_list.end() ; it++)
	{
		Detach(*it);
	}

	return	true;
}

bool	ObjectManager::Attach(Endpoint* _endpoint)
{
	auto it = endpoint_map_.find(_endpoint->GetID());
	if (it != endpoint_map_.end())
	{
		TRACE_ERROR << "The endpoint[" << _endpoint->GetID() << "] has been already attached." << Trace::End;
		return	false;	
	}

	endpoint_map_[_endpoint->GetID()] = _endpoint;
	_endpoint->parent_ = this;

	TRACE_INFO << "The endpoint[" << _endpoint->GetID() << "] has been attached." << Trace::End; 

	return	true;
}

bool	ObjectManager::Detach(Endpoint* _endpoint)
{
	auto it = endpoint_map_.find(_endpoint->GetID());
	if (it == endpoint_map_.end())
	{
		TRACE_ERROR << "The endpoint[" << _endpoint->GetID() << "] not attached." << Trace::End;
		return	false;	
	}

	endpoint_map_.erase(it);
	_endpoint->parent_ = this;

	return	true;
}

bool	ObjectManager::Attach(RemoteMessageServer* _rms)
{
	auto it = rms_map_.find(_rms->GetID());
	if (it != rms_map_.end())
	{
		TRACE_ERROR << "The rms[" << _rms->GetID() << "] has been already attached." << Trace::End;
		return	false;	
	}

	rms_map_[_rms->GetID()] = _rms;
	_rms->parent_ = this;

	TRACE_INFO << "The rms[" << _rms->GetID() << "] has been attached." << Trace::End; 

	return	true;
}

bool	ObjectManager::Detach(RemoteMessageServer* _rms)
{
	auto it = rms_map_.find(_rms->GetID());
	if (it == rms_map_.end())
	{
		TRACE_ERROR << "The rms[" << _rms->GetID() << "] not attached." << Trace::End;
		return	false;	
	}

	rms_map_.erase(it);
	_rms->parent_ = this;

	return	true;
}
