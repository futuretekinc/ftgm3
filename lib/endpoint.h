#ifndef	ENDPOINT_H_
#define	ENDPOINT_H_

#include <map>
#include "active_object.h"
#include "timer.h"
#include "time2.h"

#define	MSG_TYPE_ENDPOINT				0x00020000
#define	MSG_TYPE_ENDPOINT_UPDATED		(MSG_TYPE_ENDPOINT + 1)
#define	MSG_TYPE_ENDPOINT_REPORT		(MSG_TYPE_ENDPOINT + 2)

class	Endpoint : public ActiveObject
{
	friend 	class	Device;
	friend	class	ObjectManager;
public:

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

	class	ValueList : public std::list<Value *>
	{
	public:
		ValueList(uint32_t _limit = 100);
		~ValueList();

		bool	Add(Value const* _value);

	protected:
		uint32_t	limit_;
	};

						Endpoint(ObjectManager& _manager);
	virtual				~Endpoint();	

	virtual	Type		GetType() const = 0;

			bool		SetUnit(std::string const& _unit, bool _store = true);
	const	ValueUnit&	GetUnit() const;

			float		GetScale() const;
			bool		SetScale(float _scale, bool _store = true);
			bool		SetScale(std::string const& _scale, bool _store = true);

			std::string	GetSensorID() const;
			bool		SetSensorID(std::string const& _sensor_id, bool _store = true);


			uint64_t	GetUpdateInterval();
			bool		SetUpdateInterval(Time const& _update_interval, bool _store = true);
			bool		SetUpdateInterval(uint64_t _update_interval, bool _store = true);
			bool		SetUpdateInterval(std::string const& _scale, bool _store = true);

	virtual	bool		GetProperties(Properties& _properties) const;

			bool		ApplyChanges();

	virtual	bool		IsValid(const ValueFloat& _value);

			uint32_t	GetDataCount();
			Date		GetDateOfFirstData();
			Date		GetDateOfLastData();
			
			bool		GetUnreportedValueList(ValueList& value_list);

			bool		GetDataForPeriod(Date const& _begin, Date const& _end, ValueMap& _value_map);
			bool		GetDataForPeriod(Date const& _begin, Date const& _end, ValueList& _value_list);
			bool		DelDataForPeriod(Date const& _begin, Date const& _end);

	const	ValueID&	GetDeviceID() const;
			bool		SetDeviceID(ValueID const& _device_id, bool _store = true);

			bool		Attach(ValueID const& _device_id);
			bool		Detach(ValueID const& _device_id);
			bool		Detach();

			void		Start();
			void		Stop();

	virtual	bool		IsRunning();
	static	bool		IsValidType(std::string const& _type);

	static	Endpoint*	Create(ObjectManager& _manager, Properties const& _properties);
	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);
	static	std::string	ToString(Type _type);

protected:
	virtual	bool		SetPropertyInternal(Property const& _property, bool create = false);

			void		Preprocess();
			void		Process();
			void		Postprocess();

	virtual	void		UpdateProcess();
	virtual	bool		Add(Value const* _value);

			ObjectManager&	manager_;
			ValueID		device_id_;
			bool		active_;
			std::string	sensor_id_;
			ValueUnit	unit_;
			ValueFloat	scale_;
			ValueFloat	value_;
			Time		update_interval_;
			Timer		update_timer_;
			Date		last_report_date_;

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
	MessageEndpointUpdated(ValueID const& _id, Value const *_value) 
	: Message(MSG_TYPE_ENDPOINT_UPDATED), id(_id)
	{
		value = _value->Duplicate();
	};
	~MessageEndpointUpdated()
	{
		delete value;
	};

	ValueID		id;
	Value*		value;
};

struct	MessageEndpointReport : Message
{
	MessageEndpointReport(ValueID const& _id, Endpoint::ValueList const& _value_list) 
	: Message(MSG_TYPE_ENDPOINT_REPORT), id(_id)
	{
		for(auto it = _value_list.begin() ; it != _value_list.end() ; it++)
		{
			value_list.push_back((*it)->Duplicate());	
		}
	};
	~MessageEndpointReport()
	{
		for(auto it = value_list.begin() ; it != value_list.end() ; it++)
		{
			delete (*it);
		}
	};

	ValueID		id;
	std::list<Value*>	value_list;
};

#endif
