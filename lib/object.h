#ifndef	OBJECT_H_
#define	OBJECT_H_

#include <iostream>
#include "value.h"
#include "time2.h"
#include "property.h"

class	Object
{
public:
	Object();
	Object(const ValueID& _id);

	const	ValueID&	GetID() const;
	const	ValueName&	GetName() const;
	const	Date&		GetDate() const;
			bool		GetEnable() const;

	virtual	bool		GetProperties(Properties& _properties);
	virtual	bool		SetProperty(Property const& _property, bool create = false);
	virtual	bool		SetProperties(Properties const& _properties, bool create = false);

	virtual	void		Print(std::ostream& os) const;

	friend std::ostream&	operator<<(std::ostream& os, Object const& _object);

protected:
	ValueID		id_;
	ValueName	name_;	
	Date		date_;
	bool		enable_;
};

#endif
