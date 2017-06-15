#ifndef	ENDPOINT_SENSOR_H_
#define	ENDPOINT_SENSOR_H_

#include "endpoint.h"

class	EndpointSensor : public Endpoint
{
	friend 	class	Device;
	friend	class	ObjectManager;

public:
	EndpointSensor(ObjectManager& _manager, ValueType const& _type);
	EndpointSensor(ObjectManager& _manager, ValueType const& _type, std::string const& _unit, const ValueFloat& _value_min, const ValueFloat& _vlaue_max);
	EndpointSensor(ObjectManager& _manager, ValueType const& _type, std::string const& _uint, float _value_min, float _vlaue_max);

			bool		IsValid(const ValueFloat& _value);

			uint32_t	GetCorrectionInterval();
			bool		SetCorrectionInterval(Time const& _interval);
			bool		SetCorrectionInterval(uint32_t _interval);
			bool		SetCorrectionInterval(std::string const& _scale);

	virtual	bool		SetValueMin(ValueFloat const& _value_min);
	virtual	bool		SetValueMax(ValueFloat const& _value_max);

			bool		SetProperty(Property const& _property, Properties::Fields const& _fields = PROPERTY_ALL);

			bool		GetProperties(Properties& _properties, Properties::Fields const& _fields = PROPERTY_ALL);
protected:


			void		Preprocess();
			void		Process();
			void		Postprocess();

	virtual	void		CorrectionProcess();

			uint32_t	correction_interval_;
			Timer		correction_timer_;

			ValueFloat	value_max_;
			ValueFloat	value_min_;
};

#endif
