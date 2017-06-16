#include <inttypes.h>
#include <iomanip>
#include <sstream>
#include <map>
#include "defined.h"
#include "trace.h"
#include "object.h"
#include "object_manager.h"
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

Object::Object(ValueID const& _id)
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

const	ValueID&	Object::GetID() const
{
	return	id_;
}

bool	Object::SetID(ValueID const& _id)
{
	id_ = _id;
	
	updated_properties_.AppendID(id_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

const	ValueName&	Object::GetName() const
{
	return	name_;
}

bool	Object::SetName(ValueName const& _name)
{
	name_ = _name;

	updated_properties_.AppendName(name_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Object::GetEnable() const
{
	return	enable_;
}

bool	Object::SetEnable(bool _enable)
{
	enable_ = _enable;

	updated_properties_.AppendEnable(ValueBool(enable_));

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
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

	updated_properties_.AppendDate(date_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

const ValueID&	Object::GetParentID() const
{
	return	parent_id_;
}

bool	Object::SetParentID(ValueID const& _parent_id)
{
	parent_id_ = _parent_id;

	updated_properties_.AppendParentID(parent_id_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
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

bool	Object::AddUpdatedProperties(Property const& _property)
{
	const Property*	property = updated_properties_.Get(_property.GetName());
	if (property != NULL)
	{
		updated_properties_.Delete(_property.GetName());
	}

	updated_properties_.Append(_property);

	return	true;
}

bool	Object::ClearUpdatedProperties() 
{
	updated_properties_.Clear();

	return	true;
}

bool	Object::GetProperties(Properties& _properties, Properties::Fields const& _fields) 
{
	if (_fields.id)
	{
		_properties.AppendID(id_);
	}

	if (_fields.name)
	{
		_properties.AppendName(name_);
	}
	
	if (_fields.time)
	{
		_properties.AppendDate(date_);
	}

	if (_fields.enable)
	{
		_properties.AppendEnable(enable_);
	}

	if (_fields.parent_id)
	{
		_properties.AppendParentID(parent_id_);
	}

	return	true;
}

bool	Object::GetProperties(JSONNode& _properties, Properties::Fields const& _fields)
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
#ifdef	PROPERTY_VALUE_STRING_ONLY
		_properties.push_back(JSONNode(TITLE_NAME_TIME, std::to_string(time_t(date_))));
#else
		_properties.push_back(JSONNode(TITLE_NAME_TIME, time_t(date_)));
#endif
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

bool	Object::SetProperty(Property const& _property, Properties::Fields const& _fields)
{
	bool	ret_value = true;

	try
	{
		if (_property.GetName() == TITLE_NAME_ID)
		{
			if (_fields.id)
			{
				const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
				if (!value)
				{
					throw InvalidArgument(_property.GetName());
				}

				ret_value = SetID(value->Get());
			}
		}
		else if (_property.GetName() == TITLE_NAME_NAME)
		{
			if (_fields.name)
			{
				const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
				if (!value)
				{
					throw InvalidArgument(_property.GetName());
				}

				ret_value = SetName(value->Get());
			}
		}
		else if (_property.GetName() == TITLE_NAME_TIME)
		{
			if (_fields.time)
			{
				Date	time;
				const ValueDate*	value = dynamic_cast<const ValueDate*>(_property.GetValue());
				if (!value)
				{
					const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
					if (!string_value)
					{
						throw InvalidArgument(_property.GetName());
					}

					time = string_value->Get();
				}
				else
				{
					time = value->Get();
				}

			
				ret_value = SetDate(time);
			}
		}
		else if (_property.GetName() == TITLE_NAME_PARENT_ID)
		{
			const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
			if (!value)
			{
				throw InvalidArgument(_property.GetName());
			}

			ret_value = SetParentID(value->Get());
		}
		else if (_property.GetName() == TITLE_NAME_ENABLE)
		{
			if (dynamic_cast<const ValueBool*>(_property.GetValue()))
			{
				const ValueBool*	value = dynamic_cast<const ValueBool*>(_property.GetValue());

				ret_value = SetEnable(value->Get());
			}
			else if(dynamic_cast<const ValueString*>(_property.GetValue()))
			{
				ValueBool	value;

				value.Set(dynamic_cast<const ValueString*>(_property.GetValue())->Get());

				ret_value = SetEnable(value.Get());
			}
			else	
			{
				throw InvalidArgument(_property.GetName());
			}
		}
		else
		{
			TRACE_ERROR("Property[" << _property.GetName() << "] not supported!");
			ret_value = false;
		}
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
		ret_value = false;
	}

	return	ret_value;
}

bool	Object::SetProperties(Properties const& _properties, Properties::Fields const& _fields, bool create)
{
	SetLazyStore(create);

	for(auto it = _properties.begin(); it != _properties.end() ; it++)
	{
		SetProperty(*it, _fields);	
	}

	if (create)
	{
		ClearUpdatedProperties();
		SetLazyStore(!create);
	}

	return	true;
}

bool	Object::SetProperties(JSONNode const& _properties, Properties::Fields const& _fields, bool create)
{
	Properties	properties;

	properties.Append(_properties);

	return	SetProperties(properties, _fields, create);
}

bool	Object::GetUpdatedProperties(Properties& _properties) const
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

std::string Object::ToString(Object::Stat _stat)
{
	switch(_stat)
	{
	case	RUN:	return	std::string("run");
	case	STOP:	return	std::string("stop");
	case	ENABLED:	return	std::string("enabled");
	case	DISABLED:return	std::string("disabled");
	}	
	
	return	std::string("unknown");
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

	for(auto it = object_map.begin() ; it != object_map.end();  it++)
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
	auto it = object_map.find(_id);

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
