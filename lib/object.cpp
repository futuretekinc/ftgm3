#include <iomanip>
#include "object.h"

Object::Object()
{
}


Object::Object(const ValueID& _id)
:	id_(_id), enable_(false)
{
	name_ = id_.ToString();
	date_ = Date::GetCurrentDate();
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

bool	Object::GetProperties(Properties& _properties)
{
	_properties.Append("id", id_);
	_properties.Append("name", name_);
	_properties.Append("date", date_);
	_properties.Append("enable", enable_);

	return	true;
}

bool	Object::SetProperties(Properties const& _properties, bool create)
{
	for(auto it = _properties.begin(); it != _properties.end() ; it++)
	{
		SetProperty(*it, create);	
	}

	return	true;
}

void	Object::Print(std::ostream& os) const
{
	os << std::setw(16) << "ID : " << id_ << std::endl;
	os << std::setw(16) << "Name : " << name_ << std::endl;
	os << std::setw(16) << "Date : " << date_ << std::endl;
	os << std::setw(16) << "Enable : " << enable_ << std::endl;
}

std::ostream&	operator<<(std::ostream& os, Object const& _object)
{
	_object.Print(os);
	return	os;
}
