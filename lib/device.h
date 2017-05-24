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

	Device(ObjectManager& _manager, ValueType const& _type);
	~Device();

	// Properties operation
	ValueType const& 	GetType() const	{	return	type_;};	
	virtual	bool		IsIncludedIn(ValueType const& _type) 	{	return	(type_ == _type);};

			bool		SetLiveCheckInterval(int _interval, bool _store = true);
			bool		SetLiveCheckInterval(Time const& _interval, bool _store = true);

	virtual	bool		GetProperties(Properties& _properties) const;

			bool		ApplyChanges();	

	virtual	Endpoint*	CreateEndpoint(Properties const& _properties) = 0;

			uint32_t	GetEndpointCount();
	const 	ValueID&	GetEndpointAt(int index);
			bool		GetEndpointList(std::list<ValueID>& _endpoinst_id_list);

	// Utility
	virtual				operator JSONNode();

	static	const	ValueType&	Type();
	static	bool		IsValidType(std::string const& _type);

	static	Device*		Create(ObjectManager& _manager, Properties const& _properties);
	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);

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
	ValueType	type_;
	ValueID		parent_id_;
	Time		live_check_interval_;

	Timer		live_check_timer_;

	std::list<ValueID>	endpoint_id_list_;

	std::mutex	endpoint_schedule_list_lock_;
	std::list<std::pair<ValueID, Timer>>	endpoint_schedule_list_;
};

#endif
