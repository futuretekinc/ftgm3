#include <fstream>
#include <unistd.h>
#include "defined.h"
#include "exception.h"
#include "trace.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint.h"
#include "endpoint_actuator.h"
#include "property.h"
#include "utils.h"
#include "json.h"

ObjectManager::ObjectManager()
: 	ProcessObject(), 
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

ObjectManager::ObjectManager(std::string const& _id)
: 	ProcessObject(_id), 
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
	for(std::map<std::string, Device*>::iterator it = device_map_.begin() ; it != device_map_.end() ; it++)
	{
		delete it->second;
	}

	for(std::map<std::string, Endpoint*>::iterator it = endpoint_map_.begin() ; it != endpoint_map_.end() ; it++)
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

uint32_t ObjectManager::GetEndpointReportInterval() const
{
	return	endpoint_report_interval_;
}

bool	ObjectManager::	SetEndpointReportInterval(Time const& _time)
{
	endpoint_report_interval_ = _time;

	return	true;
}

bool	ObjectManager::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;
	
	if (_property.name() == TITLE_NAME_ID)
	{
		if (!_check)
		{
			id_ = _property.as_string();
		}
	}
	else if (_property.name() == TITLE_NAME_NAME)
	{
		if (!_check)
		{
			name_ = _property.as_string();
		}
	}
	else if (_property.name() == TITLE_NAME_AUTO_START)
	{
		if ((_property.as_string() == "yes") ||  (_property.as_string() == "on"))
		{
			if (!_check)
			{
				auto_start_ = true;
			}
		}
		else if ((_property.as_string() == "no") ||  (_property.as_string() == "off"))
		{
			if (!_check)
			{
				auto_start_ = false;
			}
		}
	}
	else if (_property.name() == TITLE_NAME_ENDPOINT_REPORT_INTERVAL)
	{
		if (!_check)
		{
			endpoint_report_interval_ = _property.as_int();
		}
	}
	else if (_property.name() == TITLE_NAME_DEVICE)
	{
		if (!_check)
		{
			if (_property.type() == JSON_ARRAY)
			{
				for(JSONNode::const_iterator it = _property.begin(); it != _property.end() ; it++)
				{
					Device* device = CreateDevice(*it);
					if (device == NULL)
					{
						ret_value = false;
						break;
					}

				}
			}
			else if (_property.type() == JSON_NODE)
			{
				Device* device = CreateDevice(_property);
				if (device == NULL)
				{
					ret_value = false;
				}
			}
		}
	}
	else if (_property.name() == TITLE_NAME_DATA_MANAGER)
	{
		if (_property.type() != JSON_NODE)
		{
			THROW_INVALID_ARGUMENT("The " << _property.name() << " type must have node!");
		}

		ret_value = data_manager_.SetProperties(_property, _check, false);
	}
	else if (_property.name() == TITLE_NAME_SERVER_LINKER)
	{
		if (_property.type() != JSON_NODE)
		{
			THROW_INVALID_ARGUMENT("The " << _property.name() << " type must have node!");
		}

		ret_value = server_linker_.SetProperties(_property, _check, false);
	}
	else if (_property.name() == TITLE_NAME_RCS_SERVER)
	{
		if (_property.type() != JSON_NODE)
		{
			THROW_INVALID_ARGUMENT("The " << _property.name() << " type must have node!");
		}

		ret_value = rcs_server_.SetProperties(_property, _check, false);
	}
	else
	{
		ret_value = ProcessObject::SetProperty(_property, _check);
	}

	return	ret_value;
}

ObjectManager::operator JSONNode() const
{
	JSONNode	root, item;

	root.push_back(JSONNode(TITLE_NAME_ID, id_));
	root.push_back(JSONNode(TITLE_NAME_NAME, name_));
	root.push_back(JSONNode(TITLE_NAME_AUTO_START, (auto_start_)?"yes":"no"));
	root.push_back(JSONNode(TITLE_NAME_ENDPOINT_REPORT_INTERVAL, endpoint_report_interval_));

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
	for(std::map<std::string, Gateway*>::iterator it = gateway_map_.begin(); it != gateway_map_.end() ; it++)
	{
		_list.push_back(it->second);
	}

	for(std::map<std::string, Device*>::iterator it = device_map_.begin(); it != device_map_.end() ; it++)
	{
		_list.push_back(it->second);
	}

	for(std::map<std::string, Endpoint*>::iterator it = endpoint_map_.begin(); it != endpoint_map_.end() ; it++)
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

Gateway*		ObjectManager::CreateGateway(JSONNode const& _properties, bool from_db)
{
	JSONNode	properties = _properties;
	
	JSONNode	device_property;
	if (JSONNodeIsExistField(properties, TITLE_NAME_DEVICE))
	{
		JSONNode::iterator it = properties.find(TITLE_NAME_DEVICE);
		if (it != properties.end())
		{
			if (it->type() == JSON_NODE)
			{
				device_property = it->as_node();
			}
			else if (it->type() == JSON_ARRAY)
			{
				device_property = it->as_array();
			}
		}

		properties.erase(it);
	}

// XTRA
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

		TRACE_INFO("The gateway[" << gateway->GetID() << "] created!");

		if ((!from_db) && (device_property.size() != 0))
		{
		
			if (device_property.type() == JSON_NODE)
			{
				device_property.push_back(JSONNode(TITLE_NAME_PARENT_ID, gateway->GetID()));		

				Device * device = CreateDevice(device_property);
				if (device == NULL)
				{
					TRACE_ERROR("Failed to create deivce!");
				}
			}
			else if (device_property.type() == JSON_ARRAY)
			{
				for(JSONNode::iterator it = device_property.begin() ; it != device_property.end() ; it++)
				{
					it->push_back(JSONNode(TITLE_NAME_PARENT_ID, gateway->GetID()));		

					Device * device = CreateDevice(*it);
					if (device == NULL)
					{
						TRACE_ERROR("Failed to create deivce!");
					}
				}
			}
		}
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
	for(std::map<std::string, Gateway *>::iterator it = gateway_map_.begin(); it != gateway_map_.end() ; it++)
	{
		_gateway_list.push_back(it->second);	
	}

	return	_gateway_list.size();
}

uint32_t	ObjectManager::GetGatewayList(std::string const& _type, std::list<Gateway*>& _gateway_list)
{
	for(std::map<std::string, Gateway*>::iterator it = gateway_map_.begin(); it != gateway_map_.end() ; it++)
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
	std::map<std::string, Gateway*>::iterator it = gateway_map_.find(_id);
	if (it != gateway_map_.end())
	{
		return	it->second;	
	}

	TRACE_ERROR("Gateway[" << _id << "] not found!");
	return	NULL;
}

Gateway*		ObjectManager::GetMasterGateway()
{
	if (gateway_map_.size() != 0)
	{
		return	gateway_map_.begin()->second;	
	}

	TRACE_ERROR("The master gateway not found!");

	return	NULL;
}

bool		ObjectManager::StartGateway(std::string const& _id)
{
	Gateway* gateway = GetGateway(_id);
	if (gateway == NULL)
	{
		TRACE_ERROR("Failed to start gateway because the gateway[" << _id << "] coult not be found.");
		return	false;	
	}

	if (!gateway->Start(1000))
	{
		TRACE_ERROR("Failed to start gateway.");
		return	false;
	}

	return	true;
}

bool		ObjectManager::StopGateway(std::string const& _id)
{
	Gateway* gateway = GetGateway(_id);
	if (gateway == NULL)
	{
		TRACE_ERROR("Failed to stop gateway because the gateway[" << _id << "] coult not be found.");
		return	false;	
	}

	if (!gateway->Stop())
	{
		TRACE_ERROR("Failed to start gateway.");
		return	false;
	}

	return	true;
}

Device*	ObjectManager::CreateDevice(JSONNode const& _properties, bool from_db)
{
	Device*		device = NULL;

	JSONNode	properties = _properties;
	
	if (!JSONNodeIsExistField(properties, TITLE_NAME_PARENT_ID))
	{
		TRACE_INFO("The properties does not contain a parent id.");

		Gateway*	gateway = GetMasterGateway();	
		if (gateway == NULL)
		{
			TRACE_ERROR("At least one gateway must be registered.");
			return	NULL;
		}

		properties.push_back(JSONNode(TITLE_NAME_PARENT_ID, gateway->GetID()));
	}


	JSONNode	endpoint_property;
	if (JSONNodeIsExistField(properties, TITLE_NAME_ENDPOINT))
	{
		JSONNode::iterator it = properties.find(TITLE_NAME_ENDPOINT);
		if (it != properties.end())
		{
			if (it->type() == JSON_NODE)
			{
				endpoint_property = it->as_node();
			}
			else if (it->type() == JSON_ARRAY)
			{
				endpoint_property = it->as_array();
			}
		}

		properties.erase(it);
	}
// XTRA
	device = Device::Create(*this, properties);
	if (device != NULL)
	{
		Attach(device);	

		if (!from_db)
		{
			data_manager_.AddDevice(device);
		}

		device->ClearUpdatedProperties();

		TRACE_INFO("The device[" << device->GetID() << "] crated!");

		Gateway*	gateway = GetGateway(device->GetParentID());
		if (gateway != NULL)
		{
			if (!gateway->Attach(device->GetID()))
			{
				TRACE_ERROR("Failed to attach device[" << device->GetTraceName() << "]");
			}
		}

		if ((!from_db) && (endpoint_property.size() != 0))
		{
		
			if (endpoint_property.type() == JSON_NODE)
			{
				endpoint_property.push_back(JSONNode(TITLE_NAME_PARENT_ID, device->GetID()));		

				Endpoint* endpoint = CreateEndpoint(endpoint_property);
				if (endpoint == NULL)
				{
					TRACE_ERROR("Failed to create endpoint!");
				}
			}
			else if (endpoint_property.type() == JSON_ARRAY)
			{
				for(JSONNode::iterator it = endpoint_property.begin() ; it != endpoint_property.end() ; it++)
				{
					it->push_back(JSONNode(TITLE_NAME_PARENT_ID, device->GetID()));		

					Endpoint* endpoint = CreateEndpoint(*it);
					if (endpoint == NULL)
					{
						TRACE_ERROR("Failed to create endpoint!");
						break;
					}
				}
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
	for(std::map<std::string, Device *>::iterator it = device_map_.begin(); it != device_map_.end() ; it++)
	{
		_device_list.push_back(it->second);	
	}

	return	_device_list.size();
}

uint32_t	ObjectManager::GetDeviceList(std::string const& _type, std::list<Device*>& _device_list)
{
	for(std::map<std::string, Device*>::iterator it = device_map_.begin(); it != device_map_.end() ; it++)
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
	std::map<std::string, Device*>::iterator it = device_map_.find(_id);
	if (it != device_map_.end())
	{
		return	it->second;	
	}

	TRACE_ERROR("Device[" << _id << "] not found!");
	return	NULL;
}

bool		ObjectManager::StartDevice(std::string const& _id)
{
	Device * device = GetDevice(_id);
	if (device == NULL)
	{
		TRACE_ERROR("Failed to start device because the device[" << _id << "] coult not be found.");
		return	false;	
	}

	if (!device->Start(1000))
	{
		TRACE_ERROR("Failed to start device");
		return	false;
	}

	return	true;
}

bool		ObjectManager::StopDevice(std::string const& _id)
{
	Device* device = GetDevice(_id);
	if (device == NULL)
	{
		TRACE_ERROR("Failed to stop device because the device[" << _id << "] coult not be found.");
		return	false;	
	}

	if (!device->Stop())
	{
		TRACE_ERROR("Failed to stop device.");
		return	false;
	}

	return	true;
}

Endpoint*	ObjectManager::CreateEndpoint(JSONNode const& _properties, bool from_db)
{
	Endpoint*	endpoint = Endpoint::Create(*this, _properties);		
	if (endpoint != NULL)
	{
		Attach(endpoint);

		if (!from_db)
		{
			JSONNode	properties;

			endpoint->GetProperties(properties);

			TRACE_INFO("ENDPOINT : " << properties.write_formatted());
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
		
		std::map<std::string, Endpoint*>::iterator it = endpoint_map_.find(endpoint->GetID());
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
	for(std::map<std::string, Endpoint*>::iterator it = endpoint_map_.begin(); it != endpoint_map_.end() ; it++)
	{
		_endpoint_list.push_back(it->second);	
	}

	return	_endpoint_list.size();
}

Endpoint*		ObjectManager::GetEndpoint(std::string const& _id)
{
	std::map<std::string, Endpoint*>::iterator it = endpoint_map_.find(_id);
	if (it != endpoint_map_.end())
	{
		return	it->second;	
	}

	TRACE_ERROR("Endpoint [" << _id << "] not found!");

	return	NULL;
}

bool		ObjectManager::StartEndpoint(std::string const& _id)
{
	Endpoint * endpoint = GetEndpoint(_id);
	if (endpoint == NULL)
	{
		TRACE_ERROR("Failed to start endpoint because the endpoint[" << _id << "] coult not be found.");
		return	false;	
	}

	if (!endpoint->Start(1000))
	{
		TRACE_ERROR("Failed to start endpoint");
		return	false;
	}

	return	true;
}

bool		ObjectManager::StopEndpoint(std::string const& _id)
{
	Endpoint* endpoint = GetEndpoint(_id);
	if (endpoint == NULL)
	{
		TRACE_ERROR("Failed to stop endpoint because the endpoint[" << _id << "] coult not be found.");
		return	false;	
	}

	if (!endpoint->Stop())
	{
		TRACE_ERROR("Failed to stop endpoint.");
		return	false;
	}

	return	true;
}

///////////////////////////////////////////////////////////////////////////
// Rule opertions
///////////////////////////////////////////////////////////////////////////

Rule*	ObjectManager::CreateRule(JSONNode const& _properties, bool from_db)
{
	Rule*	rule = rule_manager_.CreateRule(_properties);		
	if (rule != NULL)
	{
		if (!from_db)
		{
			data_manager_.AddRule(rule);

			JSONNode	properties;

			rule->GetProperties(properties);

			TRACE_INFO("Rule : " << properties.write_formatted());
		}
	}
	else
	{
		TRACE_ERROR("Failed to create rule");	
	}

	return	rule;
}

uint32_t	ObjectManager::GetRuleList(std::list<Rule*>& _list)
{
	return	rule_manager_.GetRuleList(_list);
}

Rule* ObjectManager::GetRule(std::string const& _id)
{
	return	rule_manager_.GetRule(_id);
}

bool	ObjectManager::IDChanged(Node* _node, std::string const& _old_id)
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

bool	ObjectManager::IDChanged(Gateway* _gateway, std::string const& _old_id)
{
	std::map<std::string, Gateway*>::iterator it = gateway_map_.find(std::string(_old_id));
	if (it != gateway_map_.end())
	{
		gateway_map_.erase(it);
		gateway_map_[_gateway->GetID()] = _gateway;

		return	true;	
	}

	return	false;
}

bool	ObjectManager::IDChanged(Device* _device, std::string const& _old_id)
{
	std::map<std::string, Device*>::iterator it = device_map_.find(std::string(_old_id));
	if (it != device_map_.end())
	{
		device_map_.erase(it);
		device_map_[_device->GetID()] = _device;

		return	true;	
	}

	return	false;
}

bool	ObjectManager::IDChanged(Endpoint* _endpoint, std::string const& _old_id)
{
	std::map<std::string, Endpoint*>::iterator it = endpoint_map_.find(std::string(_old_id));
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
	JSONNode	properties;

	if (_gateway->GetUpdatedProperties(properties))
	{
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
	JSONNode	properties;

	if (_device->GetUpdatedProperties(properties))
	{
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
	JSONNode	properties;

	if (_endpoint->GetUpdatedProperties(properties))
	{
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
	data_manager_.Start(10000);

	uint32_t count = data_manager_.GetGatewayCount();
	TRACE_INFO("Gateway Count : " << count);
#if 0
	if (count == 0)
	{
		JSONNode	properties;

		properties.push_back(JSONNode(TITLE_NAME_ID, id_));
		properties.push_back(JSONNode(TITLE_NAME_NAME, name_));
		properties.push_back(JSONNode(TITLE_NAME_TYPE, "gw_gen"));

		Gateway*	gateway = CreateGateway(properties);
		if (gateway == NULL)
		{
			TRACE_ERROR("Failed to create gateway!");	
		}
		else
		{
			TRACE_INFO("The gateway[" << gateway->GetTraceName() << "] created");	
		}
	}
	else
#endif
	{
		for(uint32_t i = 0 ; i < count ; i++)
		{
			JSONNode	properties_array;

			if (data_manager_.GetGatewayProperties(i, 1, properties_array))
			{
				for(JSONNode::const_iterator it = properties_array.begin() ; it != properties_array.end() ; it++)
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
			JSONNode	properties_array;

			if (data_manager_.GetDeviceProperties(i, 1, properties_array))
			{
				for(JSONNode::const_iterator it = properties_array.begin() ; it != properties_array.end() ; it++)
				{
					TRACE_INFO("Device Create ");
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
			JSONNode properties_list;

			if (data_manager_.GetEndpointProperties(i, 1, properties_list))
			{
				for(JSONNode::const_iterator it = properties_list.begin() ; it != properties_list.end() ; it++)
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

		count = data_manager_.GetRuleCount();
		TRACE_INFO("Rule Count : " << count);

		for(uint32_t i = 0 ; i < count ; i++)
		{
			JSONNode	properties_array;

			if (data_manager_.GetRuleProperties(i, 1, properties_array))
			{
				for(JSONNode::const_iterator it = properties_array.begin() ; it != properties_array.end() ; it++)
				{
					TRACE_INFO("Rule Create ");
					Rule*	rule = CreateRule(*it, true);
					if (rule == NULL)
					{
						TRACE_ERROR("Failed to create rule!");	
					}
					else
					{
						TRACE_INFO("The rule[" << rule->GetTraceName() << "] created");	
					}
				}
			}
		}
	}

	rcs_server_.Start(10000);
	server_linker_.Start(10000);

	TRACE_INFO("Auto Start : " << auto_start_ );
	if (auto_start_)
	{
		for(std::map<std::string, Gateway *>::iterator it = gateway_map_.begin(); it != gateway_map_.end() ; it++)
		{
			if(it->second->GetEnable())
			{
				it->second->Start(GATEWAY_START_INITIALIZATION_TIMEOUT);	
			}
		}

		for(std::map<std::string, Device *>::iterator it = device_map_.begin(); it != device_map_.end() ; it++)
		{
			if(it->second->GetEnable())
			{
				it->second->Start(GATEWAY_START_INITIALIZATION_TIMEOUT);	
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
		for(std::map<std::string, Endpoint*>::iterator it = endpoint_map_.begin(); it != endpoint_map_.end() ; it++)
		{
			if (it->second->IsRunning())
			{
				server_linker_.ReportEPData(it->second);
			}
		}

		endpoint_report_timer_.Add(endpoint_report_interval_);

	}

	ProcessObject::Process();
}

void	ObjectManager::Postprocess()
{
	TRACE_INFO("Object manager post process!");
	for(std::map<std::string, Endpoint*>::iterator it = endpoint_map_.begin(); it != endpoint_map_.end() ; it++)
	{
		it->second->Stop();	
	}

	for(std::map<std::string, Device*>::iterator it = device_map_.begin(); it != device_map_.end() ; it++)
	{
		it->second->Stop();	
	}

	for(std::map<std::string, Gateway *>::iterator it = gateway_map_.begin(); it != gateway_map_.end() ; it++)
	{
		it->second->Stop();	
	}

	Date date = Date::GetCurrent() + Time(endpoint_report_interval_ * TIME_SECOND);
	endpoint_report_timer_.Set(date);
	rcs_server_.Stop(true);
	server_linker_.Stop(true);

	ProcessObject::Postprocess();
}

bool	ObjectManager::Attach(Gateway* _gateway)
{
	std::map<std::string, Gateway *>::iterator it = gateway_map_.find(_gateway->GetID());
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
	std::map<std::string, Gateway *>::iterator it = gateway_map_.find(_gateway->GetID());
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
	std::map<std::string, Device*>::iterator it = device_map_.find(_device->GetID());
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
	std::map<std::string, Device*>::iterator it = device_map_.find(_device->GetID());
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
	std::map<std::string, Endpoint*>::iterator it = endpoint_map_.find(_endpoint->GetID());
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
	std::map<std::string, Endpoint*>::iterator it = endpoint_map_.find(_endpoint->GetID());
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
	std::map<std::string,RCSSession*>::iterator it = rms_map_.find(_session->GetID());
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
	std::map<std::string,RCSSession*>::iterator it = rms_map_.find(_session->GetID());
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

#if 0
			if (!data_manager_.AddValue(message->endpoint_id, message->time, message->value))
			{
				TRACE_ERROR("Failed to add value!");	
			}
#endif

			Endpoint* endpoint = GetEndpoint(message->endpoint_id);
			//if ((endpoint != NULL) && endpoint->IsParticipatedInRules())
			if (endpoint != NULL)
			{
				if (!rule_manager_.Updated(message->endpoint_id, message->time, message->value))
				{
					TRACE_ERROR("Failed to add value!");	
				}
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
			return	ProcessObject::OnMessage(_base_message);
		}

	}

	return	true;
}


//add SY.KANG
bool	ObjectManager::InfoProcess(Node* _object)
{
	JSONNode	properties;
	_object->GetWanIP(properties);

	RCSMessage message(MSG_TYPE_RCS_GW_INFO);

	message.AddGateway(properties);
	TRACE_INFO("OBJECT_MANAGER_INFO_PROCESS SUCCESS");
	return server_linker_.Send(message);

	
}
//

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

	ServerLinkerMosq::UpLink*	up_link = server_linker_.GetUpLink(_topic);
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

	topic << DEFAULT_CONST_MSG_VERSION << "/gw/" << _id;

	return	topic.str();
}

std::string	ObjectManager::GetTopicNameDevice(std::string const& _id)
{
	std::ostringstream	topic;

	topic << DEFAULT_CONST_MSG_VERSION << "/dev/" << _id;

	return	topic.str();
}

std::string	ObjectManager::GetTopicNameEndpoint(std::string const& _id)
{
	std::ostringstream	topic;

	topic << DEFAULT_CONST_MSG_VERSION << "/ep/" << _id;

	return	topic.str();
}


