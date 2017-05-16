#ifndef	DEVICE_H_
#define	DEVICE_H_

#include <list>
#include <mutex>
#include "active_object.h"
#include "timer.h"
#include "property.h"
#include "KompexSQLiteStatement.h"

class	Endpoint;

class	Device : public ActiveObject
{
	friend class	Endpoint;
	friend class	ObjectManager;

public:

	enum	Type
	{
		SNMP,
		FTE,
		MBTCP,
	};

	Device(ObjectManager& _manager, Type _type);
	~Device();

	// Properties operation
	virtual	Type		GetType() const = 0;
	virtual	bool		IsIncludedIn(Type _type) = 0;

			bool		SetLiveCheckInterval(int _interval);
			bool		SetLiveCheckInterval(Time const& _interval);

	virtual	bool		GetProperties(Properties& _properties) const;

			bool		ApplyChanges();	

	virtual	Endpoint*	CreateEndpoint(Properties const& _properties) = 0;

			uint32_t	GetEndpointCount();
	const 	ValueID&	GetEndpointAt(int index);
			bool		GetEndpointList(std::list<ValueID>& _endpoinst_id_list);

	// Utility
	virtual				operator JSONNode();

	static	bool		IsValidType(std::string const& _type);

	static	Device*		Create(ObjectManager& _manager, Properties const& _properties);
	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);
	static	std::string	ToString(Device::Type _type);

	virtual	bool		ReadValue(std::string const& _endpoint_id, Value* _value) = 0;

protected:
	virtual	bool		SetPropertyInternal(Property const& _property, bool create = false);

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

	ObjectManager&	manager_;
	ValueID		parent_id_;
	Time		live_check_interval_;

	Timer		live_check_timer_;

	std::list<ValueID>	endpoint_id_list_;

	std::mutex	endpoint_schedule_list_lock_;
	std::list<std::pair<ValueID, Timer>>	endpoint_schedule_list_;
};

#endif
