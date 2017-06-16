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

class	Object
{
public:
	friend class	ObjectManager;
	friend class	Trace;
	friend class	TraceMaster;

	enum	Stat
	{
		DISABLED,
		ENABLED,
		RUN,
		STOP
	};

	Object();
	Object(ValueID const& _id);
	virtual	~Object();

	virtual	std::string	GetClassName();

			bool		SetID(ValueID const& _id);
	const	ValueID&	GetID() const;

	const	ValueName&	GetName() const;
			bool		SetName(ValueName const& _name);

			bool		GetEnable() const;
	virtual	bool		SetEnable(bool _enable);

	virtual	Stat		GetState() const;
			bool		SetState(Stat _stat);

	const	Date&		GetDate() const;
			bool		SetDate(Date const& _date);

	const	ValueID&	GetParentID() const;
			bool		SetParentID(ValueID const& _parent);

			bool		SetLazyStore(bool _enable);

			bool		HasChanged() const;
	virtual	bool		ApplyChanges();

			bool		GetUpdatedProperties(Properties& _properties) const;
			bool		AddUpdatedProperties(Property const& _property);
			bool		ClearUpdatedProperties();

	virtual	bool		GetProperties(Properties& _properties, Properties::Fields const& _fields = PROPERTY_ALL);
	virtual	bool		GetProperties(JSONNode& _properties, Properties::Fields const& _fields = PROPERTY_ALL);

	virtual	bool		SetProperty(Property const& _property, Properties::Fields const& _fields = PROPERTY_ALL);
			bool		SetProperties(Properties const& _properties, Properties::Fields const& _fields = PROPERTY_ALL, bool create = false);
			bool		SetProperties(JSONNode const& _properties, Properties::Fields const& _fields = PROPERTY_ALL, bool create = false);
	
	virtual				operator JSONNode();

	static	bool		IsIncludeIn(Object *_object);

	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);
	static	std::string ToString(Stat _stat);

	static	int			GetCount();
	static	Object*		Get(std::string const& _name);
	static	Object*		GetAt(int index);

	std::string			GetTraceName() const;
			void		SetTrace(bool	_enable);

	friend std::ostream&	operator<<(std::ostream& os, Object & _object);

protected:

	ValueID		parent_id_;
	std::string	class_name_;
	ValueID		id_;
	ValueName	name_;	
	Date		date_;
	bool		enable_;
	bool		lazy_store_;
	std::mutex	mutex_;

	Properties	updated_properties_;
	Trace		trace;
};


#endif
