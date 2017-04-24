#include <inttypes.h>
#include <iomanip>
#include <sstream>
#include "trace.h"
#include "object.h"
#include "object_manager.h"
#include <hashlib++/hashlibpp.h>

Object::Object()
: parent_(NULL), enable_(false)
{
	md5wrapper*	md5 = new md5wrapper;

	date_ = Date::GetCurrentDate();
	std::ostringstream	oss;

	oss << date_.GetMicroSecond();

	id_ = md5->getHashFromString(oss.str());

	name_ = std::string(id_);
}


Object::Object(const ValueID& _id)
:	parent_(NULL), id_(_id), enable_(false)
{
	name_ = std::string(id_);
}

Object::~Object()
{
	
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

const	Date&		Object::GetDate() const
{
	return	date_;
}

bool	Object::GetEnable() const
{
	return	enable_;
}

void	Object::SetEnable(bool _enable)
{
	enable_ = _enable;
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

bool	Object::SetProperty(Property const& _property, bool create)
{
	bool	ret_value = false;

	if (create && (_property.GetName() == "id"))
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			ret_value = id_.Set(value->Get());
			TRACE_INFO << "The id of object[" << id_ <<"] was set to " << id_ << Trace::End;
		}
		else
		{
			TRACE_INFO << "Property id value type is incorrect!" << Trace::End;
		}
	}
	else if (_property.GetName() == "name")
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			ret_value = name_.Set(value->Get());
			TRACE_INFO << "The name of object[" << id_ <<"] was set to " << name_ << Trace::End;
		}
		else
		{
			TRACE_INFO << "Property name value type is incorrect!" << Trace::End;
		}
	}
	else if (_property.GetName() == "date")
	{
		if (create)
		{
			const ValueDate*	value = dynamic_cast<const ValueDate*>(_property.GetValue());
			if (value != NULL)
			{
				date_ = value->Get();
				TRACE_INFO << "The date of object[" << id_ <<"] was set to " << date_ << Trace::End;
				return	true;
			}

			const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
			if (string_value != NULL)
			{
				date_ = string_value->Get();
				TRACE_INFO << "The date of object[" << id_ <<"] was set to " << date_ << "[" << string_value->Get() << "]" << Trace::End;
				return	true;
			}

			TRACE_INFO << "Property date value type is incorrect!" << Trace::End;
		}
		else
		{
			TRACE_INFO << "The date of the device is only set possible at creation time." << Trace::End;
		}
	}
	else if (_property.GetName() == "enable")
	{
		const ValueBool*	value = dynamic_cast<const ValueBool*>(_property.GetValue());
		if (value != NULL)
		{
			enable_ = value->Get();
			TRACE_INFO << "The enable of object[" << id_ <<"] was set to " << enable_ << Trace::End;
			return	true;
		}

		const ValueString*	value2 = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value2 != NULL)
		{
			if ((value2->Get() == "yes") || (value2->Get() == "on") || (value2->Get() == "1") || (value2->Get() == "true"))
			{
				enable_ = true;
			}
			else if ((value2->Get() == "no") || (value2->Get() == "off") || (value2->Get() == "0") || (value2->Get() == "false"))
			{
				enable_ = false;
			}
			else
			{
				TRACE_INFO << "Property enable value is incorrect!" << Trace::End;
				return	false;	
			}

			TRACE_INFO << "The enable of object[" << id_ <<"] was set to " << enable_ << Trace::End;
			return	true;
		}

		TRACE_INFO << "Property enable value type is incorrect!" << Trace::End;
	}

	return	false;
}

bool	Object::GetProperties(Properties& _properties) const
{
	_properties.Append("id", id_);
	_properties.Append("name", name_);
	_properties.Append("date", date_);
	_properties.Append("enable", enable_);

	return	true;
}

Properties	Object::GetProperties() const
{
	Properties	properties;

	properties.Append("id", std::string(id_));
	properties.Append("name", name_);
	properties.Append("date", date_);
	properties.Append("enable", enable_);

	return	properties;
}

bool	Object::SetProperties(Properties const& _properties, bool create)
{
	for(auto it = _properties.begin(); it != _properties.end() ; it++)
	{
		SetProperty(*it, create);	
	}

	if ((!create) && (parent_ != NULL))
	{
		parent_->UpdateProperties(id_);	
	}

	return	true;
}

Object::operator JSONNode()
{
	Properties	properties;

	GetProperties(properties);

	return	ToJSON(properties);
}


void	Object::Print(std::ostream& os) const
{
	os << std::setw(16) << "ID : " << id_ << std::endl;
	os << std::setw(16) << "Name : " << name_ << std::endl;
	os << std::setw(16) << "Date : " << date_ << std::endl;
	os << std::setw(16) << "Enable : " << enable_ << std::endl;
}

std::ostream&	operator<<(std::ostream& os, Object& _object)
{
	JSONNode	json = JSONNode(_object);

	return	os << json.write_formatted();
}

bool	Object::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	_field_list.push_back("id");
	_field_list.push_back("name");
	_field_list.push_back("type");
	_field_list.push_back("date");
	_field_list.push_back("enable");
	_field_list.push_back("device_id");
	_field_list.push_back("live_check_interval");
	_field_list.push_back("loop_interval");
	_field_list.push_back("ip");
	_field_list.push_back("module");
	_field_list.push_back("community");
	_field_list.push_back("timeout");
	_field_list.push_back("unit");
	_field_list.push_back("scale");
	_field_list.push_back("update_interval");
	_field_list.push_back("sensor_id");

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
