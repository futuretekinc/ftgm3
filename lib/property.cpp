#include <iostream>
#include <iomanip>
#include "property.h"
 
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
	return	os << std::setw(16) << _property.GetName() << " : " << *_property.GetValue();
}

JSONNode	Property::GetJSON()  const
{
	JSONNode	json;

	if (dynamic_cast<ValueInt*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueInt*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueUInt32*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueUInt32*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueUInt64*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueUInt64*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueFloat*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueFloat*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueBool*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueBool*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueString*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueString*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueDate*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueDate*>(second)->ToString()); 
	}
	else if (dynamic_cast<ValueTime*>(second))
	{
		json = JSONNode(first,time_t(dynamic_cast<ValueTime*>(second)->Get())); 
	}
	else if (dynamic_cast<ValueProperties*>(second))
	{	
		ValueProperties *value = dynamic_cast<ValueProperties*>(second);

		json = value->Get().GetJSON();
		json.set_name(first);
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
	Append(_json);
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
	for(auto it = _properties.begin(); it != _properties.end(); it++)
	{
		os << (*it) << std::endl;
	}

	return	os;
}

JSONNode	Properties::GetJSON() const
{
	JSONNode	json;

	for(auto it = begin() ; it != end(); it++)
	{
		json.push_back(it->GetJSON());	
	}

	return	json;
}

///////////////////////////////////////////////////////////////////////////
// Class	PropertiesList
///////////////////////////////////////////////////////////////////////////

PropertyList::PropertyList(PropertyList const& _properties_list)
{
	for(auto it = _properties_list.begin() ; it != _properties_list.end() ; it++)
	{
		push_back((*it)->Duplicate());	
	}
}

PropertyList::PropertyList(JSONNode const& _json)
{
	if (_json.type() == JSON_ARRAY)
	{
		for(auto it = _json.begin() ; it != _json.end() ; it++)
		{
		}
	}
}

PropertyList::~PropertyList()
{
	for(auto it = begin() ; it != end() ; it++)
	{
		delete *it;
	}
}

bool	PropertyList::Append(int _value)
{
	return	Append(new ValueInt(_value));
}

bool	PropertyList::Append(uint32_t _value)
{
	return	Append(new ValueUInt32(_value));
}

bool	PropertyList::Append(uint64_t _value)
{
	return	Append(new ValueUInt64(_value));
}

bool	PropertyList::Append(bool _value)
{
	return	Append(new ValueBool(_value));
}

bool	PropertyList::Append(float _value)
{
	return	Append(new ValueFloat(_value));
}

bool	PropertyList::Append(double _value)
{
	return	Append(new ValueFloat(_value));
}

bool	PropertyList::Append(char const* _value)
{
	return	Append(new ValueString(_value));
}

bool	PropertyList::Append(std::string const& _value)
{
	return	Append(new ValueString(_value));
}

bool	PropertyList::Append(Date const& _value)
{
	return	Append(new ValueDate(_value));
}

bool	PropertyList::Append(Time const& _value)
{
	return	Append(new ValueTime(_value));
}

bool	PropertyList::Append(Properties const& _value)
{
	return	Append(new ValueProperties(_value));
}

bool	PropertyList::Append(PropertyList const& _value)
{
	return	Append(new ValuePropertyList(_value));
}

