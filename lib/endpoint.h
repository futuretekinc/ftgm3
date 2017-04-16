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
	friend class	Device;

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
	const	ValueUnit&	GetUnit() const;

			bool		SetScale(float _scale);
			float		GetScale() const;

			bool		SetUpdateInterval(Time const& _update_interval);
			bool		SetUpdateInterval(uint64_t _update_interval);
			uint64_t	GetUpdateInterval();

			uint32_t	GetValueCount();
			Date		GetFirstDate();
			Date		GetLastDate();

			bool		GetPeriodValues(Date const& _begin, Date const& _end, ValueMap& _value_map);
			bool		RemovePeriodValues(Date const& _begin, Date const& _end);

			bool		Attach(Device* _device);
			bool		Detach(Device* _device);

			void		Start();
			void		Stop();
	static	Endpoint*	Create(Properties const& _properties);
	static	Endpoint*	Get(std::string const& _id);
	static	uint32_t	Count();
	static	uint32_t	UnattachedCount();

protected:
			void		Preprocess();
			void		Process();
			void		Postprocess();

	virtual	void		UpdateProcess();
	virtual	bool		Add(Date _date, Value const *_value);

	Device*		device_;
	bool		active_;
	ValueUnit	unit_;
	ValueFloat	scale_;
	ValueFloat	value_;
	Time		update_interval_;
	Timer		update_timer_;

	ValueMap	value_map_;
};

#endif
