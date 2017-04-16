#include <iostream>
#include <iomanip>
#include <typeinfo>
#include "property.h"
#include "device.h"
 
Property::Property(Property const& _property)
{
	first = _property.first;
	second = _property.second->Duplicate();
}

Property::Property(std::string const& _name, int _value)
{
	first = _name;
	second = new ValueInt(_value);
}

Property::Property(std::string const& _name, uint32_t _value)
{
	first = _name;
	second = new ValueUInt32(_value);
}

Property::Property(std::string const& _name, uint64_t _value)
{
	first = _name;
	second = new ValueUInt64(_value);
}

Property::Property(std::string const& _name, bool _value)
{
	first = _name;
	second = new ValueBool(_value);
}

Property::Property(std::string const& _name, float _value)
{
	first = _name;
	second = new ValueFloat(_value);
}

Property::Property(std::string const& _name, double _value)
{
	first = _name;
	second = new ValueFloat(_value);
}

Property::Property(std::string const& _name, char const* _value)
{
	first = _name;
	second = new ValueString(_value);
}

Property::Property(std::string const& _name, std::string const& _value)
{
	first = _name;
	second = new ValueString(_value);
}

Property::Property(std::string const& _name, Date const& _value)
{
	first = _name;
	second = new ValueDate(_value);
}

Property::Property(std::string const& _name, Time const& _value)
{
	first = _name;
	second = new ValueTime(_value);
}

Property::Property(std::string const& _name, Properties const& _value)
{
	first = _name;
	second = new ValueProperties(_value);
}

Property::Property(std::string const& _name, PropertiesList const& _value)
{
	first = _name;
	second = new ValuePropertiesList(_value);
}

Property::~Property()
{
	delete second;
}

const Property& Property::operator=(Property const& _property)
{
	if (second != NULL)
	{
		delete second;	
	}

	first = _property.first;
	second = _property.second->Duplicate();
}

bool	Property::operator=(std::string const& _value)
{
	return	second->Set(_value);
}

bool	Property::Set(std::string const& _value)
{
	return	second->Set(_value);
}

const std::string&	Property::GetName() const
{
	return	first;
}

const Value*		Property::GetValue() const
{
	return	second;
}

std::ostream&	operator<<(std::ostream& os, Property const& _property)
{

	return	os << ToJSON(_property).write_formatted();
}

JSONNode	ToJSON(Property const& _property)
{
	JSONNode	json;

	if (dynamic_cast<ValueInt*>(_property.second))
	{
		json = JSONNode(_property.first,dynamic_cast<ValueInt*>(_property.second)->Get()); 
	}
	else if (dynamic_cast<ValueUInt32*>(_property.second))
	{
		json = JSONNode(_property.first,dynamic_cast<ValueUInt32*>(_property.second)->Get()); 
	}
	else if (dynamic_cast<ValueUInt64*>(_property.second))
	{
		json = JSONNode(_property.first,dynamic_cast<ValueUInt64*>(_property.second)->Get()); 
	}
	else if (dynamic_cast<ValueFloat*>(_property.second))
	{
		json = JSONNode(_property.first,dynamic_cast<ValueFloat*>(_property.second)->Get()); 
	}
	else if (dynamic_cast<ValueBool*>(_property.second))
	{
		json = JSONNode(_property.first,dynamic_cast<ValueBool*>(_property.second)->Get()); 
	}
	else if (dynamic_cast<ValueString*>(_property.second))
	{
		json = JSONNode(_property.first,dynamic_cast<ValueString*>(_property.second)->Get()); 
	}
	else if (dynamic_cast<ValueDate*>(_property.second))
	{
		ValueDate *date = dynamic_cast<ValueDate*>(_property.second);
		json = JSONNode(_property.first,std::string(*date));
	}
	else if (dynamic_cast<ValueTime*>(_property.second))
	{
		ValueTime *time = dynamic_cast<ValueTime*>(_property.second);
		json = JSONNode(_property.first,time_t(time->Get()));
	}
	else if (dynamic_cast<ValueProperties*>(_property.second))
	{	
		ValueProperties *value = dynamic_cast<ValueProperties*>(_property.second);
		
		json = ToJSON(value->Get());

		json.set_name(_property.first);
	}
	else if (dynamic_cast<ValuePropertiesList*>(_property.second))
	{	
		ValuePropertiesList *value = dynamic_cast<ValuePropertiesList*>(_property.second);

		json = ToJSON(value->Get());

		json.set_name(_property.first);
	}


	return	json;
}


///////////////////////////////////////////////////////////////////////////
// Class	Properties
///////////////////////////////////////////////////////////////////////////

Properties::Properties()
{
}

Properties::Properties(Properties const& _properties)
{
	for(auto it = _properties.begin() ; it != _properties.end() ; it++)
	{
		Append(*it);			
	}
}

Properties::Properties(JSONNode const& _json)
{
	if (_json.type() == JSON_NODE)
	{
		Append(_json);
	}
}

Properties::~Properties()
{
}

bool	Properties::Append(Property const& _property)
{
	if (Get(_property.GetName()) != NULL)
	{
		return	false;
	}

	push_back(_property);
	return	true;
}

bool Properties::Append(JSONNode const& _json)
{
	switch(_json.type())
	{
	case	JSON_BOOL: 	
		{
			Append(_json.name(), 	_json.as_bool());	
		}
		break;
	case	JSON_NUMBER: 	
		{
			Append(_json.name(),	(uint32_t)_json.as_int());	
		}
		break;

	case	JSON_STRING:
		{
			Append(_json.name(), 	_json.as_string());	
		}
		break;

	case	JSON_NODE: 	
		{	
			for(auto it = _json.begin(); it != _json.end() ; it++)
			{
				Append(*it);	
			}
		}
		break;

	case	JSON_ARRAY:
		{
			Append(_json.name(), PropertiesList(_json.as_array()));
		}	
	}

	return	true;
}

bool	Properties::Append(std::string const& _name, int _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, uint32_t _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, uint64_t _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, bool _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, float _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, double _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, char const* _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, std::string const& _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, Date const& _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, Time const& _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, Properties const& _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::Append(std::string const& _name, PropertiesList const& _value)
{
	return	Append(Property(_name, _value));
}

bool	Properties::AppendID(std::string const& _id)
{
	if (!ValueID::IsValid(_id))
	{
		return	false;
	}
	return	Append(Property("id", _id));
}

bool	Properties::AppendName(std::string const& _name)
{
	if (!ValueName::IsValid(_name))
	{
		return	false;
	}
	return	Append(Property("name", _name));
}

bool	Properties::AppendEnable(std::string const& _enable)
{
	if ((_enable != "true") || (_enable != "false"))
	{
		return	false;	
	}

	return	Append(Property("enable", _enable));
}

bool	Properties::AppendDeviceType(std::string const& _type)
{
	if (!Device::IsValidType(_type))
	{
		return	false;	
	}

	return	Append(Property("type", _type));
}

bool	Properties::Delete(std::string const& _name)
{
	const Property*	property = NULL;

	for(auto it = begin(); it != end(); it++)
	{
		if (it->GetName() == _name)
		{
			erase(it);
			return	true;
		}
	}

	return	false;
}

void		Properties::Clear()
{
	clear();
}


uint32_t	Properties::Count()
{
	return	size();
}

const Property*	Properties::Get(std::string const& _name) const
{
	const Property*	property = NULL;

	for(auto it = begin(); it != end(); it++)
	{
		if (it->GetName() == _name)
		{
			property = &(*it);

			break;
		}
	}

	return	property;
}

std::ostream&	operator<<(std::ostream& os, Properties const& _properties)
{
	JSONNode	json = ToJSON(_properties);

	return	os << json.write_formatted();
}

JSONNode	ToJSON(Properties const& _properties)
{
	JSONNode	json;

	for(auto it = _properties.begin() ; it != _properties.end(); it++)
	{
		json.push_back(ToJSON(*it));	
	}

	return	json;
}

///////////////////////////////////////////////////////////////////////////
// Class	PropertiesList
///////////////////////////////////////////////////////////////////////////

PropertiesList::PropertiesList()
{
}

PropertiesList::PropertiesList(PropertiesList const& _properties_list)
{
	for(auto it = _properties_list.begin() ; it != _properties_list.end() ; it++)
	{
		push_back(*it);
	}
}

PropertiesList::PropertiesList(JSONNode const& _json)
{
	if (_json.type() == JSON_ARRAY)
	{
		for(auto it = _json.begin() ; it != _json.end() ; it++)
		{
			push_back(Properties(*it));
		}
	}
}

PropertiesList::~PropertiesList()
{
	for(auto it = begin() ; it != end() ; it++)
	{
	}
}

bool	PropertiesList::Append(JSONNode const& _json)
{
	if (_json.type() != JSON_ARRAY)
	{
		return	false;
	}

	for(auto it = _json.begin() ; it != _json.end() ; it++)
	{
		push_back(Properties(*it));
	}
}

bool	PropertiesList::Append(Properties const& _value)
{
	push_back(_value);

	return	true;
}

JSONNode	ToJSON(PropertiesList const& _properties_list)
{
	JSONNode	json(JSON_ARRAY);

	for(auto it = _properties_list.begin() ; it != _properties_list.end() ; it++)
	{
		json.push_back(ToJSON(*it));
	}

	return	json;
}

std::ostream&	operator<<(std::ostream& os, PropertiesList const& _properties_list)
{
	for(auto it = _properties_list.begin(); it != _properties_list.end(); it++)
	{
		os << (*it) << std::endl;
	}
}

