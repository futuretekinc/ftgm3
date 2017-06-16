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

			std::string	GetValue();
			bool		SetValue(std::string const& _value);

			std::string	GetValueMin();
			bool		SetValueMin(std::string const& _min);

			std::string	GetValueMax();
			bool		SetValueMax(std::string const& _max);

			bool		SetProperty(Property const& _property, Properties::Fields const& _fields = PROPERTY_ALL);

			bool		GetProperties(Properties& _properties, Properties::Fields const& _fields = PROPERTY_ALL);

	static	bool		IsIncludeIn(Object *_object);
protected:


			void		Preprocess();
			void		Process();
			void		Postprocess();

			float		value_;
			float		value_min_;
			float		value_max_;
};

#endif
