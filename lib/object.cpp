#include <inttypes.h>
#include <typeinfo>
#include <iomanip>
#include <sstream>
#include <map>
#include "defined.h"
#include "trace.h"
#include "object.h"
#include "object_manager.h"
#include "json.h"
#include "utils.h"
#include <hashlib++/hashlibpp.h>
#include "exception.h"

static std::map<std::string, Object*>	object_map;

Object::Object()
: parent_id_(""), enable_(false), trace(this), lazy_store_(false)
{
	md5wrapper*	md5 = new md5wrapper;

	date_ = Date::GetCurrent();
	std::ostringstream	oss;

	oss << date_.GetMicroSecond();

	id_ = md5->getHashFromString(oss.str());

	name_ = std::string(id_);

	object_map[id_] = this;
}

Object::Object(std::string const& _id)
: parent_id_(""), id_(_id), enable_(false), trace(this)
{
	md5wrapper*	md5 = new md5wrapper;

	date_ = Date::GetCurrent();
	std::ostringstream	oss;

	oss << date_.GetMicroSecond();

	name_ = std::string(id_);

	object_map[id_] = this;
}

Object::~Object()
{
	object_map.erase(name_);
	TRACE_INFO("Object[" << GetTraceName() << "] destroy");
}


std::string		Object::GetClassName()
{
	//if (class_name_.size() == 0)
	{
		uint32_t	i = 0;
		std::string	name = typeid(*this).name();

		for(i = 0 ; i < name.size() ; i++)
		{
			if (!isdigit(name[i]))
			{
				break;
			}
		}

		class_name_ = name.substr(i, name.size() - i);
	}

	return	class_name_;
}

const	std::string&	Object::GetID() const
{
	return	id_;
}

bool	Object::SetID(std::string const& _id, bool _check)
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
				id_ = _id;

				JSONNodeUpdate(updated_properties_, TITLE_NAME_ID, id_);

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

const std::string&	Object::GetName() const
{
	return	name_;
}

bool	Object::SetName(std::string const& _name, bool _check)
{
	bool	ret_value = true;
	try
	{
		if ((_name.size() == 0)  || (_name.size() > NAME_LENGTH_MAX))
		{
			THROW_INVALID_ARGUMENT("The ID length is (0 < length && length <= " << NAME_LENGTH_MAX << ").");
		}

		if (!_check)
		{
			if (name_ != _name)
			{
				name_ = _name;

				JSONNodeUpdate(updated_properties_, TITLE_NAME_NAME, name_);

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

bool	Object::GetEnable() const
{
	return	enable_;
}

bool	Object::SetEnable(bool _enable)
{
	if (enable_ != _enable)
	{
		enable_ = _enable;

		JSONNodeUpdate(updated_properties_, TITLE_NAME_ENABLE, enable_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

bool	Object::SetEnable(std::string const& _enable, bool _check)
{
	bool	enable;
	bool	ret_value  = true;

	try
	{
		if (!_check)
		{
			enable = IsTrue(_enable);

			if (enable_ != enable)
			{
				enable_ = enable;

				JSONNodeUpdate(updated_properties_, TITLE_NAME_ENABLE, enable_);

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

Object::Stat	Object::GetState() const
{
	if (enable_)
	{
		return	ENABLED;	
	}
	else
	{
		return	DISABLED;	
	}
}

const Date&		Object::GetDate() const
{
	return	date_;
}

bool	Object::SetDate(Date const& _date)
{
	date_ = _date;

	JSONNodeUpdate(updated_properties_, TITLE_NAME_TIME, time_t(date_));

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Object::SetDate(std::string const& _date, bool _check)
{
	bool	ret_value = true;
	try
	{
		if (!_check)
		{
			Date	date = _date;
			if (date_ != date)
			{
				date_ = date;

				JSONNodeUpdate(updated_properties_, TITLE_NAME_TIME, time_t(date_));

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

const std::string&	Object::GetParentID() const
{
	return	parent_id_;
}

bool	Object::SetParentID(std::string const& _parent_id, bool _check)
{
	bool	ret_value = true;

	try
	{
		if (_parent_id.size() > ID_LENGTH_MAX)
		{
			THROW_INVALID_ARGUMENT("The ID length is (0 < length && length <= " << ID_LENGTH_MAX << ").");
		}

		if (!_check)
		{
			if (parent_id_ != _parent_id)
			{
				parent_id_ = _parent_id;

				TRACE_INFO("Object[" << id_ << "] set parent id[" << parent_id_  << "]");
				JSONNodeUpdate(updated_properties_, TITLE_NAME_PARENT_ID, parent_id_);

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

bool	Object::SetLazyStore(bool _enable)
{
	lazy_store_ = _enable;

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Object::HasChanged() const
{
	return	(updated_properties_.size() != 0);
}

bool	Object::ApplyChanges() 
{
	lazy_store_ = false;

	return	true;
}

bool	Object::ClearUpdatedProperties() 
{

	updated_properties_.clear();

	return	true;
}

bool	Object::GetProperty(JSONNode& _property)
{
	bool	ret_value = true;

	try
	{
		if (_property.name() == TITLE_NAME_TRACE)
		{
			_property = JSONNode(trace);
			_property.set_name(TITLE_NAME_TRACE);
		}
		else if (_property.name() == TITLE_NAME_ID)
		{
			_property = id_;
		}
		else if (_property.name() == TITLE_NAME_NAME)
		{
			_property = name_;
		}
		else if (_property.name() == TITLE_NAME_TIME)
		{
			_property = time_t(date_);
		}
		else if (_property.name() == TITLE_NAME_PARENT_ID)
		{
			_property = parent_id_;
		}
		else if (_property.name() == TITLE_NAME_ENABLE)
		{
			_property = enable_;
		}
		else
		{
			THROW_INVALID_ARGUMENT("The " << _property.name() << " configuration is not supported!");
		}
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
		ret_value = false;
	}

	return	ret_value;
}

bool	Object::GetProperties(JSONNode& _properties, Fields const& _fields)
{
	if (_fields.id)
	{
		_properties.push_back(JSONNode(TITLE_NAME_ID, id_));
	}

	if (_fields.name)
	{
		_properties.push_back(JSONNode(TITLE_NAME_NAME, name_));
	}
	
	if (_fields.time)
	{
		_properties.push_back(JSONNode(TITLE_NAME_TIME, time_t(date_)));
	}

	if (_fields.enable)
	{
		_properties.push_back(JSONNode(TITLE_NAME_ENABLE, enable_));
	}

	if (_fields.parent_id)
	{
		_properties.push_back(JSONNode(TITLE_NAME_PARENT_ID, parent_id_));
	}

	return	true;

}

bool	Object::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;

	try
	{
		if (_property.name() == TITLE_NAME_TRACE)
		{
			if (_property.type() != JSON_NODE)
			{
				THROW_INVALID_ARGUMENT("The " << _property.name() << " configuration is not node!");
			}

			ret_value = trace.Load(_property, _check);
		}
		else if (_property.name() == TITLE_NAME_ID)
		{
			ret_value = SetID(_property.as_string(), _check);
		}
		else if (_property.name() == TITLE_NAME_NAME)
		{
			ret_value = SetName(_property.as_string(), _check);
		}
		else if (_property.name() == TITLE_NAME_TIME)
		{
			ret_value = SetDate(_property.as_string(), _check);
		}
		else if (_property.name() == TITLE_NAME_PARENT_ID)
		{
			ret_value = SetParentID(_property.as_string(), _check);
		}
		else if (_property.name() == TITLE_NAME_ENABLE)
		{
			ret_value = SetEnable(_property.as_string(), _check);
		}
		else
		{
			THROW_INVALID_ARGUMENT("The " << _property.name() << " configuration is not supported!");
		}
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
		ret_value = false;
	}

	return	ret_value;
}

bool	Object::SetProperties(JSONNode const& _config, bool _check, bool _create)
{
	bool	ret_value = true;

	SetLazyStore(true);

	try
	{
		JSONNode	trace_config = JSONNodeGetTraceNode(_config);

		if (SetProperty(trace_config, _check) == false)
		{	
			ret_value = false;
		}
	}
	catch(ObjectNotFound& e)
	{
	}

	if (ret_value == true)
	{
		for(JSONNode::const_iterator it = _config.begin(); it != _config.end() ; it++)
		{
			if (it->name() != TITLE_NAME_TRACE)
			{
				try
				{
					if (SetProperty(*it, _check) == false)
					{	
						ret_value = false;
						break;
					}
				}
				catch(InvalidArgument& e)
				{
					TRACE_ERROR(e.what());	
					ret_value = false;
					break;
				}
			}
		}
	}

	if (_create)
	{
		ClearUpdatedProperties();
	}

	SetLazyStore(false);

	return	ret_value;
}

bool	Object::GetUpdatedProperties(JSONNode& _properties) const
{
	_properties = updated_properties_;

	return	true;
}

Object::operator JSONNode() 
{
	JSONNode	properties;

	GetProperties(properties);

	return	properties;
}


std::ostream&	operator<<(std::ostream& os, Object& _object)
{
	JSONNode	json = JSONNode(_object);

	return	os << json.write_formatted();
}

bool	Object::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	_field_list.push_back(TITLE_NAME_ID);
	_field_list.push_back(TITLE_NAME_NAME);
	_field_list.push_back(TITLE_NAME_TYPE);
	_field_list.push_back(TITLE_NAME_TIME);
	_field_list.push_back(TITLE_NAME_ENABLE);
	_field_list.push_back(TITLE_NAME_KEEP_ALIVE_INTERVAL);
	_field_list.push_back(TITLE_NAME_LOOP_INTERVAL);
	_field_list.push_back(TITLE_NAME_IP);
	_field_list.push_back(TITLE_NAME_MODULE);
	_field_list.push_back(TITLE_NAME_COMMUNITY);
	_field_list.push_back(TITLE_NAME_TIMEOUT);
	_field_list.push_back(TITLE_NAME_UNIT);
	_field_list.push_back(TITLE_NAME_SCALE);
	_field_list.push_back(TITLE_NAME_UPDATE_INTERVAL);
	_field_list.push_back(TITLE_NAME_SENSOR_ID);

	return	true;
}

std::string	Object::GetTraceName() const
{
	return	std::string(name_);
}

void	Object::SetTrace(bool	_enable)
{
	trace.SetEnable(_enable);
}

int	Object::GetCount()
{
	return	object_map.size();
}

Object*	Object::GetAt(int index)
{
	if((index < 0) || (index >= object_map.size()))
	{
		return	NULL;
	}

	for(std::map<std::string, Object*>::iterator it = object_map.begin() ; it != object_map.end();  it++)
	{
		if (index == 0)
		{
			return	it->second;
		}
		index--;
	}

	return	NULL;
}

Object*	Object::Get(std::string const& _id)
{
	std::map<std::string, Object*>::iterator it = object_map.find(_id);

	if (it == object_map.end())
	{
		return	NULL;	
	}

	return	it->second;
}

bool	Object::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<Object*>(_object) != NULL;
}

std::string ToString(Object::Stat _stat)
{
	switch(_stat)
	{
	case	Object::RUN:	return	std::string("run");
	case	Object::STOP:	return	std::string("stop");
	case	Object::ENABLED:	return	std::string("enabled");
	case	Object::DISABLED:return	std::string("disabled");
	}	
	
	return	std::string("unknown");
}

