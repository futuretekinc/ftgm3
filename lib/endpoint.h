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

	class	ValueMap : public std::multimap<Date, std::string>
	{
	public:
		ValueMap(uint32_t _limit = 100);
		~ValueMap();

		bool		Add(Date const& _date, std::string const& _value);
		uint32_t	GetMaxCount()	{	return	limit_;	};
	protected:
		uint32_t	limit_;
	};

	class	ValueList : public std::list<std::string>
	{
	public:
		ValueList(uint32_t _limit = 100);
		~ValueList();

		bool	Add(std::string const& _value);

	protected:
		uint32_t	limit_;
	};

						Endpoint(ObjectManager& _manager, ValueType const& _type);
						Endpoint(ObjectManager& _manager, ValueType const& _type, std::string const& _unit);
	virtual				~Endpoint();	

			bool		SetUnit(std::string const& _unit);
	const	ValueUnit&	GetUnit() const;

			float		GetScale() const;
			bool		SetScale(float _scale);
			bool		SetScale(std::string const& _scale);

			std::string	GetSensorID() const;
			bool		SetSensorID(std::string const& _sensor_id);

			uint32_t	GetCorrectionInterval();
			bool		SetCorrectionInterval(Time const& _interval);
			bool		SetCorrectionInterval(uint32_t _interval);
			bool		SetCorrectionInterval(std::string const& _scale);

	virtual	std::string	GetValue()	=	0;
	virtual	bool		SetValue(std::string const& _value) = 0;

	virtual	std::string	GetValueMin() = 0;
	virtual	bool		SetValueMin(std::string const& _min) = 0;

	virtual	std::string	GetValueMax() = 0;
	virtual	bool		SetValueMax(std::string const& _max) = 0;

	virtual	bool		SetProperty(Property const& _property, Properties::Fields const& _fields = PROPERTY_ALL);

	virtual	bool		GetProperties(Properties& _properties, Properties::Fields const& _fields = PROPERTY_ALL);
	virtual	bool		GetProperties(JSONNode& _properties, Properties::Fields const& _fields = PROPERTY_ALL);

	virtual				operator JSONNode();

	virtual	bool		IsValid(std::string const& _value);

			uint32_t	GetDataCount();
			Date		GetDateOfFirstData();
			Date		GetDateOfLastData();
			
			bool		GetUnreportedValueMap(ValueMap& value_list);
			Date		GetLastReportTime()	{	return	last_report_date_;	};
			bool		SetLastReportTime(Date const& _time);
			Date		GetLastConfirmTime(){	return	last_confirm_date_;	};
			bool		SetLastConfirmTime(Date const& _time);

			bool		GetDataForPeriod(Date const& _begin, Date const& _end, ValueMap& _value_map);
			bool		GetDataForPeriod(Date const& _begin, Date const& _end, ValueList& _value_list);
			uint32_t	GetData(uint32_t _count, ValueMap& _value_map);
			bool		DelDataForPeriod(Date const& _begin, Date const& _end);

			bool		Attach(ValueID const& _parent_id);
			bool		Detach(ValueID const& _parent_id);
			bool		Detach();

	virtual	bool		Start();
	virtual	bool		Stop(bool _wait = false);

	virtual	bool		IsRunning();
	static	bool		IsIncludeIn(Object *_object);
	static	bool		IsValidType(std::string const& _type);

	static	Endpoint*	Create(ObjectManager& _manager, Properties const& _properties);
	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);

protected:

			void		Preprocess();
			void		Process();
			void		Postprocess();
	virtual	void		CorrectionProcess();

	virtual	bool		Add(std::string const& _value);

			bool		active_;
			std::string	sensor_id_;
			ValueUnit	unit_;
			ValueFloat	scale_;

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
	: Message(MSG_TYPE_ENDPOINT_UPDATED, _sender), endpoint_id(_endpoint_id), value(_value)
	{
	};
	~MessageEndpointUpdated()
	{
	};

	ValueID		endpoint_id;
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

	ValueID				endpoint_id;
};

#endif
