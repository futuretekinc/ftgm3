#ifndef	ENDPOINT_H_
#define	ENDPOINT_H_

#include <map>
#include "node.h"
#include "timer.h"
#include "time2.h"

#define	MSG_TYPE_ENDPOINT				0x00020000
#define	MSG_TYPE_ENDPOINT_UPDATED		(MSG_TYPE_ENDPOINT + 1)
#define	MSG_TYPE_ENDPOINT_REPORT		(MSG_TYPE_ENDPOINT + 2)

class	ObjectManager;

class	Endpoint : public Node
{
	friend 	class	Device;
	friend	class	ObjectManager;
public:

	class	ValueMap : public std::multimap<time_t, std::string>
	{
	public:
		ValueMap(uint32_t _limit = 100);
		~ValueMap();

		bool		Add(time_t _time, std::string const& _value);
		uint32_t	GetMaxCount()	{	return	limit_;	};
	protected:
		uint32_t	limit_;
	};

						Endpoint(ObjectManager& _manager, std::string const& _type);
						Endpoint(ObjectManager& _manager, std::string const& _type, std::string const& _unit);
	virtual				~Endpoint();	

	const std::string&	GetUnit() const;
			bool		SetUnit(std::string const& _unit, bool _check = false);

			float		GetScale() const;
			bool		SetScale(float _scale);
			bool		SetScale(std::string const& _scale, bool _check = false);

			uint32_t	GetSensorID() const;
			bool		SetSensorID(uint32_t _sensor_id, bool _check = false);

			uint32_t	GetCorrectionInterval();
			bool		SetCorrectionInterval(Time const& _interval);
			bool		SetCorrectionInterval(uint32_t _interval);
			bool		SetCorrectionInterval(std::string const& _scale, bool _check = false);

	virtual	bool		IsValid(std::string const& _value);

	virtual	std::string	GetValue();
	virtual	bool		SetValue(std::string const& _value, bool _check = false) = 0;

	virtual	std::string	GetValueMin() = 0;
	virtual	bool		SetValueMin(std::string const& _min, bool _check = false) = 0;

	virtual	std::string	GetValueMax() = 0;
	virtual	bool		SetValueMax(std::string const& _max, bool _check = false) = 0;


	virtual	bool		GetProperties(JSONNode& _properties, Fields const& _fields = PROPERTY_ALL);

	virtual	bool		SetProperty(JSONNode const& _property, bool _check = false);

	virtual				operator JSONNode();

			uint32_t	GetDataCount();
			Date		GetDateOfFirstData();
			Date		GetDateOfLastData();
			
			bool		GetUnreportedValueMap(ValueMap& value_list);
			Date		GetLastReportTime()	{	return	last_report_date_;	};
			bool		SetLastReportTime(Date const& _time);
			Date		GetLastConfirmTime(){	return	last_confirm_date_;	};
			bool		SetLastConfirmTime(Date const& _time);

			bool		GetDataForPeriod(Date const& _begin, Date const& _end, ValueMap& _value_map);
			uint32_t	GetData(uint32_t _count, ValueMap& _value_map);
			bool		DelDataForPeriod(Date const& _begin, Date const& _end);

			bool		Attach(std::string const& _parent_id);
			bool		Detach(std::string const& _parent_id);
			bool		Detach();

	virtual	bool		Start(uint32_t _wait_for_init_time = 0);
	virtual	bool		Stop(bool _wait = false);

	virtual	bool		IsRunning();

	static	const char*	Type();
	static	bool		IsIncludeIn(Object *_object);

	static	Endpoint*	Create(ObjectManager& _manager, JSONNode const& _properties);
	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);

protected:

			void		Preprocess();
			void		Process();
			void		Postprocess();
	virtual	void		CorrectionProcess();

	virtual	bool		Add(time_t time, std::string const& _value);
	virtual	bool		Add(time_t time, bool _value);

			bool		active_;
			uint32_t	sensor_id_;
			std::string	unit_;
			float		scale_;
			time_t		time_;
			std::string	value_;

			uint32_t	correction_interval_;
			Timer		correction_timer_;

			Date		last_report_date_;
			Date		last_confirm_date_;
			ValueMap	value_map_;
};

extern	const	char*	ENDPOINT_TYPE_NAME_TEMPERATURE;
extern	const	char*	ENDPOINT_TYPE_NAME_HUMIDITY;
extern	const	char*	ENDPOINT_TYPE_NAME_VOLTAGE;
extern	const	char*	ENDPOINT_TYPE_NAME_CURRENT;
extern	const	char*	ENDPOINT_TYPE_NAME_DI;
extern	const	char*	ENDPOINT_TYPE_NAME_PRESSURE;
extern	const	char*	ENDPOINT_TYPE_NAME_DO;

struct	MessageEndpointUpdated : Message
{
	MessageEndpointUpdated(std::string const& _sender, std::string const& _endpoint_id, time_t _time, std::string const& _value) 
	: Message(MSG_TYPE_ENDPOINT_UPDATED, _sender), endpoint_id(_endpoint_id), time(_time), value(_value)
	{
	};
	~MessageEndpointUpdated()
	{
	};

	std::string	endpoint_id;
	time_t		time;
	std::string	value;
};

struct	MessageEndpointReport : Message
{
	MessageEndpointReport(std::string const& _sender, std::string const& _endpoint_id) 
	: Message(MSG_TYPE_ENDPOINT_REPORT, _sender), endpoint_id(_endpoint_id)
	{
	};
	~MessageEndpointReport()
	{
	};

	std::string	endpoint_id;
};

#endif
