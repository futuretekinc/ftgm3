#ifndef	OBJECT_H_
#define	OBJECT_H_

#include <iostream>
#include <mutex>
#include <libjson/libjson.h>
#include <list>
#include "value.h"
#include "time2.h"
#include "property.h"

class	Object
{
public:
	Object();
	Object(const ValueID& _id);
	~Object();

	const	ValueID&	GetID() const;
	const	ValueName&	GetName() const;
	const	Date&		GetDate() const;
			bool		GetEnable() const;

	virtual	bool		GetProperties(Properties& _properties) const;
	virtual	Properties	GetProperties() const;
	virtual	bool		SetProperty(Property const& _property, bool create = false);
	virtual	bool		SetProperties(Properties const& _properties, bool create = false);

	virtual	operator JSONNode();
	//virtual	JSONNode	GetJSON() const;
	virtual	void		Print(std::ostream& os) const;

	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);

	friend std::ostream&	operator<<(std::ostream& os, Object & _object);

protected:
	ValueID		id_;
	ValueName	name_;	
	Date		date_;
	bool		enable_;
	std::mutex	mutex_;
};

#endif
