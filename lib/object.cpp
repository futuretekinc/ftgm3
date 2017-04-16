#include <iomanip>
#include "object.h"

Object::Object()
{
}


Object::Object(const ValueID& _id)
:	id_(_id), enable_(false)
{
	name_ = std::string(id_);
	date_ = Date::GetCurrentDate();
}

Object::~Object()
{
	
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

bool	Object::SetProperty(Property const& _property, bool create)
{
	if (create && (_property.GetName() == "id"))
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			return	id_.Set(value->Get());
		}
	}
	else if (_property.GetName() == "name")
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			return	name_.Set(value->Get());
		}
	}
	else if (_property.GetName() == "date")
	{
		const ValueDate*	value = dynamic_cast<const ValueDate*>(_property.GetValue());
		if (value != NULL)
		{
			date_ = value->Get();
			return	true;
		}
	}
	else if (_property.GetName() == "enable")
	{
		const ValueBool*	value = dynamic_cast<const ValueBool*>(_property.GetValue());
		if (value != NULL)
		{
			enable_ = value->Get();
			return	true;
		}
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
	_field_list.push_back("live_check_interval");
	_field_list.push_back("loop_interval");
	_field_list.push_back("ip");
	_field_list.push_back("module");
	_field_list.push_back("community");
	_field_list.push_back("timeout");

	return	true;
}
