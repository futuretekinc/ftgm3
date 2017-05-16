#ifndef	OBJECT_H_
#define	OBJECT_H_

#include <iostream>
#include <mutex>
#include <libjson/libjson.h>
#include <list>
#include "value.h"
#include "trace.h"
#include "time2.h"
#include "property.h"


class	ObjectManager;

class	Object
{
public:
	friend class	ObjectManager;
	friend class	Trace;

	enum	Stat
	{
		DISABLED,
		ENABLED,
		RUN,
		STOP
	};

	Object();
	Object(const ValueID& _id);
	virtual	~Object();

	virtual	std::string	GetClassName();

	const	ValueID&	GetID() const;
	const	Date&		GetDate() const;

	const	ValueName&	GetName() const;
			bool		SetName(ValueName const& _name);

			bool		GetEnable() const;
	virtual	void		SetEnable(bool _enable);

	virtual	Stat		GetState() const;
			bool		SetState(Stat _stat);


			bool		HasChanged() const;
	virtual	bool		ApplyChanges();
			bool		GetUpdatedProperties(Properties& _properties) const;
			bool		AddUpdatedProperties(Property const& _property);

	virtual	bool		GetProperties(Properties& _properties) const;
	virtual	Properties	GetProperties() const;
			bool		SetProperty(Property const& _property, bool create = false);
	virtual	bool		SetProperties(Properties const& _properties, bool create = false);
	
	virtual				operator JSONNode();

	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);
	static	std::string ToString(Stat _stat);
	static	int			GetCount();
	static	Object*		Get(std::string const& _name);
	static	Object*		GetAt(int index);

	std::string			GetTraceName() const;
			void		SetTrace(bool	_enable);

	friend std::ostream&	operator<<(std::ostream& os, Object & _object);

protected:
	virtual	bool		SetPropertyInternal(Property const& _property, bool create = false);

	ObjectManager*	parent_;
	std::string	class_name_;
	ValueID		id_;
	ValueName	name_;	
	Date		date_;
	bool		enable_;
	std::mutex	mutex_;

	Properties	updated_properties_;
	Trace		trace;
};


#endif
