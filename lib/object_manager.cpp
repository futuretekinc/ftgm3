#include <fstream>
#include <libjson/libjson.h>
#include <unistd.h>
#include "defined.h"
#include "trace.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint.h"
#include "property.h"

ObjectManager::ObjectManager()
: 	ActiveObject(), 
	server_linker_(), 
	data_manager_(DB_DEFAULT_FILE),
	endpoint_report_interval_(ENDPOINT_REPORT_INTERVAL),
	auto_start_(false)
{
	name_ 	= "object_manager";
	enable_ = true;

	trace.SetClassName(GetClassName());
	server_linker_.AddBroker(MSG_BROKER_DEFAULT);
}

ObjectManager::~ObjectManager()
{
	for(auto it = device_map_.begin() ; it != device_map_.end() ; it++)
	{
		delete it->second;
	}

	for(auto it = endpoint_map_.begin() ; it != endpoint_map_.end() ; it++)
	{
		delete it->second;
	}
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
		std::cout << "Invalid json format" << std::endl;
		return	false;
	}

	JSONNode	json = libjson::parse(_buffer);

	return	Load(json);
}

bool	ObjectManager::Load(JSONNode const& _json)
{
	bool	ret_value = true;
	if (_json.type() == JSON_NODE)
	{
		for(auto it = _json.begin(); it != _json.end() ; it++)
		{
			ret_value = Load(*it);
			if (!ret_value)
			{
				TRACE_ERROR("Invalid json format");
			}
		}
	}
	else if (_json.name() == "auto_start")
	{
		if ((_json.as_string() == "yes") &&  (_json.as_string() == "on"))
		{
			auto_start_ = true;
		}
		else if ((_json.as_string() == "no") &&  (_json.as_string() == "off"))
		{
			auto_start_ = false;
		}
	}
	else if (_json.name() == "endpoint_report_interval")
	{
		endpoint_report_interval_ = _json.as_int();
	}
	else if (_json.name() == "device")
	{
		if (_json.type() == JSON_ARRAY)
		{
			for(auto it = _json.begin(); it != _json.end() ; it++)
			{
				Properties	properties;

				properties.Append(*it);

				Device *device = CreateDevice(properties);
				if (device == NULL)
				{
					TRACE_ERROR("Failed to create device!");
					ret_value = false;
				}
			}
		}
		else if (_json.type() == JSON_NODE)
		{
			Properties	properties;

			properties.Append(_json);

			Device *device = CreateDevice(properties);
			if (device == NULL)
			{
				TRACE_ERROR("Failed to create device!");
				ret_value = false;
			}
		}
	}
	else if (_json.name() == "database")
	{
		ret_value = data_manager_.Load(_json);
	}
	else if (_json.name() == "server")
	{
		ret_value = server_linker_.Load(_json);
	}
	else if (_json.name() == "trace")
	{
		ret_value = trace.Load(_json);
	}
	else
	{
		TRACE_ERROR("Invalid json format");
		ret_value = false;
	}

	return	ret_value;
}

Device*		ObjectManager::CreateDevice(Properties const& _properties, bool from_db)
{
	Device*	device = Device::Create(*this, _properties);
	if (device != NULL)
	{
		if (!from_db)
		{
			data_manager_.AddDevice(device);
		}

		std::ostringstream	oss;

		oss << "v1_dev_" << device->GetID();
		server_linker_.AddUpLink(oss.str());
	}

	return	device;
}

bool		ObjectManager::DestroyDevice(std::string const& _id)
{
	Device* device = GetDevice(_id);
	if (device == NULL)
	{
		TRACE_ERROR("Failed to delete device because the device[" << _id << "] coult not be found.");
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

uint32_t	ObjectManager::GetDeviceList(ValueType const& _type, std::list<Device*>& _device_list)
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

	TRACE_ERROR("Device[" << _id << "] not found!");
	return	NULL;
}

Endpoint*	ObjectManager::CreateEndpoint(Properties const& _properties, bool from_db)
{
	Endpoint*	endpoint = Endpoint::Create(*this, _properties);		
	if (endpoint != NULL)
	{
		if (!from_db)
		{
			data_manager_.AddEndpoint(endpoint);
		}

		std::ostringstream	oss;

		oss << "v1_ep_" << endpoint->GetID();
		server_linker_.AddUpLink(oss.str());
	}
	else
	{
		TRACE_ERROR("Failed to create endpoint");	
	}

	return	endpoint;
}

bool		ObjectManager::DestroyEndpoint(std::string const& _endpoint_id)
{
	Endpoint*	endpoint = GetEndpoint(_endpoint_id);
	if (endpoint != NULL)
	{
		Device*	device = GetDevice(endpoint->GetDeviceID());
		if (device != NULL)
		{
			device->Detach(_endpoint_id);	
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

	TRACE_ERROR("Endpoint [" << _id << "] not found!");

	return	NULL;
}

bool	ObjectManager::IDChanged(Device* _device, ValueID const& _old_id)
{
	auto it = device_map_.find(std::string(_old_id));
	if (it != device_map_.end())
	{
		device_map_.erase(it);
		device_map_[_device->GetID()] = _device;
		return	true;	
	}

	return	false;
}

bool	ObjectManager::IDChanged(Endpoint* _endpoint, ValueID const& _old_id)
{
	auto it = endpoint_map_.find(std::string(_old_id));
	if (it != endpoint_map_.end())
	{
		endpoint_map_.erase(it);
		endpoint_map_[_endpoint->GetID()] = _endpoint;
		return	true;	
	}

	return	false;
}

bool	ObjectManager::UpdateProperties(Object* _object)
{
	Device* device = dynamic_cast<Device*>(_object);
	if (device != NULL)
	{
		return	UpdateProperties(device);
	}
	else
	{
		Endpoint* endpoint= dynamic_cast<Endpoint*>(_object);
		if (endpoint != NULL)
		{
			return	UpdateProperties(endpoint);
		}
	}

	return	false;
}

bool	ObjectManager::UpdateProperties(Device* _device)
{
	Properties	properties;
	if (_device->GetUpdatedProperties(properties))
	{
		properties.Delete(OBJECT_FIELD_ID);

		if (!data_manager_.SetDeviceProperties(_device->GetID(), properties))
		{
			TRACE_ERROR("Failed to set device[" << _device->GetTraceName() << "] properties!");	
			return	false;
		}

		_device->ClearUpdatedProperties();
//		_device->ApplyChanges();
	}
	else
	{
		TRACE_ERROR("Failed to get device[" << _device->GetTraceName() << "] properties!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::UpdateProperties(Endpoint* _endpoint)
{
	Properties	properties;
	if (_endpoint->GetUpdatedProperties(properties))
	{
		properties.Delete(OBJECT_FIELD_ID);

		if (!data_manager_.SetEndpointProperties(_endpoint->GetID(), properties))
		{
			TRACE_ERROR("Failed to set endpoint[" << _endpoint->GetTraceName() << "] properties!");	
			return	false;
		}

		_endpoint->ClearUpdatedProperties();
//		_endpoint->ApplyChanges();
	}
	else
	{
		TRACE_ERROR("Failed to get endpoint[" << _endpoint->GetTraceName() << "] properties!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::UpdateProperties(std::string const& _id)
{
	auto device_it = device_map_.find(_id);
	if (device_it != device_map_.end())
	{
		Device* device = device_it->second;
	
		return	UpdateProperties(device);
	}

	auto endpoint_it = endpoint_map_.find(_id);
	if (endpoint_it != endpoint_map_.end())
	{
		Endpoint* endpoint = endpoint_it->second;
	
		return	UpdateProperties(endpoint);
	}

	return	false;

}

bool	ObjectManager::AddData(std::string const& _endpoint_id, Value const* _value)
{
	return	data_manager_.AddValue(_endpoint_id, _value);
}

void	ObjectManager::Preprocess()
{
	data_manager_.Start();

	while(!data_manager_.IsRunning())
	{
		usleep(TIME_MILLISECOND);
	}

	uint32_t count = data_manager_.GetDeviceCount();
	TRACE_INFO("Device Count : " << count);

	for(uint32_t i = 0 ; i < count ; i++)
	{
		std::list<Properties> properties_list;

		if (data_manager_.GetDeviceProperties(i, 1, properties_list))
		{
			for(auto it = properties_list.begin() ; it != properties_list.end() ; it++)
			{
				Device*	device = CreateDevice(*it, true);
				if (device == NULL)
				{
					TRACE_ERROR("Failed to create device!");	
				}
				else
				{
					TRACE_INFO("The device[" << device->GetTraceName() << "] created");	
				}
			}
		}
	}

	TRACE_ENTRY;
	count = data_manager_.GetEndpointCount();
	TRACE_INFO("Endpoint Count : " << count);

	for(uint32_t i = 0 ; i < count ; i++)
	{
		std::list<Properties> properties_list;

		if (data_manager_.GetEndpointProperties(i, 1, properties_list))
		{
			for(auto it = properties_list.begin() ; it != properties_list.end() ; it++)
			{
				const Property*	id_property = it->Get(OBJECT_FIELD_ID);
				if (id_property != NULL)
				{
					Endpoint* endpoint = CreateEndpoint(*it, true);						
					if (endpoint == NULL)
					{
						TRACE_ERROR("Failed to create endpoint!");	
					}
					else
					{
						TRACE_INFO("The endpoint[" << endpoint->GetTraceName() << "] created");	

						Device* device = GetDevice(endpoint->GetDeviceID());
						if (device != NULL)
						{
							if (device->Attach(endpoint->GetID()))
							{
								TRACE_INFO("The endpoint[" << endpoint->GetTraceName() << "] attached to [" << endpoint->GetDeviceID() << "]");
							
							}
						}

					}
				}
			}
		}
	}

	server_linker_.Start();

	if (auto_start_)
	{
		for(auto it = device_map_.begin(); it != device_map_.end() ; it++)
		{
			if(it->second->GetEnable())
			{
				it->second->Start();	
			}
		}
	}

	Date date = Date::GetCurrentDate() + endpoint_report_interval_;
	endpoint_report_timer_.Set(date);
}

void	ObjectManager::Process()
{
	
	if (endpoint_report_timer_.RemainTime() == 0)
	{
		for(auto it = endpoint_map_.begin(); it != endpoint_map_.end() ; it++)
		{
			if (it->second->IsRunning())
			{
				Endpoint::ValueList value_list;

				if (it->second->GetUnreportedValueList(value_list))
				{
					Message*	message = new MessageEndpointReport(it->second->GetID(), value_list);

					if (!Post(message))
					{
						TRACE_ERROR("Failed to post mesage to manager!");	

						delete message;
					}
				}
			}
		}

		endpoint_report_timer_ += endpoint_report_interval_;

	}

	ActiveObject::Process();
}

void	ObjectManager::Postprocess()
{
	server_linker_.Stop();
	ActiveObject::Postprocess();
}



bool	ObjectManager::Attach(Device* _device)
{
	auto it = device_map_.find(_device->GetID());
	if (it != device_map_.end())
	{
		TRACE_ERROR("The device[" << _device->GetTraceName() << "] has been already attached.");
		return	false;	
	}

	device_map_[_device->GetID()] = _device;

	TRACE_INFO("The device[" << _device->GetTraceName() << "] has been attached."); 
	return	true;
}

bool	ObjectManager::Detach(Device* _device)
{
	auto it = device_map_.find(_device->GetID());
	if (it == device_map_.end())
	{
		TRACE_ERROR("The device[" << _device->GetTraceName() << "] not attached.");
		return	false;	
	}

	device_map_.erase(it);

	return	true;
}

bool	ObjectManager::Attach(Endpoint* _endpoint)
{
	auto it = endpoint_map_.find(_endpoint->GetID());
	if (it != endpoint_map_.end())
	{
		TRACE_ERROR("The endpoint[" << _endpoint->GetTraceName() << "] has been already attached.");
		return	false;	
	}

	endpoint_map_[_endpoint->GetID()] = _endpoint;
	_endpoint->parent_ = this;

	std::string	device_id = _endpoint->GetDeviceID();
	if (!device_id.empty())
	{
		Device* device = GetDevice(_endpoint->GetDeviceID());
		if (device != NULL)
		{
			device->Attach(_endpoint->GetID());	
		}
	}
	TRACE_INFO("The endpoint[" << _endpoint->GetTraceName() << "] has been attached."); 

	return	true;
}

bool	ObjectManager::Detach(Endpoint* _endpoint)
{
	auto it = endpoint_map_.find(_endpoint->GetID());
	if (it == endpoint_map_.end())
	{
		TRACE_ERROR("The endpoint[" << _endpoint->GetTraceName() << "] not attached.");
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
		TRACE_ERROR("The rms[" << _rms->GetTraceName() << "] has been already attached.");
		return	false;	
	}

	rms_map_[_rms->GetID()] = _rms;
	_rms->SetObjectManager(this);

	TRACE_INFO("The rms[" << _rms->GetTraceName() << "] has been attached."); 

	return	true;
}

bool	ObjectManager::Detach(RemoteMessageServer* _rms)
{
	auto it = rms_map_.find(_rms->GetID());
	if (it == rms_map_.end())
	{
		TRACE_ERROR("The rms[" << _rms->GetTraceName() << "] not attached.");
		return	false;	
	}

	rms_map_.erase(it);
	_rms->SetObjectManager(NULL);

	return	true;
}

void	ObjectManager::OnMessage(Message* _base_message)
{
	switch(_base_message->type)
	{
	case	MSG_TYPE_KEEP_ALIVE:
		{
			MessageKeepAlive* message = dynamic_cast<MessageKeepAlive*>(_base_message);
			if (message != NULL)
			{
				SendKeepAlive(message->id);
			}

		}
		break;

	case	MSG_TYPE_ENDPOINT_UPDATED:
		{	
			MessageEndpointUpdated* message = dynamic_cast<MessageEndpointUpdated*>(_base_message);

			if (!data_manager_.AddValue(message->id, message->value))
			{
				TRACE_ERROR("Failed to add value!");	
			}
		}
		break;

	case	MSG_TYPE_ENDPOINT_REPORT:
		{
			MessageEndpointReport* message = dynamic_cast<MessageEndpointReport *>(_base_message);
			if (message != NULL)
			{
				SendEndpointReport(message->id, message->value_list);
			}
		}
		break;
	}
}


bool	ObjectManager::SendKeepAlive(ValueID const& _id)
{
	std::ostringstream	topic;

	topic << "v1_dev_" << _id;
	TRACE_INFO("Message : Keep alive " << topic.str());

	JSONNode	root;
	Date		date;
	time_t		time;

	time = time_t(date);

	root.push_back(JSONNode(MSG_FIELD_DEVICE_ID, _id));
	root.push_back(JSONNode(MSG_FIELD_CMD, MSG_COMMAND_KEEP_ALIVE));
	root.push_back(JSONNode(MSG_FIELD_TIME, time));

	return	SendMessage(topic.str(), root.write());
}

bool	ObjectManager::SendEndpointReport(ValueID const& _id, std::list<Value*> const& _value_list)
{
	std::ostringstream	topic;

	topic << "v1_ep_" << _id;
	TRACE_INFO("Message : Endpoint Report " << topic.str());

	JSONNode	root;
	Date		date;
	time_t		time;

	time = time_t(date);

	root.push_back(JSONNode(MSG_FIELD_ENDPOINT_ID, _id));
	root.push_back(JSONNode(MSG_FIELD_CMD, MSG_COMMAND_ENDPOINT_REPORT));
	root.push_back(JSONNode(MSG_FIELD_TIME, time));
	root.push_back(JSONNode(MSG_FIELD_COUNT, _value_list.size()));

	JSONNode	array(JSON_ARRAY);
	for(auto it = _value_list.begin(); it != _value_list.end() ; it++)
	{
		JSONNode	item;
		
		time = time_t((*it)->GetDate());

		item.push_back(JSONNode(MSG_FIELD_TIME, time));
		item.push_back(JSONNode(MSG_FIELD_VALUE, std::string(*(*it))));

		array.push_back(item);
	}

	array.set_name(MSG_FIELD_DATA);

	root.push_back(array);

	return	SendMessage(topic.str(), root.write());
}

bool	ObjectManager::SendMessage(std::string const& _topic, std::string const& _message)
{
	bool	ret_value = true;

	ServerLinker::UpLink*	up_link = server_linker_.GetUpLink(_topic);
	if (up_link == NULL)
	{
		TRACE_ERROR("Failed to send message because up link[" << _topic << "] not found");	
		ret_value = false;
	}
	else
	{
		if (!up_link->Send(_message))
		{
			TRACE_ERROR("Failed to send message for up link[" << _topic << "]");	
			ret_value = false;
		}
	}

	return	ret_value;
}
