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
public:
	friend class	Endpoint;
	friend class	ObjectManager;
	
	enum	Type
	{
		SNMP,
		FTE,
		MBTCP,
	};

	Device(Type _type);
	~Device();

	// Properties operation
	virtual	Type		GetType() const = 0;
	virtual	bool		IsIncludedIn(Type _type) = 0;

			bool		SetLiveCheckInterval(int _interval);
			bool		SetLiveCheckInterval(Time const& _interval);

	virtual	bool		GetProperties(Properties& _properties) const;
	virtual	bool		SetProperty(Property const& _property, bool create = false);


	virtual	Endpoint*	CreateEndpoint(Properties const& _properties) = 0;

			uint32_t	GetEndpointCount();
			Endpoint*	GetEndpoint(std::string const& _endpoint_id);
			bool		GetEndpointList(std::list<Endpoint*>& _endpoinst_list);

	// Utility
	virtual				operator JSONNode();
			void		Print(std::ostream& os) const;

	static	bool		IsValidType(std::string const& _type);

	static	Device*		Create(Properties const& _properties);
	static	uint32_t	Count();
	static	Device*		Get(std::string const& _id);
	static	Device*		Get(uint32_t _index);

	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);
	static	uint32_t	GetList(Type _type, std::list<Device*>& _device_list);
	static	std::string	ToString(Device::Type _type);

	virtual	bool		ReadValue(std::string const& _endpoint_id, Value* _value) = 0;

protected:
			void		Preprocess();
			void		Process();
			void		Postprocess();

	// Endpoint operation
	virtual	bool		Attach(Endpoint* _endpoint);
			bool		Detach(Endpoint* _endpoint);
			bool		Detach(std::string const& _endpoint_id);

			bool		AddSchedule(ValueID const& _id, Timer const& _timer);
			bool		RemoveSchedule(ValueID const& _id);
	
	ValueID		parent_id_;
	Time		live_check_interval_;

	Timer		live_check_timer_;

	std::mutex	endpoint_list_lock_;
	std::list<Endpoint*>	endpoint_list_;

	std::mutex	endpoint_schedule_list_lock_;
	std::list<std::pair<ValueID, Timer>>	endpoint_schedule_list_;
};

#endif
