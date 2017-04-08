#ifndef	ENDPOINT_H_
#define	ENDPOINT_H_

#include <map>
#include "active_object.h"
#include "timer.h"
#include "time2.h"

class	Device;

class	Endpoint : public ActiveObject
{
public:
	class	ValueMap : public std::multimap<Date, Value *>
	{
	public:
		ValueMap(uint32_t _limit);
		~ValueMap();

		bool	Add(Date _date, Value const *_value);

	protected:
		uint32_t	limit_;
	};

						Endpoint();
						Endpoint(std::string const& _unit);
	virtual				~Endpoint();	

	virtual	bool		GetProperies(Properties& _properties);
	virtual	bool		SetProperty(Property const& _property, bool create = false);

			bool		SetUnit(std::string const& _unit);
			bool		SetScale(float _scale);
			bool		SetUpdateInterval(Time const& _update_interval);
			bool		SetUpdateInterval(int _update_interval);
			bool		SetUpdateInterval(uint64_t _update_interval);

			uint32_t	GetValueCount();
			Date		GetFirstDate();
			Date		GetLastDate();

			bool		GetPeriodValues(Date const& _begin, Date const& _end, ValueMap& _value_map);

			bool		Attach(Device* _device);
			bool		Detach(Device* _device);

protected:
			void		Preprocess();
			void		Process();
			void		Postprocess();

	virtual	void		UpdateProcess();
	virtual	bool		Add(Date _date, Value const *_value);

	Device*		device_;
	ValueUnit	unit_;
	ValueFloat	scale_;
	ValueFloat	value_;
	Time		update_interval_;
	Timer		update_timer_;

	ValueMap	value_map_;
};

#endif
