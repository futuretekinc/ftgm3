#include <inttypes.h>
#include <iomanip>
#include <sstream>
#include <map>
#include "defined.h"
#include "trace.h"
#include "object.h"
#include "object_manager.h"
#include <hashlib++/hashlibpp.h>

static std::map<std::string, Object*>	object_map;

Object::Object()
: parent_(NULL), enable_(false), trace(this)
{
	md5wrapper*	md5 = new md5wrapper;

	date_ = Date::GetCurrentDate();
	std::ostringstream	oss;

	oss << date_.GetMicroSecond();

	id_ = md5->getHashFromString(oss.str());

	name_ = std::string(id_);

	object_map[id_] = this;
}


Object::Object(const ValueID& _id)
:	parent_(NULL), id_(_id), enable_(false)
{
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

const	ValueName&	Object::GetName() const
{
	return	name_;
}

bool	Object::SetName(ValueName const& _name, bool _store)
{
	name_ = _name;

	updated_properties_.AppendName(name_);

	if (_store)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Object::GetEnable() const
{
	return	enable_;
}

bool	Object::SetEnable(bool _enable, bool _store)
{
	enable_ = _enable;

	updated_properties_.AppendEnable(ValueBool(enable_));

	if (_store)
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

bool	Object::SetDate(Date const& _date, bool _store)
{
	date_ = _date;

	updated_properties_.AppendDate(date_);

	if (_store)
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

bool	Object::SetProperty(Property const& _property, bool create)
{
	if (SetPropertyInternal(_property, create))
	{
		return	AddUpdatedProperties(_property);
	}

	return	false;
}

bool	Object::SetPropertyInternal(Property const& _property, bool create)
{
	bool	ret_value = true;

	if (create && (_property.GetName() == OBJECT_FIELD_ID))
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			if (!id_.Set(value->Get()))
			{
				TRACE_ERROR("Failed to set id!");
				return	false;
			}

			TRACE_INFO("The id set to " << id_);
		}
		else
		{
			TRACE_INFO("Property id value type is incorrect!");
		}
	}
	else if (_property.GetName() == OBJECT_FIELD_NAME)
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			return	SetName(value->Get(), !create);
		}
		else
		{
			ret_value = false;
			TRACE_INFO("Property name value type is incorrect!");
		}
	}
	else if (_property.GetName() == OBJECT_FIELD_DATE)
	{
		if (create)
		{
			const ValueDate*	value = dynamic_cast<const ValueDate*>(_property.GetValue());
			if (value != NULL)
			{
				return	SetDate(value->Get(), !create);
			}
			else
			{
				const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
				if (string_value != NULL)
				{
					Date	date;
					
					date = string_value->Get();
					return	SetDate(date, !create);
				}
				else
				{
					ret_value = false;
					TRACE_INFO("Property date value type is incorrect!");
				}
			}
		}
		else
		{
			ret_value = false;
			TRACE_INFO("The date of the device is only set possible at creation time.");
		}
	}
	else if (_property.GetName() == OBJECT_FIELD_ENABLE)
	{
		bool	enable	= false;
		const ValueBool*	value = dynamic_cast<const ValueBool*>(_property.GetValue());
		if (value != NULL)
		{
			enable = value->Get();
		}
		else
		{
			const ValueString*	value2 = dynamic_cast<const ValueString*>(_property.GetValue());
			if (value2 != NULL)
			{
				if ((value2->Get() == "yes") || (value2->Get() == "on") || (value2->Get() == "1") || (value2->Get() == "true"))
				{
					enable = true;
				}
				else if ((value2->Get() == "no") || (value2->Get() == "off") || (value2->Get() == "0") || (value2->Get() == "false"))
				{
					enable = false;
				}
				else
				{
					ret_value = false;	
					TRACE_INFO("Property enable value is incorrect!");
				}

			}
			else
			{
				ret_value = false;
				TRACE_INFO("Property enable value type is incorrect!");
			}
		}

		if (ret_value == true)
		{
			ret_value = SetEnable(enable, !create);
		}
	}
	else
	{
		TRACE_ERROR("Property[" << _property.GetName() << "] not supported!");
		ret_value = false;
	}

	return	ret_value;
}

bool	Object::GetProperties(Properties& _properties) const
{
	_properties.AppendID(id_);
	_properties.AppendName(name_);
	_properties.AppendDate(date_);
	_properties.AppendEnable(enable_);

	return	true;
}

Properties	Object::GetProperties() const
{
	Properties	properties;

	properties.AppendID(std::string(id_));
	properties.AppendName(name_);
	properties.AppendDate(date_);
	properties.AppendEnable(enable_);

	return	properties;
}

bool	Object::SetProperties(Properties const& _properties, bool create)
{
	for(auto it = _properties.begin(); it != _properties.end() ; it++)
	{
		SetPropertyInternal(*it, create);	
	}

	return	true;
}

bool	Object::GetUpdatedProperties(Properties& _properties) const
{
	_properties = updated_properties_;

	return	true;
}

Object::operator JSONNode()
{
	Properties	properties;

	GetProperties(properties);

	return	ToJSON(properties);
}


std::ostream&	operator<<(std::ostream& os, Object& _object)
{
	JSONNode	json = JSONNode(_object);

	return	os << json.write_formatted();
}

bool	Object::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	_field_list.push_back(OBJECT_FIELD_ID);
	_field_list.push_back(OBJECT_FIELD_NAME);
	_field_list.push_back(OBJECT_FIELD_TYPE);
	_field_list.push_back(OBJECT_FIELD_DATE);
	_field_list.push_back(OBJECT_FIELD_ENABLE);
	_field_list.push_back(OBJECT_FIELD_DEVICE_ID);
	_field_list.push_back(OBJECT_FIELD_LIVE_CHECK_INTERVAL);
	_field_list.push_back(OBJECT_FIELD_LOOP_INTERVAL);
	_field_list.push_back(OBJECT_FIELD_IP);
	_field_list.push_back(OBJECT_FIELD_MODULE);
	_field_list.push_back(OBJECT_FIELD_COMMUNITY);
	_field_list.push_back(OBJECT_FIELD_TIMEOUT);
	_field_list.push_back(OBJECT_FIELD_UNIT);
	_field_list.push_back(OBJECT_FIELD_SCALE);
	_field_list.push_back(OBJECT_FIELD_UPDATE_INTERVAL);
	_field_list.push_back(OBJECT_FIELD_SENSOR_ID);

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
	trace.Enable(_enable);
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
