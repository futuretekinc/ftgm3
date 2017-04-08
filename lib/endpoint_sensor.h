#ifndef	ENDPOINT_SENSOR_H_
#define	ENDPOINT_SENSOR_H_

#include "endpoint.h"

class	EndpointSensor : public Endpoint
{
public:
	EndpointSensor();
	EndpointSensor(std::string const& _unit, const ValueFloat& _value_min, const ValueFloat& _vlaue_max);
	EndpointSensor(std::string const& _uint, float _value_min, float _vlaue_max);

	bool	IsValid(const ValueFloat& _value);

protected:
	ValueFloat	value_max_;
	ValueFloat	value_min_;
};

#endif
