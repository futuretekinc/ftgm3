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
	
	enum	Type
	{
		SNMP,
		MBTCP
	};

	Device(Type _type);
	~Device();

	// Properties operation
			Type		GetType();

			bool		SetLiveCheckInterval(int _interval);
			bool		SetLiveCheckInterval(Time const& _interval);

	virtual	bool		GetProperies(Properties& _properties) const;
	virtual	bool		SetProperty(Property const& _property, bool create = false);

	// Endpoint operation
			bool		Attach(Endpoint* _endpoint);
			bool		Detach(Endpoint* _endpoint);
			bool		Detach(std::string const& _endpoint_id);

			Endpoint*	GetEndpoint(std::string const& _endpoint_id);

	// Utility
	virtual	operator JSONNode();
			void		Print(std::ostream& os) const;

	static	bool		IsValidType(std::string const& _type);

	static	Device*		Create(Properties const& _properties);
	static	Device*		Create(JSONNode const& _node);
	static	uint32_t	Count();
	static	Device*		Get(std::string const& _id);
	static	Device*		Get(uint32_t _index);
	static	uint32_t	GetList(Type _type, std::list<Device*>& _device_list);

protected:
	void		Preprocess();
	void		Process();
	void		Postprocess();

	bool		AddSchedule(ValueID const& _id, Timer const& _timer);
	bool		RemoveSchedule(ValueID const& _id);
	
	Type		type_;
	ValueID		parent_id_;
	Time		live_check_interval_;

	Timer		live_check_timer_;

	std::mutex	endpoint_list_lock_;
	std::list<Endpoint*>	endpoint_list_;

	std::mutex	endpoint_schedule_list_lock_;
	std::list<std::pair<ValueID, Timer>>	endpoint_schedule_list_;
};

std::string		ToString(Device::Type _type);

#endif
