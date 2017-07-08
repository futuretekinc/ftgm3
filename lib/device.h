#ifndef	DEVICE_H_
#define	DEVICE_H_

#include <list>
#include <map>
#include "locker.h"
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

	Device(ObjectManager& _manager, std::string const& _type);
	~Device();

	// Properties operation
	virtual	bool		GetProperty(uint32_t _type, JSONNode& _property);
	//virtual	bool		GetProperties(JSONNode& _properties, Fields const& _fields = PROPERTY_ALL);

	virtual	bool		SetProperty(JSONNode const& _property, bool _check = false);

	virtual	Endpoint*	CreateEndpoint(JSONNode const& _properties);

			uint32_t	GetEndpointCount();
	const std::string&	GetEndpointAt(int index);
			bool		GetEndpointList(std::list<std::string>& _list);
			bool		GetEndpointMap(std::map<std::string, Endpoint*>& _map);

	// Utility
	virtual				operator JSONNode();

	static	const char*	Type();

	static	Device*		Create(ObjectManager& _manager, JSONNode const& _properties);

	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);

	virtual	bool		ReadValue(std::string const& _epid, time_t& _time, std::string& _value);
	virtual	bool		ReadValue(std::string const& _epid, time_t& _time, uint32_t& _value);
	virtual	bool		ReadValue(std::string const& _epid, time_t& _time, bool& _value);

	virtual	bool		ReadValue(uint32_t _index, time_t& _time, bool& _value);
	virtual	bool		ReadValue(uint32_t _index, time_t& _time, uint32_t& _value);
	virtual	bool		ReadValue(uint32_t _index, time_t& _time, std::string& _value);

	virtual	bool		WriteValue(std::string const& _epid, std::string const& _value);
	virtual	bool		WriteValue(std::string const& _epid, uint32_t _value);
	virtual	bool		WriteValue(std::string const& _epid, bool _value);

protected:

			void		Preprocess();
			void		Process();
			void		Postprocess();

	// Endpoint operation
			bool		IsAttached(std::string const& _epid);
	virtual	bool		Attach(std::string const& _epid);
			bool		Detach(std::string const& _epid);
			bool		Detach();

			bool		AddSchedule(std::string const& _id, time_t _time);
			bool		RemoveSchedule(std::string const& _id);

	std::list<std::string>	epid_list_;
	
	Locker							endpoint_schedule_list_lock_;
	std::multimap<time_t, std::string>	endpoint_schedule_list_;
};

#endif
