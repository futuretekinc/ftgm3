#ifndef	DEVICE_H_
#define	DEVICE_H_

#include <list>
#include <mutex>
#include "node.h"
#include "timer.h"
#include "property.h"
#include "KompexSQLiteStatement.h"

class	Endpoint;
class	ObjectManager;

class	Device : public Node
{
	friend class	Endpoint;
	friend class	ObjectManager;

public:

	Device(ObjectManager& _manager, ValueType const& _type);
	~Device();

	// Properties operation
	virtual	bool		SetProperty(Property const& _property, Properties::Fields const& _fields = PROPERTY_ALL);

	virtual	bool		GetProperties(Properties& _properties, Properties::Fields const& _fields = PROPERTY_ALL);
	virtual	bool		GetProperties(JSONNode& _properties, Properties::Fields const& _fields = PROPERTY_ALL);

	virtual	Endpoint*	CreateEndpoint(Properties const& _properties) = 0;

			uint32_t	GetEndpointCount();
	const 	ValueID&	GetEndpointAt(int index);
			bool		GetEndpointList(std::list<ValueID>& _list);
			bool		GetEndpointMap(std::map<std::string, Endpoint*>& _map);

	// Utility
	virtual				operator JSONNode();

	static	bool		IsValidType(ValueType const& _type);

	static	Device*		Create(ObjectManager& _manager, Properties const& _properties);
	static	Device*		Create(ObjectManager& _manager, JSONNode const& _properties);

	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);

	virtual	bool		ReadValue(std::string const& _endpoint_id, std::string& _value) = 0;
	virtual	bool		WriteValue(std::string const& _endpoint_id, std::string const& _value);

protected:

			void		Preprocess();
			void		Process();
			void		Postprocess();

	// Endpoint operation
			bool		IsAttached(ValueID const& _endpoint_id);
	virtual	bool		Attach(ValueID const& _endpoint_id);
			bool		Detach(ValueID const& _endpiont_id);
			bool		Detach();

			bool		AddSchedule(ValueID const& _id, Timer const& _timer);
			bool		RemoveSchedule(ValueID const& _id);

	std::list<ValueID>	endpoint_id_list_;

	std::mutex	endpoint_schedule_list_lock_;
	std::list<std::pair<ValueID, Timer>>	endpoint_schedule_list_;
};

#endif
