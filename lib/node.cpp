#include "defined.h"
#include "exception.h"
#include "json.h"
#include "property.h"
#include "object_manager.h"
#include "node.h"
#include "utils.h"

Node::Node(ObjectManager& _manager, std::string const& _type)
: 	ActiveObject(), 
	manager_(_manager), 
	type_(_type), 
	keep_alive_interval_(OBJECT_KEEP_ALIVE_INTERVAL_SEC),
	model_("general"),
	location_(""), 
	registered_(false),
	db_remove_(false),
	locker_()
{
}

std::string 	Node::GetType() const
{
	return	type_;
}

#if 0
bool	Node::SetType(std::string const& _type, bool _check)
{
	if (!_check)
	{
		type_ = _type;
	}

	return	true;
}
#endif

/*
std::string	Node::GetModel()
{	
	return	"general";
}
*/

std::string	Node::GetModel()
{
	return	model_;
}

bool    Node::SetModel(std::string const& _model, bool _check)
{
	if (!_check)
  	{
  		model_ = _model;
		JSONNodeUpdate(updated_properties_, TITLE_NAME_MODEL, model_);
  
		if (!lazy_store_)
  		{
			ApplyChanges();
 		}
 	}
	 
	return  true;
}


bool	Node::IsIncludedIn(std::string const& _type)
{
	return	Node::Type() == _type;	
}


bool	Node::SetID(std::string const& _id, bool _check)
{
	bool	ret_value = true;

	try
	{
		if ((_id.size() == 0)  || (_id.size() > ID_LENGTH_MAX))
		{
			THROW_INVALID_ARGUMENT("The ID length is (0 < length && length <= " << ID_LENGTH_MAX << ").");
		}

		if (!_check)
		{
			if (id_ != _id)
			{
				std::string 	old_id = id_;

				id_ = _id;
				ret_value = manager_.IDChanged(this, old_id);
			}
		}
	}
	catch(InvalidArgument& e)
	{
		TRACE_ERROR(e.what());
		ret_value = false;	
	}

	return	ret_value;
}

bool	Node::SetKeepAliveInterval(uint32_t	_interval)
{
	if ((_interval != 0) && ((_interval < OBJECT_KEEP_ALIVE_INTERVAL_SEC_MIN) || (OBJECT_KEEP_ALIVE_INTERVAL_SEC_MAX	< _interval)))
	{
		return	false;	
	}

	TRACE_INFO("The keep_alive_interval of the node[" << id_ << "] has been changed from " << keep_alive_interval_ << " to " << _interval << ".");
	keep_alive_interval_ = _interval;

	JSONNodeUpdate(updated_properties_, TITLE_NAME_KEEP_ALIVE_INTERVAL, keep_alive_interval_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Node::SetKeepAliveInterval(std::string const& _interval, bool _check)
{
	if (!_check)
	{
		uint32_t	interval = strtoul(_interval.c_str(), 0, 10);

		return	SetKeepAliveInterval(interval);
	}

	return	true;
}

Date	Node::TimeOfExpire()
{
	return	keep_alive_timer_.reference_date_;
}

bool	Node::SetLocation(std::string const& _location, bool _check)
{
	if (!_check)
	{
		location_ = _location;

		JSONNodeUpdate(updated_properties_, TITLE_NAME_LOCATION, location_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

std::string	Node::GetLocation()
{
	return	location_;
}

bool    Node::SetDevID(std::string const& _dev_id, bool _check)
{
  	if (!_check)
   	{
   		dev_id_ = _dev_id;
 		JSONNodeUpdate(updated_properties_, TITLE_NAME_DEV_ID, dev_id_);
  		if (!lazy_store_)
   		{
   			ApplyChanges();
   		}
  	}
	return  true;
}


std::string     Node::GetDevID()
{
	return  dev_id_;
}

bool	Node::SetRegistered(std::string const& _registered, bool _check)
{
	bool	ret_value = true;
	bool	registered;

	try
	{
		registered = IsTrue(_registered);

		if (!_check)
		{
			if (registered != registered_)
			{
				registered_ = registered;

				JSONNodeUpdate(updated_properties_, TITLE_NAME_REGISTERED, registered_);

				if (!lazy_store_)
				{
					ApplyChanges();	
				}
			}
		}
	}
	catch(InvalidArgument& e)
	{
		TRACE_ERROR(e.what());
		ret_value = false;	
	}

	return	ret_value;
}

bool	Node::SetRegistered(bool _registered)
{
	registered_ = _registered;

	JSONNodeUpdate(updated_properties_, TITLE_NAME_REGISTERED, registered_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}
bool	Node::GetRegistered()
{
	return registered_;
}

bool    Node::SetDBRemove(std::string const& _db_remove, bool _check)
{
	bool    ret_value = true;
	bool    db_remove;
	
	try
	{
		db_remove = IsTrue(_db_remove);
 		if (!_check)
    		{
    			if (db_remove != db_remove_)
   			{
   				db_remove_ = db_remove;
   				JSONNodeUpdate(updated_properties_, TITLE_NAME_DB_REMOVE, (db_remove_)?"true":"false");
      
				if (!lazy_store_)
   				{
 					ApplyChanges();
 				}
 			}
		}
	}
	catch(InvalidArgument& e)
	{
		TRACE_ERROR(e.what());
		ret_value = false;
   	}
	return  ret_value;
}
	 
bool    Node::SetDBRemove(bool _db_remove)
{
	db_remove_ = _db_remove;

	JSONNodeUpdate(updated_properties_, TITLE_NAME_DB_REMOVE, (db_remove_)?"true":"false");
	
	if (!lazy_store_)
	{
		ApplyChanges();
	}
	return  true;
}

bool	Node::GetDBRemove()
{
	return	db_remove_;
}

bool	Node::GetProperty(uint32_t _type, JSONNode& _property)
{
	switch(_type)
	{
	case	PROPERTY_TYPE_FLAG:			_property = JSONNode(TITLE_NAME_TYPE, type_); break;
	case	PROPERTY_MODEL_FLAG: 		_property = JSONNode(TITLE_NAME_MODEL, GetModel()); break;
	case	PROPERTY_LOCATION_FLAG:		_property = JSONNode(TITLE_NAME_LOCATION, location_); break;
	case	PROPERTY_KEEP_ALIVE_INTERVAL_FLAG: _property = JSONNode(TITLE_NAME_KEEP_ALIVE_INTERVAL, keep_alive_interval_); break;
	case	PROPERTY_REGISTERED_FLAG: 	_property = JSONNode(TITLE_NAME_REGISTERED, registered_); break;
	case 	PROPERTY_DB_REMOVE_FLAG:	_property = JSONNode(TITLE_NAME_DB_REMOVE, db_remove_); break;
	case	PROPERTY_TIME_OF_EXPIRE_FLAG:_property = JSONNode(TITLE_NAME_TIME_OF_EXPIRE, TimeOfExpire()); break;
	case    PROPERTY_DEV_ID_FLAG:                   _property = JSONNode(TITLE_NAME_DEV_ID, GetDevID()); break;	
	case	PROPERTY_OPTIONS_FLAG:
		{
			JSONNode	options;

			GetOptions(options);
		
			_property = JSONNode(TITLE_NAME_OPTIONS, options.write());
		}
		break;

	default:
		return	ActiveObject::GetProperty(_type, _property);
	}

	return	true;
}

#if 0
bool	Node::GetProperties(JSONNode& _properties, Fields const& _fields)
{
	if (!ActiveObject::GetProperties(_properties, _fields))
	{
		return	false;	
	}

	if (_fields.type)
	{
		_properties.push_back(JSONNode(TITLE_NAME_TYPE, type_));
	}

	if (_fields.model)
	{
		_properties.push_back(JSONNode(TITLE_NAME_MODEL, GetModel()));
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
		_properties.push_back(JSONNode(TITLE_NAME_TIME_OF_EXPIRE, TimeOfExpire()));
	}

	if (_fields.options)
	{
		JSONNode	options;

		GetOptions(options);
		
		_properties.push_back(JSONNode(TITLE_NAME_OPTIONS, options.write()));
	}

	return	true;
}
#endif

bool	Node::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;

	try
	{
		if (_property.name() == TITLE_NAME_TYPE)
		{
			// Readonly
//			ret_value = SetType(_property.as_string(), _check);
		}
		else if (_property.name() == TITLE_NAME_MODEL)
		{
			// Readonly
			TRACE_INFO("SET MODEL");
			ret_value = SetModel(_property.as_string(), _check);
		}
		else if (_property.name() == TITLE_NAME_LOCATION)
		{
			ret_value = SetLocation(_property.as_string(), _check);
		}
		else if (_property.name() == TITLE_NAME_REGISTERED)
		{
			ret_value = SetRegistered(_property.as_string(), _check);
		}
		else if (_property.name() == TITLE_NAME_KEEP_ALIVE_INTERVAL)
		{
			ret_value = SetKeepAliveInterval(_property.as_string(), _check);
		}
		else if (_property.name() == TITLE_NAME_OPTIONS)
		{
			if (_property.type() == JSON_NODE)
			{
				ret_value = SetOptions(_property.as_node(), _check);
			}
			else
			{
				if (!libjson::is_valid(_property.as_string()))
				{
					TRACE_INFO("Failed to set optoins!");
					TRACE_INFO("Options : " << _property.as_string());
					ret_value = false;
				}
				else
				{
					JSONNode	options = libjson::parse(_property.as_string());

					TRACE_INFO("Set Options : " << options.write_formatted());

					ret_value = SetOptions(options, _check);
				}
			}
		}
		else if (_property.name() == TITLE_NAME_DB_REMOVE)
		{
			ret_value = SetDBRemove(_property.as_string(), _check);
		}	
		else if (_property.name() == TITLE_NAME_DEV_ID)
		{
			ret_value = SetDevID(_property.as_string(), _check);
  		}
		else
		{
			ret_value = ActiveObject::SetProperty(_property, _check);
		}

	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
		ret_value = false;
	}

	return	ret_value;

}

bool	Node::GetOptions(JSONNode& _properties)
{
	JSONNode	empty_options(JSON_NODE);

	_properties = empty_options;
	return	true;
}

bool	Node::SetOptions(JSONNode const& _properties, bool _check)
{
	for(JSONNode::const_iterator it = _properties.begin() ; it != _properties.end() ; it++)
	{
		if (!SetOption(*it, _check))
		{
			return	false;	
		}
	}

	return	true;
}

bool	Node::SetOption(JSONNode const& _property, bool _check)
{
	return	true;
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

	if(GATEWAY_INFO_INTERVAL_SEC != 0)
	{
		Date date = Date::GetCurrent() + Time((uint64_t)GATEWAY_INFO_INTERVAL_SEC * TIME_SECOND);
		gw_info_timer_.Set(date);
		TRACE_INFO("live checker start [" << date << "]");
	}
}

//ADd SY.KANG
void 	Node::InfoProcess()
{
	if(GATEWAY_INFO_INTERVAL_SEC != 0)
	{
		if(gw_info_timer_.RemainTime() == 0)
		{
			gw_info_timer_.Add(GATEWAY_INFO_INTERVAL_SEC);
			manager_.InfoProcess(this);
		}
	}
}
//

void	Node::Process()
{
	if (keep_alive_interval_ != 0)
	{
		if (keep_alive_timer_.RemainTime() == 0)
		{
			keep_alive_timer_.Add(keep_alive_interval_);	
			manager_.KeepAlive(this);
		}
	}

	if(HasChanged())
	{
		ApplyChanges();
	}

	ActiveObject::Process();
}

bool	Node::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	_field_list.push_back(TITLE_NAME_ID);
	_field_list.push_back(TITLE_NAME_TYPE);
	_field_list.push_back(TITLE_NAME_MODEL);
	_field_list.push_back(TITLE_NAME_NAME);
	_field_list.push_back(TITLE_NAME_TIME);
	_field_list.push_back(TITLE_NAME_ENABLE);
	_field_list.push_back(TITLE_NAME_LOOP_INTERVAL);
	_field_list.push_back(TITLE_NAME_PARENT_ID);
	_field_list.push_back(TITLE_NAME_KEEP_ALIVE_INTERVAL);
	_field_list.push_back(TITLE_NAME_LOCATION);
	_field_list.push_back(TITLE_NAME_REGISTERED);
	_field_list.push_back(TITLE_NAME_OPTIONS);
	_field_list.push_back(TITLE_NAME_DB_REMOVE);
	_field_list.push_back(TITLE_NAME_DEV_ID);
	return	true;
}

bool	Node::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<Node*>(_object) != NULL;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	Node::Type()
{
	return	OBJECT_TYPE_NODE;
}

