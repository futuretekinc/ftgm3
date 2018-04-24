#ifndef	ENDPOINT_ACTUATOR_LINEAR_H_
#define	ENDPOINT_ACTUATOR_LINEAR_H_

#include "endpoint_actuator.h"

class	EndpointActuatorTempControl : public EndpointActuator
{
public:
	EndpointActuatorTempControl(ObjectManager& _manager, std::string const& _type, std::string const& _unit, double     _min, double _max);
	EndpointActuatorTempControl(ObjectManager& _manager, std::string const& _type, JSONNode const& _properties, std::string const& _unit, double     _min, double _max );

			const char*	GetClassName();

	virtual	bool		IsValid(std::string const& _value);

			std::string	GetValue();
			bool		SetValue(std::string const& _value, bool _check = false);

			std::string	GetValueMin();
			bool		SetValueMin(std::string const& _min, bool _check = false);

			std::string	GetValueMax();
			bool		SetValueMax(std::string const& _max, bool _check = false);
			std::string*    strSplit(std::string strOrigin, std::string strTok, int size);
	static const char*	Type();
protected:
	double value_min_;
	double value_max_;


	double target_value;
	double heating_value;
	double cooling_value;

	std::string s_value_tmp;
	std::string total_value;
	std::string save_value;
};

#endif
