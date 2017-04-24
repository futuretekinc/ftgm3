#ifndef	PROPERTY_H_
#define	PROPERTY_H_

#include <utility>
#include <map>
#include <list>
#include <iostream>
#include <libjson/libjson.h>
#include "value.h"
#include "time2.h"

class	Properties;
class	PropertiesList;

class	Property : public std::pair<std::string,Value*>
{
public:
	Property(Property const& _property);
	Property(std::string const& _name, int _value);
	Property(std::string const& _name, uint32_t _value);
	Property(std::string const& _name, uint64_t _value);
	Property(std::string const& _name, bool _value);
	Property(std::string const& _name, float _value);
	Property(std::string const& _name, double _value);
	Property(std::string const& _name, char const* _value);
	Property(std::string const& _name, std::string const& _value);
	Property(std::string const& _name, Date const& _value);
	Property(std::string const& _name, Time const& _value);
	Property(std::string const& _name, Properties const& _vlaue);
	Property(std::string const& _name, PropertiesList const& _vlaue);
	~Property();

	const Property& operator=(Property const& _property);
			bool		operator=(std::string const& _vlaue);
			bool		Set(std::string const& _vlaue);

	const std::string&	GetName() const;
	const Value*		GetValue() const;
	//operator	JSONNode();
	friend	JSONNode	ToJSON(Property const& _property);
	friend	std::ostream&	::operator<<(std::ostream& os, Property const& _property);
};

JSONNode	ToJSON(Property const& _property);

class	Properties : public std::list<Property>
{
public:
	Properties();
	Properties(Properties const& _properties);
	Properties(JSONNode const& _json);
	~Properties();

	bool	Append(Property const& _property);
	bool	Append(JSONNode const& _json);
	bool	Append(std::string const& _name, int _value);
	bool	Append(std::string const& _name, uint32_t _value);
	bool	Append(std::string const& _name, uint64_t _value);
	bool	Append(std::string const& _name, bool _value);
	bool	Append(std::string const& _name, float _value);
	bool	Append(std::string const& _name, double _value);
	bool	Append(std::string const& _name, char const* _value);
	bool	Append(std::string const& _name, std::string const& _value);
	bool	Append(std::string const& _name, Date const& _value);
	bool	Append(std::string const& _name, Time const& _value);
	bool	Append(std::string const& _name, Properties const& _vlaue);
	bool	Append(std::string const& _name, PropertiesList const& _vlaue);

	bool	AppendID(std::string const& _id);
	bool	AppendName(std::string const& _name);
	bool	AppendEnable(std::string const& _enable);
	bool	AppendDeviceType(std::string const& _type);
	bool	AppendEndpointType(std::string const& _type);

	bool	Delete(std::string const& _name);

	void		Clear();
	uint32_t	Count();

	const Property*	Get(std::string const& _name) const;

	friend	JSONNode		ToJSON(Properties const& _properties);
	friend	std::ostream&	::operator<<(std::ostream& os, Properties const& _properties);

};

JSONNode		ToJSON(Properties const& _properties);

class	PropertiesList : public std::list<Properties>
{
public:
	PropertiesList();
	PropertiesList(PropertiesList const& _properties_list);
	PropertiesList(JSONNode const& _json);
	~PropertiesList();

	bool	Append(JSONNode const& _json);
	bool	Append(Properties const& _vlaue);

	
	friend	JSONNode		ToJSON(PropertiesList const& _properties_list);

	friend	std::ostream&	::operator<<(std::ostream& os, PropertiesList const& _properties_list);
};

JSONNode		ToJSON(PropertiesList const& _properties_list);

#endif
