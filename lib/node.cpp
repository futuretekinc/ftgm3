#include "defined.h"
#include "object_manager.h"
#include "node.h"
#include "utils.h"

NodeInfo::NodeInfo()
: 	fields(), 
	id(""), 
	name(""), 
	date(Date::GetCurrent()), 
	enable(false),
	type(""), 
	parent_id(""), 
	keep_alive_interval(OBJECT_KEEP_ALIVE_INTERVAL_SEC),
	location(""), 
	registered(false)
{
}

NodeInfo::NodeInfo(JSONNode const& _json)
{
	std::string	value;

	if (GetValue(_json, TITLE_NAME_PARENT_ID, value, false))
	{
		parent_id = value;
		fields.parent_id = true;
	}

	if (GetValue(_json, TITLE_NAME_ID, value, false))
	{
		id = value;
		fields.id = true;
	}

	if (GetValue(_json, TITLE_NAME_NAME, value, false))
	{
		name = value;
		fields.name = true;
	}

	if (GetValue(_json, TITLE_NAME_TIME, date, false))
	{
		fields.date = true;
	}

	if (GetValue(_json, TITLE_NAME_ENABLE, enable, false))
	{
		fields.enable = true;
	}

	if (GetValue(_json, TITLE_NAME_TYPE, value, false))
	{
		type = value;
		fields.type = true;
	}

	if (GetValue(_json, TITLE_NAME_KEEP_ALIVE_INTERVAL, keep_alive_interval, false))
	{
		fields.keep_alive_interval = true;
	}

	if (GetValue(_json, TITLE_NAME_LOCATION, value, false))
	{
		location = value;
		fields.location = true;
	}

	if (GetValue(_json, TITLE_NAME_REGISTERED, registered, false))
	{
		fields.registered = true;
	}
}

bool	NodeInfo::GetProperties(JSONNode& _properties, Properties::Fields const& _fields)
{
	if (_fields.id)
	{
		_properties.push_back(JSONNode(TITLE_NAME_ID, id));
	}

	if (_fields.name)
	{
		_properties.push_back(JSONNode(TITLE_NAME_NAME, name));
	}
	
	if (_fields.time)
	{
		_properties.push_back(JSONNode(TITLE_NAME_TIME, time_t(date)));
	}

	if (_fields.enable)
	{
		_properties.push_back(JSONNode(TITLE_NAME_ENABLE, enable));
	}

	if (_fields.parent_id)
	{
		_properties.push_back(JSONNode(TITLE_NAME_PARENT_ID, parent_id));
	}


	if (_fields.type)
	{
		_properties.push_back(JSONNode(TITLE_NAME_TYPE, type));
	}

	if (_fields.location)
	{
		_properties.push_back(JSONNode(TITLE_NAME_LOCATION, location));
	}

	if (_fields.keep_alive_interval)
	{
		_properties.push_back(JSONNode(TITLE_NAME_KEEP_ALIVE_INTERVAL, keep_alive_interval));
	}

	if (_fields.registered)
	{
		_properties.push_back(JSONNode(TITLE_NAME_REGISTERED, registered));
	}
#if 0
	if (_fields.time_of_expire)
	{
		_properties.push_back(JSONNode(TITLE_NAME_TIME_OF_EXPIRE, TimeOfExpire()));
	}
#endif

	return	true;
}

Node::Node(ObjectManager& _manager, ValueType const& _type)
: 	ActiveObject(), 
	manager_(_manager), 
	type_(_type), 
	keep_alive_interval_(OBJECT_KEEP_ALIVE_INTERVAL_SEC),
	location_(""), 
	registered_(false)
{
}

ValueType 	Node::GetType() const
{
	return	type_;
}

bool		Node::IsIncludedIn(ValueType const& _type)
{
	return	Node::Type() == _type;	
}

bool	Node::SetKeepAliveInterval(uint32_t	_interval)
{
	if ((_interval < OBJECT_KEEP_ALIVE_INTERVAL_SEC_MIN) || (OBJECT_KEEP_ALIVE_INTERVAL_SEC_MAX	< _interval))
	{
		return	false;	
	}

	TRACE_INFO("The keep_alive_interval of the node[" << id_ << "] has been changed from " << keep_alive_interval_ << " to " << _interval << ".");
	keep_alive_interval_ = _interval;

	updated_properties_.AppendKeepAliveInterval(keep_alive_interval_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

Date	Node::TimeOfExpire()
{
	return	keep_alive_timer_.reference_date_;
}

bool		Node::SetLocation(std::string const& _location)
{
	location_ = _location;

	updated_properties_.AppendLocation(location_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

const	std::string&	Node::GetLocation()
{
	return	location_;
}

bool	Node::SetRegistered(std::string const& _registered)
{
	if ((_registered == "yes") || (_registered == "on") || (_registered == "true") || (_registered == "1"))
	{
		registered_ = true;
	}
	else if ((_registered == "no") || (_registered == "off") || (_registered == "false") || (_registered == "0"))
	{
		registered_ = false;
	}
	else
	{
		return	false;	
	}

	updated_properties_.AppendRegistered(registered_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}
bool	Node::SetRegistered(bool _registered)
{
	registered_ = _registered;

	updated_properties_.AppendRegistered(registered_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}


bool	Node::GetRegistered()
{
	return	registered_;
}

bool	Node::GetProperties(Properties& _properties, Properties::Fields const& _fields) 
{
	if (!ActiveObject::GetProperties(_properties, _fields))
	{
		return	false;
	}

	if (_fields.type)
	{
		_properties.AppendType(type_);
	}

	if (_fields.location)
	{
		_properties.AppendLocation(location_);
	}

	if (_fields.keep_alive_interval)
	{
		_properties.AppendKeepAliveInterval(keep_alive_interval_);
	}

	if (_fields.registered)
	{
		_properties.AppendRegistered(registered_);
	}

	if (_fields.time_of_expire)
	{
		_properties.AppendTimeOfExpire(TimeOfExpire());
	}

	return	true;
}

bool	Node::GetProperties(JSONNode& _properties, Properties::Fields const& _fields)
{
	if (!ActiveObject::GetProperties(_properties, _fields))
	{
		return	false;	
	}

	if (_fields.type)
	{
		_properties.push_back(JSONNode(TITLE_NAME_TYPE, type_));
	}

	if (_fields.location)
	{
		_properties.push_back(JSONNode(TITLE_NAME_LOCATION, location_));
	}

	if (_fields.keep_alive_interval)
	{
		_properties.push_back(JSONNode(TITLE_NAME_KEEP_ALIVE_INTERVAL, keep_alive_interval_));
	}

	if (_fields.registered)
	{
		_properties.push_back(JSONNode(TITLE_NAME_REGISTERED, registered_));
	}

	if (_fields.time_of_expire)
	{
#ifdef	PROPERTY_VALUE_STRING_ONLY
		_properties.push_back(JSONNode(TITLE_NAME_TIME_OF_EXPIRE, std::to_string(time_t(keep_alive_timer_.reference_date_))));
#else
		_properties.push_back(JSONNode(TITLE_NAME_TIME_OF_EXPIRE, TimeOfExpire()));
#endif	
	}

	return	true;
}


bool	Node::SetProperty(Property const& _property, Properties::Fields const& _fields)
{
	bool	ret_value = true;

	try
	{
		if (_property.GetName() == TITLE_NAME_TYPE)
		{
			if (_fields.type)
			{
				const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
				if (value == NULL)
				{
					throw std::invalid_argument("Property id value type is incorrect!");
				}

				type_ = value->Get();
			}
		}
		else if (_property.GetName() == TITLE_NAME_ID)
		{
			if (_fields.id)
			{
				ValueID	old_id = id_;

				const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
				if (value == NULL)
				{
					throw std::invalid_argument("Property id value type is incorrect!");
				}

				if (!id_.Set(value->Get()))
				{
					TRACE_ERROR("Failed to set id!");
					ret_value = false;
				}
				else
				{
					ret_value = manager_.IDChanged(this, old_id);
				}
			}
		}
		else if (_property.GetName() == TITLE_NAME_LOCATION)
		{
			if (_fields.location)
			{
				const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
				if (value == NULL)
				{
					std::ostringstream	oss;

					oss << "Property " << _property.GetName() << " value type is incorrect!";
					throw std::invalid_argument(oss.str());
				}

				ret_value = SetLocation(value->Get());
			}
		}
		else if (_property.GetName() == TITLE_NAME_REGISTERED)
		{
			if (_fields.registered)
			{
				const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
				if (value != NULL)
				{
					ret_value = SetRegistered(value->Get());
				}
				else 
				{
					const ValueInt*	value = dynamic_cast<const ValueInt*>(_property.GetValue());
					if (value != NULL)
					{
						ret_value = SetRegistered(value->Get());
					}
					else
					{
						const ValueBool*	value = dynamic_cast<const ValueBool*>(_property.GetValue());
						if (value == NULL)
						{
							std::ostringstream	oss;

							oss << "Property " << _property.GetName() << " value type[" << _property.GetValue()->GetTypeString() << "]is incorrect!";
							throw std::invalid_argument(oss.str());
						}

						ret_value = SetRegistered(value->Get());
					}
				}
			}
		}
		else if (_property.GetName() == TITLE_NAME_KEEP_ALIVE_INTERVAL)
		{
			if (_fields.keep_alive_interval)
			{
				Time keep_alive_interval = 0;

				const ValueInt*	int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
				if (int_value == NULL)
				{
					const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
					if (string_value == NULL)
					{
						const ValueTime *time_value = dynamic_cast<const ValueTime*>(_property.GetValue());
						if (time_value == NULL)
						{	
							std::ostringstream	oss;

							oss << "Property " << _property.GetName() << " value type is incorrect!";
							throw std::invalid_argument(oss.str());
						}

						ret_value = SetKeepAliveInterval(time_value->Get());
					}
					else
					{
						keep_alive_interval = strtoul(string_value->Get().c_str(), NULL, 10);
					}
				}
				else
				{
					keep_alive_interval = int_value->Get();	
				}

				ret_value = SetKeepAliveInterval(keep_alive_interval);
			}
		}
		else
		{
			ret_value = ActiveObject::SetProperty(_property, _fields);
		}

	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
		ret_value = false;
	}

	return	ret_value;

}

bool	Node::ApplyChanges()
{
	if (updated_properties_.size() != 0)
	{
		manager_.UpdateProperties(this);
	}

	return	Object::ApplyChanges();
}

void	Node::Preprocess()
{
	ActiveObject::Preprocess();

	if (keep_alive_interval_ != 0)
	{
		Date date = Date::GetCurrent() + Time((uint64_t)keep_alive_interval_ * TIME_SECOND);
		keep_alive_timer_.Set(date);
		TRACE_INFO("live checker start [" << date << "]");
	}
}

void	Node::Process()
{
	if (keep_alive_interval_ != 0)
	{
		if (keep_alive_timer_.RemainTime() == 0)
		{
			keep_alive_timer_ += Time((uint64_t)keep_alive_interval_ * TIME_SECOND);	
			manager_.KeepAlive(this);
		}
	}

	if(HasChanged())
	{
		ApplyChanges();
	}

	ActiveObject::Process();
}

const	ValueType&	Node::Type()
{
	static	ValueType type("node");

	return	type;
}

bool		Node::IsValidType(std::string const& _type)
{
	if (_type == std::string(Node::Type()))
	{
		return	true;	
	}

	return	false;
}

bool	Node::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	_field_list.push_back(TITLE_NAME_ID);
	_field_list.push_back(TITLE_NAME_TYPE);
	_field_list.push_back(TITLE_NAME_NAME);
	_field_list.push_back(TITLE_NAME_TIME);
	_field_list.push_back(TITLE_NAME_ENABLE);
	_field_list.push_back(TITLE_NAME_LOOP_INTERVAL);
	_field_list.push_back(TITLE_NAME_PARENT_ID);
	_field_list.push_back(TITLE_NAME_KEEP_ALIVE_INTERVAL);
	_field_list.push_back(TITLE_NAME_LOCATION);
	_field_list.push_back(TITLE_NAME_REGISTERED);

	return	true;
}

bool	Node::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<Node*>(_object) != NULL;
}
