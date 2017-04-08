#ifndef	DEVICE_H_
#define	DEVICE_H_

#include <list>
#include "active_object.h"
#include "timer.h"
#include "property.h"

class	Endpoint;

class	Device : public ActiveObject
{
public:
	Device();
	~Device();


	// Properties operation
			bool		SetLiveCheckInterval(int _interval);
			bool		SetLiveCheckInterval(Time const& _interval);

	virtual	bool		GetProperies(Properties& _properties);
	virtual	bool		SetProperty(Property const& _property, bool create = false);

	// Endpoint operation
			bool		Attach(Endpoint* _endpoint);
			bool		Detach(Endpoint* _endpoint);
			bool		Detach(std::string const& _endpoint_id);

	// Utility
			void		Print(std::ostream& os) const;

	static	Device*		Create(Properties const& _properties);
	static	uint32_t	Count();
	static	Device*		Get(std::string const& _id);

protected:
	void		Preprocess();
	void		Process();
	void		Postprocess();

	ValueID		parent_id_;
	Time		live_check_interval_;

	Timer		live_check_timer_;

	std::list<Endpoint*>	endpoint_list_;
};

#endif
