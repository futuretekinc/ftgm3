#include <fstream>
#include <unistd.h>
#include "defined.h"
#include "trace.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint.h"
#include "endpoint_actuator.h"
#include "property.h"
#include "utils.h"
#include "json.h"

ObjectManager::ObjectManager()
: 	ActiveObject(), 
	server_linker_(this), 
	rcs_server_(this),
	data_manager_(DEFAULT_CONST_DB_FILE),
	endpoint_report_interval_(ENDPOINT_REPORT_INTERVAL),
	auto_start_(false)
{
	name_ 	= "object_manager";
	enable_ = true;

	trace.SetClassName(GetClassName());
}

ObjectManager::ObjectManager(ValueID const& _id)
: 	ActiveObject(_id), 
	server_linker_(this), 
	rcs_server_(this),
	data_manager_(DEFAULT_CONST_DB_FILE),
	endpoint_report_interval_(ENDPOINT_REPORT_INTERVAL),
	auto_start_(false)
{
	name_ 	= "object_manager";
	enable_ = true;

	trace.SetClassName(GetClassName());
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

bool	ObjectManager::GetAutoStart()
{
	return	auto_start_;
}

bool	ObjectManager::SetAutoStart(bool _auto_start)
{
	auto_start_ = _auto_start;

	return	true;
}

Time const&	ObjectManager::GetEndpointReportInterval() const
{
	return	endpoint_report_interval_;
}

bool	ObjectManager::	SetEndpointReportInterval(Time const& _time)
{
	endpoint_report_interval_ = _time;

	return	true;
}

bool ObjectManager::LoadFromFile(std::string const&  _file_name)
{
	bool	ret_value = false;

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

		ret_value = Load(buffer);

		delete buffer;
	}

	return	ret_value;
}

bool ObjectManager::Load(const char *_buffer)
{
	if (!libjson::is_valid(_buffer))
	{
		TRACE_ERROR("Invalid json format");
		TRACE_ERROR(_buffer);
		return	false;
	}

	JSONNode	json = libjson::parse(_buffer);

	return	Load(json);
}

bool	ObjectManager::Load(JSONNode const& _json)
{
	bool	ret_value = true;

	if ((_json.name() == TITLE_NAME_OBJECT_MANAGER) || (_json.name().size() == 0))
	{
		if (_json.type() == JSON_NODE)
		{
			for(auto it = _json.begin(); it != _json.end() ; it++)
			{
				ret_value = Load(*it);
				if(ret_value == false)
				{
					TRACE_ERROR("Failed to load field[" << it->name() << "]");
					break;
				}
			}
		}
		else
		{
			TRACE_ERROR("Failed to load because json format is invalid.");
			ret_value = false;	
		}
	}
	else if (_json.name() == TITLE_NAME_ID)
	{
		id_ = _json.as_string();
	}
	else if (_json.name() == TITLE_NAME_NAME)
	{
		name_ = _json.as_string();
	}
	else if (_json.name() == TITLE_NAME_AUTO_START)
	{
		if ((_json.as_string() == "yes") ||  (_json.as_string() == "on"))
		{
			auto_start_ = true;
		}
		else if ((_json.as_string() == "no") ||  (_json.as_string() == "off"))
		{
			auto_start_ = false;
		}
	}
	else if (_json.name() == TITLE_NAME_ENDPOINT_REPORT_INTERVAL)
	{
		endpoint_report_interval_ = _json.as_int();
	}
	else if (_json.name() == TITLE_NAME_DEVICE)
	{
		if (_json.type() == JSON_ARRAY)
		{
			for(auto it = _json.begin(); it != _json.end() ; it++)
			{
				Device* device = CreateDevice(*it);
				if (device == NULL)
				{
					ret_value = false;
					break;
				}
			}
		}
		else if (_json.type() == JSON_NODE)
		{
			Device* device = CreateDevice(_json);
			if (device == NULL)
			{
				ret_value = false;
			}
		}
	}
	else if (_json.name() == TITLE_NAME_DATA_MANAGER)
	{
		ret_value = data_manager_.Load(_json);
	}
	else if (_json.name() == TITLE_NAME_SERVER_LINKER)
	{
		ret_value = server_linker_.Load(_json);
	}
	else if (_json.name() == TITLE_NAME_RCS_SERVER)
	{
		ret_value = rcs_server_.Load(_json);
	}
	else if (_json.name() == TITLE_NAME_TRACE)
	{
		ret_value = trace_master.Load(_json);
	}
	else
	{
		TRACE_ERROR("Undefined field[" << _json.name() << "]");
		ret_value = false;
	}

	return	ret_value;
}

ObjectManager::operator JSONNode() const
{
	JSONNode	root, item;

	root.push_back(JSONNode(TITLE_NAME_ENDPOINT_REPORT_INTERVAL, endpoint_report_interval_));
	root.push_back(JSONNode(TITLE_NAME_AUTO_START, (auto_start_)?"yes":"no"));

	item = JSONNode(data_manager_);
	item.set_name(TITLE_NAME_DATA_MANAGER);
	root.push_back(item);

	item = JSONNode(server_linker_);
	item.set_name(TITLE_NAME_SERVER_LINKER);
	root.push_back(item);

	item = JSONNode(rcs_server_);
	item.set_name(TITLE_NAME_RCS_SERVER);
	root.push_back(item);

	item = JSONNode(trace_master);
	item.set_name(TITLE_NAME_TRACE);
	root.push_back(item);

	return	root;
}

uint32_t	ObjectManager::GetNodeCount()
{
	return	gateway_map_.size() + device_map_.size() + endpoint_map_.size();
}

uint32_t	ObjectManager::GetNodeList(std::list<Node*>& _list)
{
	for(auto it = gateway_map_.begin(); it != gateway_map_.end() ; it++)
	{
		_list.push_back(it->second);
	}

	for(auto it = device_map_.begin(); it != device_map_.end() ; it++)
	{
		_list.push_back(it->second);
	}

	for(auto it = endpoint_map_.begin(); it != endpoint_map_.end() ; it++)
	{
		_list.push_back(it->second);
	}

	return	_list.size();
}

Node*		ObjectManager::GetNode(std::string const& _id)
{
	Object*	object = NULL;

	object = GetGateway(_id);
	if (object == 0)
	{
		object = GetDevice(_id);
		if (object == 0)
		{
			object = GetEndpoint(_id);
		}
	}

	return	dynamic_cast<Node*>(object);
}

Gateway*	ObjectManager::CreateGateway(JSONNode const& _json)
{
	Gateway*	gateway = NULL;

	if (_json.type() == JSON_NODE)
	{
		Properties	properties;

		properties.Append(_json);

		TRACE_INFO(properties);
		gateway = CreateGateway(properties);
		if (gateway == NULL)
		{
			TRACE_ERROR("Failed to create gateway!");
		}
	}

	return	gateway;
}

Gateway*		ObjectManager::CreateGateway(Properties const& _properties, bool from_db)
{
	Gateway*		gateway = NULL;

	gateway = Gateway::Create(*this, _properties);
	if (gateway != NULL)
	{
		if (!Attach(gateway))
		{
			delete gateway;	
			return	NULL;
		}

		if (!from_db)
		{
			data_manager_.AddGateway(gateway);
		}

		gateway->ClearUpdatedProperties();

		const Property*	device_property = _properties.Get(TITLE_NAME_DEVICE);
		if (device_property)
		{
			const ValuePropertiesList* properties_list_value = dynamic_cast<const ValuePropertiesList*>(device_property->GetValue());
			if (properties_list_value != NULL)
			{
				const PropertiesList& 	properties_list = properties_list_value->Get();
				for(auto it = properties_list.begin(); it != properties_list.end() ; it++)
				{
					Properties properties(*it);

					properties.Delete(TITLE_NAME_PARENT_ID);

					properties.AppendParentID(gateway->GetID());

					TRACE_INFO(properties);
					Device* device = CreateDevice(properties);
					if (device == NULL)
					{
						TRACE_ERROR("Failed to create device!");
						break;
					}
				}
			}

		}

		TRACE_INFO("The gateway[" << gateway->GetID() << "] created!");
	}

	return	gateway;
}

bool		ObjectManager::DestroyGateway(std::string const& _id)
{
	Gateway* gateway = GetGateway(_id);
	if (gateway == NULL)
	{
		TRACE_ERROR("Failed to delete device because the device[" << _id << "] coult not be found.");
		return	false;	
	}

	Detach(gateway);

	delete gateway;

	return	true;
}

uint32_t	ObjectManager::GetGatewayCount()
{
	return	gateway_map_.size();
}

uint32_t	ObjectManager::GetGatewayList(std::list<Gateway*>& _gateway_list)
{
	for(auto it = gateway_map_.begin(); it != gateway_map_.end() ; it++)
	{
		_gateway_list.push_back(it->second);	
	}

	return	_gateway_list.size();
}

uint32_t	ObjectManager::GetGatewayList(ValueType const& _type, std::list<Gateway*>& _gateway_list)
{
	for(auto it = gateway_map_.begin(); it != gateway_map_.end() ; it++)
	{
		if (it->second->IsIncludedIn(_type))
		{
			_gateway_list.push_back(it->second);	
		}
	}

	return	_gateway_list.size();
}

Gateway*		ObjectManager::GetGateway(std::string const& _id)
{
	auto it = gateway_map_.find(_id);
	if (it != gateway_map_.end())
	{
		return	it->second;	
	}

	TRACE_ERROR("Gateway[" << _id << "] not found!");
	return	NULL;
}

Device*	ObjectManager::CreateDevice(JSONNode const& _json)
{
	Device*	device = NULL;

	if (_json.type() == JSON_NODE)
	{
		Properties	properties;

		properties.Append(_json);

		TRACE_INFO(properties);
		device = CreateDevice(properties);
		if (device == NULL)
		{
			TRACE_ERROR("Failed to create device!");
		}
	}

	return	device;
}

Device*		ObjectManager::CreateDevice(Properties const& _properties, bool from_db)
{
	Device*		device = NULL;

	device = Device::Create(*this, _properties);
	if (device != NULL)
	{
		Attach(device);	

		if (!from_db)
		{
			data_manager_.AddDevice(device);
		}

		device->ClearUpdatedProperties();

		const Property*	endpoint_property = _properties.Get(TITLE_NAME_ENDPOINT);
		if (endpoint_property)
		{
			const ValuePropertiesList* properties_list_value = dynamic_cast<const ValuePropertiesList*>(endpoint_property->GetValue());
			if (properties_list_value != NULL)
			{
				const PropertiesList& 	properties_list = properties_list_value->Get();
				for(auto it = properties_list.begin(); it != properties_list.end() ; it++)
				{
					Properties properties(*it);

					properties.Delete(TITLE_NAME_PARENT_ID);

					properties.AppendParentID(device->GetID());

					Endpoint* endpoint = CreateEndpoint(properties);
					if (endpoint == NULL)
					{
						TRACE_ERROR("Failed to create endpoint!");
						break;
					}
				}
			}
		}
		TRACE_INFO("The device[" << device->GetID() << "] crated!");

		Gateway*	gateway = GetGateway(device->GetParentID());
		if (gateway != NULL)
		{
			if (!gateway->Attach(device->GetID()))
			{
				TRACE_ERROR("Failed to attach device[" << device->GetTraceName() << "]");
			}
		}
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

Endpoint*	ObjectManager::CreateEndpoint(JSONNode const& _properties, bool from_db)
{
	Properties	properties;

	properties.Append(_properties);

	return	CreateEndpoint(properties, from_db);
}

Endpoint*	ObjectManager::CreateEndpoint(Properties const& _properties, bool from_db)
{
	TRACE_INFO(_properties);

	Endpoint*	endpoint = Endpoint::Create(*this, _properties);		
	if (endpoint != NULL)
	{
		Attach(endpoint);

		if (!from_db)
		{
			data_manager_.AddEndpoint(endpoint);
		}

		endpoint->ClearUpdatedProperties();

		Device*	device = GetDevice(endpoint->GetParentID());
		if (device != NULL)
		{
			if (!device->Attach(endpoint->GetID()))
			{
				TRACE_ERROR("Failed to attach endpoint[" << endpoint->GetTraceName() << "]");
			}
		}
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
		Device*	device = GetDevice(endpoint->GetParentID());
		if (device != NULL)
		{
			device->Detach(_endpoint_id);	
		}
		
		auto it = endpoint_map_.find(endpoint->GetID());
		if (it != endpoint_map_.end())
		{
			endpoint_map_.erase(it);
		}

		Detach(endpoint);

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

bool	ObjectManager::IDChanged(Node* _node, ValueID const& _old_id)
{
	if (dynamic_cast<Gateway*>(_node))
	{
		return	IDChanged(dynamic_cast<Gateway*>(_node), _old_id);
	}
	else if (dynamic_cast<Device*>(_node))
	{
		return	IDChanged(dynamic_cast<Device*>(_node), _old_id);
	}
	else if (dynamic_cast<Endpoint*>(_node))
	{
		return	IDChanged(dynamic_cast<Endpoint*>(_node), _old_id);
	}

	return	false;
}

bool	ObjectManager::IDChanged(Gateway* _gateway, ValueID const& _old_id)
{
	auto it = gateway_map_.find(std::string(_old_id));
	if (it != gateway_map_.end())
	{
		gateway_map_.erase(it);
		gateway_map_[_gateway->GetID()] = _gateway;

		return	true;	
	}

	return	false;
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
	Gateway* gateway= dynamic_cast<Gateway*>(_object);
	if (gateway != NULL)
	{
		return	UpdateProperties(gateway);
	}

	Device* device = dynamic_cast<Device*>(_object);
	if (device != NULL)
	{
		return	UpdateProperties(device);
	}

	Endpoint* endpoint= dynamic_cast<Endpoint*>(_object);
	if (endpoint != NULL)
	{
		return	UpdateProperties(endpoint);
	}

	return	false;
}

bool	ObjectManager::UpdateProperties(Gateway* _gateway)
{
	Properties	properties;
	if (_gateway->GetUpdatedProperties(properties))
	{
		properties.Delete(TITLE_NAME_ID);

		if (!data_manager_.SetGatewayProperties(_gateway->GetID(), properties))
		{
			TRACE_ERROR("Failed to set gateway[" << _gateway->GetTraceName() << "] properties!");	
			return	false;
		}

		_gateway->ClearUpdatedProperties();
	}
	else
	{
		TRACE_ERROR("Failed to get gateway[" << _gateway->GetTraceName() << "] properties!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::UpdateProperties(Device* _device)
{
	Properties	properties;
	if (_device->GetUpdatedProperties(properties))
	{
		properties.Delete(TITLE_NAME_ID);

		if (!data_manager_.SetDeviceProperties(_device->GetID(), properties))
		{
			TRACE_ERROR("Failed to set device[" << _device->GetTraceName() << "] properties!");	
			return	false;
		}

		_device->ClearUpdatedProperties();
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
		properties.Delete(TITLE_NAME_ID);

		if (!data_manager_.SetEndpointProperties(_endpoint->GetID(), properties))
		{
			TRACE_ERROR("Failed to set endpoint[" << _endpoint->GetTraceName() << "] properties!");	
			return	false;
		}

		_endpoint->ClearUpdatedProperties();
	}
	else
	{
		TRACE_ERROR("Failed to get endpoint[" << _endpoint->GetTraceName() << "] properties!");
		return	false;
	}

	return	true;
}

void	ObjectManager::Preprocess()
{
	data_manager_.Start();

	while(!data_manager_.IsRunning())
	{
		usleep(TIME_MILLISECOND);
	}

	rcs_server_.Start();
	while(!rcs_server_.IsRunning())
	{
		usleep(TIME_MILLISECOND);
	}


	uint32_t count = data_manager_.GetGatewayCount();
	TRACE_INFO("Gateway Count : " << count);

	for(uint32_t i = 0 ; i < count ; i++)
	{
		std::list<Properties> properties_list;

		if (data_manager_.GetGatewayProperties(i, 1, properties_list))
		{
			for(auto it = properties_list.begin() ; it != properties_list.end() ; it++)
			{
				Gateway*	gateway = CreateGateway(*it, true);
				if (gateway == NULL)
				{
					TRACE_ERROR("Failed to create gateway!");	
				}
				else
				{
					TRACE_INFO("The gateway[" << gateway->GetTraceName() << "] created");	
				}
			}
		}
	}

	count = data_manager_.GetDeviceCount();
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

	count = data_manager_.GetEndpointCount();
	TRACE_INFO("Endpoint Count : " << count);

	for(uint32_t i = 0 ; i < count ; i++)
	{
		std::list<Properties> properties_list;

		if (data_manager_.GetEndpointProperties(i, 1, properties_list))
		{
			for(auto it = properties_list.begin() ; it != properties_list.end() ; it++)
			{
				const Property*	id_property = it->Get(TITLE_NAME_ID);
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

						Device* device = GetDevice(endpoint->GetParentID());
						if (device != NULL)
						{
							if (device->Attach(endpoint->GetID()))
							{
								TRACE_INFO("The endpoint[" << endpoint->GetTraceName() << "] attached to [" << endpoint->GetParentID() << "]");
							
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
		for(auto it = gateway_map_.begin(); it != gateway_map_.end() ; it++)
		{
			if(it->second->GetEnable())
			{
				it->second->Start();	
			}
		}

		for(auto it = device_map_.begin(); it != device_map_.end() ; it++)
		{
			if(it->second->GetEnable())
			{
				it->second->Start();	
			}
		}

		for(auto it = endpoint_map_.begin(); it != endpoint_map_.end() ; it++)
		{
			if(it->second->GetEnable())
			{
				it->second->Start();	
			}
		}
	}

	Date date = Date::GetCurrent() + Time(endpoint_report_interval_ * TIME_SECOND);
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
				server_linker_.ReportEPData(it->second);
			}
		}

		endpoint_report_timer_ += Time(endpoint_report_interval_ * TIME_SECOND);

	}

	ActiveObject::Process();
}

void	ObjectManager::Postprocess()
{
	rcs_server_.Stop(true);
	server_linker_.Stop();
	ActiveObject::Postprocess();
}

bool	ObjectManager::Attach(Gateway* _gateway)
{
	auto it = gateway_map_.find(_gateway->GetID());
	if (it != gateway_map_.end())
	{
		TRACE_ERROR("The gateway[" << _gateway->GetTraceName() << "] has been already attached.");
		return	false;	
	}

	gateway_map_[_gateway->GetID()] = _gateway;

	server_linker_.AddDownLink(GetTopicNameGateway(_gateway->GetID()));

	TRACE_INFO("The gateway[" << _gateway->GetTraceName() << "] has been attached."); 
	return	true;
}

bool	ObjectManager::Detach(Gateway* _gateway)
{
	auto it = gateway_map_.find(_gateway->GetID());
	if (it == gateway_map_.end())
	{
		TRACE_ERROR("The gateway[" << _gateway->GetTraceName() << "] not attached.");
		return	false;	
	}

	server_linker_.DelDownLink(GetTopicNameGateway(_gateway->GetID()));

	gateway_map_.erase(it);

	return	true;
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

	server_linker_.AddDownLink(GetTopicNameDevice(_device->GetID()));

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

	server_linker_.DelDownLink(GetTopicNameDevice(_device->GetID()));

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

	std::string	device_id = _endpoint->GetParentID();
	if (!device_id.empty())
	{
		Device* device = GetDevice(device_id);
		if (device != NULL)
		{
			device->Attach(_endpoint->GetID());	
		}
	}

	server_linker_.AddDownLink(GetTopicNameEndpoint(_endpoint->GetID()));

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

	server_linker_.DelDownLink(GetTopicNameEndpoint(_endpoint->GetID()));

	endpoint_map_.erase(it);

	return	true;
}

bool	ObjectManager::Attach(RCSSession* _session)
{
	auto it = rms_map_.find(_session->GetID());
	if (it != rms_map_.end())
	{
		TRACE_ERROR("The rms[" << _session->GetTraceName() << "] has been already attached.");
		return	false;	
	}

	rms_map_[_session->GetID()] = _session;

	TRACE_INFO("The rms[" << _session->GetTraceName() << "] has been attached."); 

	return	true;
}

bool	ObjectManager::Detach(RCSSession* _session)
{
	auto it = rms_map_.find(_session->GetID());
	if (it == rms_map_.end())
	{
		TRACE_ERROR("The rms[" << _session->GetTraceName() << "] not attached.");
		return	false;	
	}

	rms_map_.erase(it);

	return	true;
}

bool	ObjectManager::OnMessage(Message* _base_message)
{
	switch(_base_message->GetType())
	{
	case	MSG_TYPE_ENDPOINT_UPDATED:
		{	
			MessageEndpointUpdated* message = dynamic_cast<MessageEndpointUpdated*>(_base_message);

			if (!data_manager_.AddValue(message->endpoint_id, message->value))
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
				Endpoint*	endpoint = GetEndpoint(message->endpoint_id);
				if (endpoint != NULL)
				{
					server_linker_.ReportEPData(endpoint);
				}
			}
		}
		break;

	default:
		{
			return	ActiveObject::OnMessage(_base_message);
		}

	}

	return	true;
}


bool	ObjectManager::KeepAlive(Node* _object)
{
	JSONNode	properties;

	_object->GetProperties(properties, PROPERTY_ID + PROPERTY_TIME_OF_EXPIRE);

	RCSMessage	message(MSG_TYPE_RCS_KEEP_ALIVE);

	if (dynamic_cast<Gateway*>(_object))
	{
		message.AddGateway(properties);
	}
	else if (dynamic_cast<Device*>(_object))
	{
		message.AddDevice(properties);
	}
	else if (dynamic_cast<Endpoint*>(_object))
	{
		message.AddEndpoint(properties);
	}

	return	server_linker_.Send(message);
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

std::string	ObjectManager::GetTopicNameGateway(std::string const& _id)
{
	std::ostringstream	topic;

	topic << DEFAULT_CONST_MSG_VERSION << "_gw_" << _id;

	return	topic.str();
}

std::string	ObjectManager::GetTopicNameDevice(std::string const& _id)
{
	std::ostringstream	topic;

	topic << DEFAULT_CONST_MSG_VERSION << "_dev_" << _id;

	return	topic.str();
}

std::string	ObjectManager::GetTopicNameEndpoint(std::string const& _id)
{
	std::ostringstream	topic;

	topic << DEFAULT_CONST_MSG_VERSION << "_ep_" << _id;

	return	topic.str();
}


#if 0
bool	ObjectManager::RemoteServiceCall(RCSMessage& _request, RCSMessage& _response)
{
	bool	result = false;

	try
	{
		if (_request.GetMsgType() == MSG_TYPE_RCS_ADD)
		{
			result = RCSAdd(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_DEL)
		{
			result = RCSDel(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_GET)
		{
			result = RCSGet(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_SET)
		{
			result = RCSSet(_request, _response);	
		}
		else if (_request.GetMsgType() == MSG_TYPE_RCS_LIST)
		{
			result = RCSList(_request, _response);	
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

bool	ObjectManager::RCSAdd(RCSMessage& _request, RCSMessage& _response)
{
	JSONNode	payload  = _request.GetPayload();
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);

	for(auto it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			JSONNode	result(JSON_NODE);

			Gateway*	gateway = CreateGateway(*it);
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

			Device*	device = CreateDevice(*it);
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

			Endpoint*	endpoint = CreateEndpoint(*it);
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

bool	ObjectManager::RCSDel(RCSMessage& _request, RCSMessage& _response)
{
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);
	JSONNode	payload  = _request.GetPayload();

	for(auto it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			JSONNode	result(JSON_NODE);
			std::string id = JSONNodeGetID(*it);
			
			if (DestroyGateway(id))
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

			if (DestroyDevice(id))
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

			if (DestroyEndpoint(id))
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
				Endpoint*	endpoint = GetEndpoint(id);
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

bool	ObjectManager::RCSSet(RCSMessage& _request, RCSMessage& _response)
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
			Gateway*	gateway = GetGateway(id);
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
			Device*	device = GetDevice(id);
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
			Endpoint*	endpoint= GetEndpoint(id);
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

				Endpoint*	endpoint = GetEndpoint(id);
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

bool	ObjectManager::RCSGet(RCSMessage& _request, RCSMessage& _response)
{
	JSONNode	payload  = _request.GetPayload();
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);

	for(auto it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			JSONNode	result;
			std::string	id = JSONNodeGetID(*it);

			Gateway*	gateway = GetGateway(id);
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

			Device*	device = GetDevice(id);
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

			Endpoint*	endpoint= GetEndpoint(id);
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

bool	ObjectManager::RCSList(RCSMessage& _request, RCSMessage& _response)
{
	JSONNode	payload  = _request.GetPayload();
	RCSMessage	response(MSG_TYPE_RCS_CONFIRM);

	for(auto it = payload.begin(); it != payload.end() ; it++)
	{
		if (it->name() == TITLE_NAME_GATEWAY)
		{
			if (gateway_map_.size() != 0)
			{
				for(auto gateway = gateway_map_.begin(); gateway != gateway_map_.end() ; gateway++)
				{
					JSONNode	result;

					gateway->second->GetProperties(result, PROPERTY_ALL);

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
			if (device_map_.size() != 0)
			{
				for(auto device = device_map_.begin(); device != device_map_.end() ; device++)
				{
					JSONNode	result;

					device->second->GetProperties(result, PROPERTY_ALL);

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
			if (endpoint_map_.size() != 0)
			{
				for(auto endpoint = endpoint_map_.begin(); endpoint != endpoint_map_.end() ; endpoint++)
				{
					JSONNode	result;

					endpoint->second->GetProperties(result, PROPERTY_ALL);

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

bool	ObjectManager::RCSConfirm(RCSMessage& _reply, std::string& _req_type)
{
	bool		ret_value = false;
	JSONNode	reply_payload  = _reply.GetPayload();

	try
	{
		for(auto it = reply_payload.begin(); it != reply_payload.end() ; it++)
		{
			if (it->name() == TITLE_NAME_GATEWAY)
			{
				RCSConfirmGateway(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_DEVICE)
			{
				RCSConfirmDevice(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_ENDPOINT)
			{
				RCSConfirmEndpoint(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_DATA)
			{
				RCSConfirmData(*it, _req_type);
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

bool	ObjectManager::RCSError(RCSMessage& _reply)
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

bool	ObjectManager::RCSError(RCSMessage& _reply, std::string& _req_type)
{
	bool		ret_value = false;
	JSONNode	reply_payload  = _reply.GetPayload();

	try
	{
		for(auto it = reply_payload.begin(); it != reply_payload.end() ; it++)
		{
			if (it->name() == TITLE_NAME_GATEWAY)
			{
				RCSConfirmGateway(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_DEVICE)
			{
				RCSConfirmDevice(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_ENDPOINT)
			{
				RCSConfirmEndpoint(*it, _req_type);
			}
			else if (it->name() == TITLE_NAME_DATA)
			{
				RCSConfirmData(*it, _req_type);
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

bool	ObjectManager::RCSSetGateway(JSONNode& _node, JSONNode& _result)
{
	TRACE_ENTRY;
	if (_node.type() == JSON_NODE)
	{
		std::string	id;
		if (!GetValue(_node, TITLE_NAME_ID, id))
		{
			TRACE_ERROR("Invalid _message fromat!");
			return	false;
		}

		Gateway*	gateway = GetGateway(id);
		if (gateway == NULL)
		{
			TRACE_ERROR("Object[" << id << "not found!");
			return	false;
		}

		if (!gateway->SetProperties(_node))
		{
			TRACE_ERROR("Invalid _message fromat!");
			return	false;
		}

		AddGatewayToMessage(_result, gateway, PROPERTY_ALL);
	}
	else if (_node.type() == JSON_ARRAY)
	{
		for(auto it = _node.begin() ; it != _node.end() ; it++)
		{
			RCSSetGateway(*it, _result);
		}
	}
	else
	{
		TRACE_ERROR("Invalid _message fromat!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::RCSConfirmGateway(JSONNode& _node, std::string& _req_type)
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
			Gateway*	gateway = GetGateway(id);
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
			RCSConfirmGateway(*it, _req_type);
		}
	}
	else
	{
		throw InvalidArgument("Invalid _message fromat!");
	}
}

bool	ObjectManager::RCSSetDevice(JSONNode& _node, JSONNode& _reply)
{
	if (_node.type() == JSON_NODE)
	{
		std::string	id;
		if (!GetValue(_node, TITLE_NAME_ID, id))
		{
			TRACE_ERROR("Invalid message fromat!");
			return	false;
		}

		Device*	device =GetDevice(id);
		if (device == NULL)
		{
			TRACE_ERROR("Object[" << id << "not found!");
			return	false;
		}

		if (!device->SetProperties(_node))
		{
			TRACE_ERROR("Invalid message fromat!");
			return	false;
		}

		AddDeviceToMessage(_reply, device, PROPERTY_ALL);
	}
	else if (_node.type() == JSON_ARRAY)
	{
		for(auto it = _node.begin() ; it != _node.end() ; it++)
		{
			RCSSetDevice(*it, _reply);
		}
	}
	else
	{
		TRACE_ERROR("Invalid message fromat!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::RCSConfirmDevice(JSONNode& _node, std::string& _req_type)
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
			Device*	device = GetDevice(id);
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
			RCSConfirmDevice(*it, _req_type);
		}
	}
	else
	{
		TRACE_ERROR("Invalid _message fromat!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::RCSSetEndpoint(JSONNode& _node, JSONNode& _reply)
{
	if (_node.type() == JSON_NODE)
	{
		std::string	id;
		if (!GetValue(_node, TITLE_NAME_ID, id))
		{
			TRACE_ERROR("Invalid message fromat!");
			return	false;
		}

		Endpoint*	endpoint = GetEndpoint(id);
		if (endpoint == NULL)
		{
			TRACE_ERROR("Object[" << id << "] not found!");
			return	false;
		}

		if (!endpoint->SetProperties(_node))
		{
			throw InvalidArgument("Invalid argument!");
		}

		AddEndpointToMessage(_reply, endpoint, PROPERTY_ALL);
	}
	else if (_node.type() == JSON_ARRAY)
	{
		for(auto it = _node.begin() ; it != _node.end() ; it++)
		{
			RCSSetEndpoint(*it, _reply);
		}
	}
	else
	{
		TRACE_ERROR("Invalid _message fromat!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::RCSConfirmEndpoint(JSONNode& _node, std::string& _req_type)
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
			Endpoint*	endpoint = GetEndpoint(id);
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
			RCSConfirmEndpoint(*it, _req_type);
		}
	}
	else
	{
		TRACE_ERROR("Invalid _message fromat!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::RCSGetData(JSONNode& _node, JSONNode& _reply)
{
	if (_node.type() == JSON_NODE)
	{
		std::string	id;
		if (!GetValue(_node, TITLE_NAME_ID, id))
		{
			TRACE_ERROR("Invalid _message fromat!");
			return	false;
		}

		Endpoint*	endpoint = GetEndpoint(id);
		if (endpoint == NULL)
		{
			TRACE_ERROR("Object[" << id << "] not found!");
			return	false;
		}

		uint32_t	start_time = 0;
		if (!GetValue(_node, TITLE_NAME_START_TIME, start_time, true))
		{
			throw InvalidArgument("Invalid message fromat!");
		}

		uint32_t	end_time = 0;
		if (!GetValue(_node, TITLE_NAME_END_TIME, end_time, true))
		{
			throw InvalidArgument("Invalid message fromat!");
		}

		AddEPDataToMessage(_reply, endpoint, start_time, end_time);
	}
	else if (_node.type() == JSON_ARRAY)
	{
		for(auto it = _node.begin() ; it != _node.end() ; it++)
		{
			RCSGetData(*it, _reply);
		}
	}
	else
	{
		TRACE_ERROR("Invalid _message fromat!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::RCSSetData(JSONNode& _node, JSONNode& _reply)
{
	if (_node.type() == JSON_NODE)
	{
		std::string	id;
		if (!GetValue(_node, TITLE_NAME_ID, id))
		{
			TRACE_ERROR("Invalid _message fromat!");
			return	false;
		}

		Endpoint*	endpoint = GetEndpoint(id);
		if (endpoint == NULL)
		{
			TRACE_ERROR("Object[" << id << "] not found!");
			return	false;
		}

		EndpointActuator* actuator = dynamic_cast<EndpointActuator*>(endpoint);
		if (actuator == NULL)
		{
			TRACE_ERROR("It is not an actuator!");
			return	false;
		}

		Date	time_of_expire = 0;
		if (!GetValue(_node, TITLE_NAME_TIME_OF_EXPIRE, time_of_expire, true))
		{
			TRACE_ERROR("Invalid _message fromat!");
			return	false;
		}

		std::string	value = 0;
		if (!GetValue(_node, TITLE_NAME_VALUE, value))
		{
			TRACE_ERROR("Invalid _message fromat!");
			return	false;
		}
	
		if (actuator->Set(value))
		{
			AddEPDataToMessage(_reply, endpoint);	
		}
	}
	else if (_node.type() == JSON_ARRAY)
	{
		for(auto it = _node.begin() ; it != _node.end() ; it++)
		{
			RCSSetData(*it, _reply);
		}
	}
	else
	{
		TRACE_ERROR("Invalid _message fromat!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::RCSConfirmData(JSONNode& _node, std::string& _req_type)
{
	if (_node.type() == JSON_NODE)
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
			Endpoint*	endpoint = GetEndpoint(id);
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
	}
	else if (_node.type() == JSON_ARRAY)
	{
		for(auto it = _node.begin() ; it != _node.end() ; it++)
		{
			RCSConfirmEndpoint(*it, _req_type);
		}
	}
	else
	{
		TRACE_ERROR("Invalid _message fromat!");
		return	false;
	}

	return	false;
}

bool	ObjectManager::AddNodeToMessage(JSONNode& _payload, Node* _node, Properties::Fields const& _fields)
{
	if (dynamic_cast<Gateway *>(_node))
	{
		return	AddGatewayToMessage(_payload, dynamic_cast<Gateway*>(_node), _fields);
	}
	else if (dynamic_cast<Device *>(_node))
	{
		return	AddDeviceToMessage(_payload, dynamic_cast<Device*>(_node), _fields);
	}
	else if (dynamic_cast<Endpoint*>(_node))
	{
		return	AddEndpointToMessage(_payload, dynamic_cast<Endpoint*>(_node), _fields);
	}
	
	TRACE_ERROR("Invalid node");

	return	false;
}

bool	ObjectManager::AddGatewayToMessage(JSONNode& _payload, Gateway* _gateway, Properties::Fields const& _fields)
{
	auto it = _payload.find(TITLE_NAME_GATEWAY);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	gateway_array(JSON_ARRAY);		

			gateway_array.set_name(TITLE_NAME_GATEWAY);
			gateway_array.push_back(*it);

			JSONNode	properties;
			_gateway->GetProperties(properties, _fields);

			gateway_array.push_back(properties);

			_payload.erase(it);

			_payload.push_back(gateway_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode	properties;

			_gateway->GetProperties(properties, _fields);
			it->push_back(properties);
		}
	}
	else
	{
		JSONNode	properties;
		_gateway->GetProperties(properties, _fields);

		properties.set_name(TITLE_NAME_GATEWAY);
		_payload.push_back(properties);
	}

	return	true;
}

bool	ObjectManager::AddGatewayToMessage(JSONNode& _payload, std::string const& _id)
{
	auto it = _payload.find(TITLE_NAME_GATEWAY);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	gateway_array(JSON_ARRAY);		

			gateway_array.set_name(TITLE_NAME_GATEWAY);
			gateway_array.push_back(*it);

			JSONNode	node;
			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			gateway_array.push_back(node);

			_payload.erase(it);

			_payload.push_back(gateway_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode	node;

			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			it->push_back(node);
		}
	}
	else
	{
		JSONNode	node;
		
		node.push_back(JSONNode(TITLE_NAME_ID, _id));	
		node.set_name(TITLE_NAME_GATEWAY);
		_payload.push_back(JSONNode(node));
	}

	return	true;
}

bool	ObjectManager::AddDeviceToMessage(JSONNode& _payload, Device* _device, Properties::Fields const& _fields)
{
	auto it = _payload.find(TITLE_NAME_DEVICE);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	device_array(JSON_ARRAY);		

			device_array.set_name(TITLE_NAME_DEVICE);
			device_array.push_back(*it);

			JSONNode	properties;
			_device->GetProperties(properties, _fields);

			device_array.push_back(properties);

			_payload.erase(it);

			_payload.push_back(device_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode	properties;
			_device->GetProperties(properties, _fields);

			it->push_back(properties);
		}
	}
	else
	{
		JSONNode	properties;
		_device->GetProperties(properties, _fields);
		properties.set_name(TITLE_NAME_DEVICE);
		_payload.push_back(properties);
	}

	return	true;
}

bool	ObjectManager::AddDeviceToMessage(JSONNode& _payload, std::string const& _id)
{
	auto it = _payload.find(TITLE_NAME_DEVICE);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	device_array(JSON_ARRAY);		

			device_array.set_name(TITLE_NAME_DEVICE);
			device_array.push_back(*it);

			JSONNode	node;
			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			device_array.push_back(node);

			_payload.erase(it);

			_payload.push_back(device_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode	node;
			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			it->push_back(node);
		}
	}
	else
	{
		JSONNode	node;
		
		node.push_back(JSONNode(TITLE_NAME_ID, _id));	
		node.set_name(TITLE_NAME_DEVICE);
		_payload.push_back(node);
	}

	return	true;
}

bool	ObjectManager::AddEndpointToMessage(JSONNode& _payload, Endpoint* _endpoint, Properties::Fields const& _fields)
{
	auto it = _payload.find(TITLE_NAME_ENDPOINT);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	endpoint_array(JSON_ARRAY);		
			endpoint_array.set_name(TITLE_NAME_ENDPOINT);
			endpoint_array.push_back(*it);

			JSONNode	properties;
			_endpoint->GetProperties(properties, _fields);
			endpoint_array.push_back(properties);

			_payload.erase(it);

			_payload.push_back(endpoint_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode	properties;
			_endpoint->GetProperties(properties, _fields);

			it->push_back(properties);
		}
	}
	else
	{
		JSONNode	properties;
		_endpoint->GetProperties(properties, _fields);

		properties.set_name(TITLE_NAME_ENDPOINT);
		_payload.push_back(properties);
	}

	return	true;
}

bool	ObjectManager::AddEndpointToMessage(JSONNode& _payload, std::string const& _id)
{
	auto it = _payload.find(TITLE_NAME_ENDPOINT);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	endpoint_array(JSON_ARRAY);		

			endpoint_array.set_name(TITLE_NAME_ENDPOINT);
			endpoint_array.push_back(*it);

			JSONNode	node;

			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			endpoint_array.push_back(node);

			_payload.erase(it);

			_payload.push_back(endpoint_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode node;
			
			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			it->push_back(node);
		}
	}
	else
	{
		JSONNode	node;
		
		node.push_back(JSONNode(TITLE_NAME_ID, _id));	
		node.set_name(TITLE_NAME_ENDPOINT);
		_payload.push_back(node);
	}

	return	true;
}

bool	ObjectManager::AddEPDataToMessage(JSONNode& _payload, Endpoint* _ep)
{
	JSONNode	node;

	Endpoint::ValueList value_list;

	TRACE_INFO("Add EP Data!");
	if (!_ep->GetUnreportedValueList(value_list))
	{
		TRACE_WARN("Failed to get unreported value list.");
		return	false;
	}

	node.push_back(JSONNode(TITLE_NAME_ID, _ep->GetID()));
	node.push_back(JSONNode(TITLE_NAME_COUNT, value_list.size()));

	JSONNode	array(JSON_ARRAY);
	for(auto it = value_list.begin(); it != value_list.end() ; it++)
	{
		JSONNode	item;
		
		item.push_back(JSONNode(TITLE_NAME_TIME, time_t((*it)->GetDate())));
		item.push_back(JSONNode(TITLE_NAME_VALUE, std::string(*(*it))));

		array.push_back(item);
	}

	array.set_name(TITLE_NAME_VALUE);
	node.push_back(array);

	auto it = _payload.find(TITLE_NAME_DATA);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	endpoint_array(JSON_ARRAY);		

			endpoint_array.set_name(TITLE_NAME_DATA);
			endpoint_array.push_back(*it);
			endpoint_array.push_back(node);

			_payload.erase(it);

			_payload.push_back(endpoint_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			it->push_back(node);
		}
	}
	else
	{
		node.set_name(TITLE_NAME_DATA);
		_payload.push_back(node);
	}

	return	true;	
}

bool	ObjectManager::AddEPDataToMessage(JSONNode& _payload, Endpoint* _ep, uint32_t _lower_bound, uint32_t _upper_bound)
{
	JSONNode	node;
	Endpoint::ValueList value_list;

	if (!_ep->GetUnreportedValueList(value_list))
	{
		TRACE_WARN("Failed to get unreported value list.");
		return	false;
	}

	node.push_back(JSONNode(TITLE_NAME_ID, _ep->GetID()));
	node.push_back(JSONNode(TITLE_NAME_COUNT, value_list.size()));

	JSONNode	array(JSON_ARRAY);
	for(auto it = value_list.begin(); it != value_list.end() ; it++)
	{
		JSONNode	item;
		
		item.push_back(JSONNode(TITLE_NAME_TIME, time_t((*it)->GetDate())));
		item.push_back(JSONNode(TITLE_NAME_VALUE, std::string(*(*it))));

		array.push_back(item);
	}
	array.set_name(TITLE_NAME_VALUE);

	node.push_back(array);

	auto it = _payload.find(TITLE_NAME_DATA);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	endpoint_array(JSON_ARRAY);		

			endpoint_array.set_name(TITLE_NAME_DATA);
			endpoint_array.push_back(*it);
			endpoint_array.push_back(node);

			_payload.erase(it);

			_payload.push_back(endpoint_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			it->push_back(node);
		}
	}
	else
	{
		node.set_name(TITLE_NAME_DATA);
		_payload.push_back(node);
	}

	return	true;

}
#endif
