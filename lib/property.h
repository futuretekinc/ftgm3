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
class	PropertyList;

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
	Property(std::string const& _name, PropertyList const& _vlaue);
	~Property();

	const Property& operator=(Property const& _property);

	const std::string&	GetName() const;
	const Value*		GetValue() const;
	JSONNode			GetJSON() const;

	friend	std::ostream&	::operator<<(std::ostream& os, Property const& _property);
};

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
	bool	Append(std::string const& _name, PropertyList const& _vlaue);

	bool	Delete(std::string const& _name);

	void		Clear();
	uint32_t	Count();

	const Property*	Get(std::string const& _name) const;
	JSONNode	GetJSON() const;


	friend	std::ostream&	::operator<<(std::ostream& os, Properties const& _properties);

};

class	PropertyList : public std::list<Value*>
{
public:
	PropertyList();
	PropertyList(PropertyList const& _properties_list);
	PropertyList(JSONNode const& _json);
	~PropertyList();

	bool	Append(Property const& _property);
	bool	Append(JSONNode const& _json);
	bool	Append(int _value);
	bool	Append(uint32_t _value);
	bool	Append(uint64_t _value);
	bool	Append(bool _value);
	bool	Append(float _value);
	bool	Append(double _value);
	bool	Append(char const* _value);
	bool	Append(std::string const& _value);
	bool	Append(Date const& _value);
	bool	Append(Time const& _value);
	bool	Append(Properties const& _vlaue);
	bool	Append(PropertyList const& _vlaue);

	friend	std::ostream&	::operator<<(std::ostream& os, PropertyList const& _properties_list);
};
#endif
