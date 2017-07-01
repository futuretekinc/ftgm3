#ifndef	ENDPOINT_SENSOR_H_
#define	ENDPOINT_SENSOR_H_

#include "endpoint.h"

class	EndpointSensor : public Endpoint
{
	friend 	class	Device;
	friend	class	ObjectManager;

public:
	EndpointSensor(ObjectManager& _manager, std::string const& _type);
	EndpointSensor(ObjectManager& _manager, std::string const& _type, std::string const& _unit);

	static	bool		IsIncludeIn(Object *_object);
	
protected:

			void		Preprocess();
			void		Process();
			void		Postprocess();

};

#endif
