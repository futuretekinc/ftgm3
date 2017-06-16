#ifndef	NODE_H_
#define	NODE_H_

#include "active_object.h"
#include "timer.h"

class	ObjectManager;

struct	NodeInfo
{
	NodeInfo();
	NodeInfo(JSONNode const& _json);

	virtual	bool		GetProperties(JSONNode& _properties, Properties::Fields const& _fields = PROPERTY_ALL);

	Properties::Fields	fields;
	ValueID			parent_id;
	ValueID			id;
	ValueName		name;	
	Date			date;
	bool			enable;
	std::string		type;
	std::string		location;
	uint32_t		keep_alive_interval;
	bool			registered;
};

class	Node : public ActiveObject
{
	friend class	ObjectManager;
public:
	enum	TYPE
	{
		GATEWAY,
		DEVICE,
		ENDPOINT
	};

	Node(ObjectManager& _manager, ValueType const& _type);

	virtual	ValueType 	GetType() const;
	virtual	bool		IsIncludedIn(ValueType const& _type);

			bool		SetKeepAliveInterval(uint32_t _interval);
			uint32_t	GetKeepAliveInterval()	{	return	keep_alive_interval_;	};
			Date		TimeOfExpire();

			bool		SetRegistered(bool _registered);
			bool		SetRegistered(std::string const& _registered);
			bool		GetRegistered();

			bool		SetLocation(std::string const& _location);
	const	std::string&	GetLocation();

			bool		SetProperty(Property const& _property, Properties::Fields const& _fields = PROPERTY_ALL);

	virtual	bool		GetProperties(Properties& _properties, Properties::Fields const& _fields = PROPERTY_ALL);
	virtual	bool		GetProperties(JSONNode& _properties, Properties::Fields const& _fields = PROPERTY_ALL);

			bool		ApplyChanges();


			void		Preprocess();
			void		Process();

	static	const	ValueType&	Type();
	static	bool		IsValidType(std::string const& _type);
	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);
	static	bool		IsIncludeIn(Object* _object);

protected:
	ObjectManager&	manager_;
	ValueType		type_;

	std::string		location_;
	uint32_t		keep_alive_interval_;
	Timer			keep_alive_timer_;

	bool			registered_;
};

#endif
