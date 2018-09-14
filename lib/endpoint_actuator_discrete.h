#ifndef	ENDPOINT_ACTUATOR_DISCRETE_H_
#define	ENDPOINT_ACTUATOR_DISCRETE_H_

#include "endpoint_actuator.h"

class	EndpointActuatorDiscrete : public EndpointActuator
{
public:
	EndpointActuatorDiscrete(ObjectManager& _manager, std::string const& _type);
	EndpointActuatorDiscrete(ObjectManager& _manager, std::string const& _type, JSONNode const& _properties);

			const char*	GetClassName();

	virtual	bool		IsValid(std::string const& _value);

			std::string	GetValue();
			bool		SetValue(std::string const& _value, bool _check = false);

			std::string	GetValueMin();
			bool		SetValueMin(std::string const& _min, bool _check = false);

			std::string	GetValueMax();
			bool		SetValueMax(std::string const& _max, bool _check = false);

	static const char*	Type();
protected:
	bool	Add(time_t	time, std::string const& _value);
	
	bool	value_;
};

#endif
