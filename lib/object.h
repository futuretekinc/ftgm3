#ifndef	OBJECT_H_
#define	OBJECT_H_

#include <iostream>
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
	Object(std::string const& _id);
	virtual	~Object();

	virtual	const char*	GetClassName();
	


	const std::string&	GetID() const;
			bool		SetID(std::string const& _id, bool _check = false);

	const std::string&	GetName() const;
			bool		SetName(std::string const& _name, bool _check = false);

			bool		GetEnable() const;
	virtual	bool		SetEnable(bool _enable);
	virtual	bool		SetEnable(std::string const& _enable, bool _check = false);

	virtual	Stat		GetState() const;
			bool		SetState(Stat _stat);
			bool		SetState(std::string const& _stat, bool _check = false);

	const	Date&		GetDate() const;
			bool		SetDate(Date const& _date);
			bool		SetDate(std::string const& _Date, bool _check = false);

	const std::string&	GetParentID() const;
			bool		SetParentID(std::string const& _parent, bool _check = false);

			bool		SetLazyStore(bool _enable);

			bool		HasChanged() const;
	virtual	bool		ApplyChanges();

			bool		GetUpdatedProperties(JSONNode& _properties) const;
			bool		ClearUpdatedProperties();

	virtual	bool		GetProperty(JSONNode& _property);
	virtual	bool		GetProperty(uint32_t _type, JSONNode& _property);
	virtual	bool		GetProperties(JSONNode& _properties, uint32_t _fields = PROPERTY_STATIC_FLAG);

	virtual	bool		SetProperty(JSONNode const& _property, bool _check = false);
	virtual	bool		SetProperties(JSONNode const& _properties, bool _check, bool create);
	
	virtual				operator JSONNode();

	static	bool		IsIncludeIn(Object *_object);

	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);

	static	int			GetCount();
	static	Object*		Get(std::string const& _name);
	static	Object*		GetAt(int index);

	std::string			GetTraceName() const;
			void		SetTrace(bool	_enable);
	Trace&				GetTrace();
	//Add SY.KANG
	std::string GetIP();	
	bool    GetWanIP(JSONNode& _property);
	friend std::ostream&	operator<<(std::ostream& os, Object & _object);

protected:

	std::string	parent_id_;
	std::string	id_;
	std::string	name_;	
	Date		date_;
	bool		enable_;
	bool		lazy_store_;

	JSONNode	updated_properties_;
	Trace		trace;
	//Add SY.KANG
	std::string	wan_ip_; 
};

std::string	ToString(Object::Stat _stat);

#endif
