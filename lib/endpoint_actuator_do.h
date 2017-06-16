#ifndef	ENDPOINT_ACTUATOR_DO_H_
#define	ENDPOINT_ACTUATOR_DO_H_

#include "endpoint_actuator.h"

class	EndpointActuatorDO : public EndpointActuator
{
public:
	EndpointActuatorDO(ObjectManager& _manager);
	EndpointActuatorDO(ObjectManager& _manager, Properties const& _properties);

			std::string	GetClassName();
			bool		SetProperty(Property const& _property);

	const 	ValueType 	Type();

	virtual	bool		IsValid(std::string const& _value);

			std::string	GetValue();
			bool		SetValue(std::string const& _value);

			std::string	GetValueMin();
			bool		SetValueMin(std::string const& _min);

			std::string	GetValueMax();
			bool		SetValueMax(std::string const& _max);

protected:
	std::string	value_;
	std::string	value_min_;
	std::string	value_max_;
};

#endif
