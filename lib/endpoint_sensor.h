#ifndef	ENDPOINT_SENSOR_H_
#define	ENDPOINT_SENSOR_H_

#include "endpoint.h"

class	EndpointSensor : public Endpoint
{
public:
	EndpointSensor(ObjectManager& _manager);
	EndpointSensor(ObjectManager& _manager, std::string const& _unit, const ValueFloat& _value_min, const ValueFloat& _vlaue_max);
	EndpointSensor(ObjectManager& _manager, std::string const& _uint, float _value_min, float _vlaue_max);

			bool	IsValid(const ValueFloat& _value);


	virtual	bool	SetValueMin(ValueFloat const& _value_min, bool _store = true);
	virtual	bool	SetValueMax(ValueFloat const& _value_max, bool _store = true);
protected:

			bool	SetPropertyInternal(Property const& _property, bool create);

	ValueFloat	value_max_;
	ValueFloat	value_min_;
};

#endif
