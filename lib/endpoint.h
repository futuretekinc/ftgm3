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

	enum	Type
	{
		S_TEMPERATURE,
		S_HUMIDITY,
		S_VOLTAGE,
		S_CURRENT,
		S_PRESSURE,
		S_DI,
		A_DO
	};

	class	ValueMap : public std::multimap<Date, Value *>
	{
	public:
		ValueMap(uint32_t _limit);
		~ValueMap();

		bool	Add(Date const& _date, Value const* _value);

	protected:
		uint32_t	limit_;
	};

						Endpoint();
	virtual				~Endpoint();	

	virtual	Type		GetType() const = 0;

			bool		SetUnit(std::string const& _unit);
	const	ValueUnit&	GetUnit() const;

			float		GetScale() const;
			bool		SetScale(float _scale);
			bool		SetScale(std::string const& _scale);

			std::string	GetSensorID() const;
			bool		SetSensorID(std::string const& _sensor_id);

			uint64_t	GetUpdateInterval();
			bool		SetUpdateInterval(Time const& _update_interval);
			bool		SetUpdateInterval(uint64_t _update_interval);
			bool		SetUpdateInterval(std::string const& _scale);

	virtual	bool		GetProperties(Properties& _properties) const;
	virtual	bool		SetProperty(Property const& _property, bool create = false);

	virtual	bool		IsValid(const ValueFloat& _value);

			uint32_t	GetDataCount();
			Date		GetDateOfFirstData();
			Date		GetDateOfLastData();

			bool		GetDataForPeriod(Date const& _begin, Date const& _end, ValueMap& _value_map);
			bool		DelDataForPeriod(Date const& _begin, Date const& _end);

			Device*		GetDevice();

			bool		Attach(Device* _device);
			bool		Detach(Device* _device);

			void		Start();
			void		Stop();

	static	bool		IsValidType(std::string const& _type);

	static	Endpoint*	Create(Properties const& _properties);
	static	Endpoint*	Get(std::string const& _id);
	static	uint32_t	GetList(std::list<Endpoint*>& _device_list);
	static	uint32_t	Count();
	static	uint32_t	UnattachedCount();
	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);
	static	std::string	ToString(Type _type);

protected:
			void		Preprocess();
			void		Process();
			void		Postprocess();

	virtual	void		UpdateProcess();
	virtual	bool		Add(Date const& _date, Value const* _value);

	Device*		device_;
	bool		active_;
	std::string	sensor_id_;
	ValueUnit	unit_;
	ValueFloat	scale_;
	ValueFloat	value_;
	Time		update_interval_;
	Timer		update_timer_;

	ValueMap	value_map_;
};

#endif
