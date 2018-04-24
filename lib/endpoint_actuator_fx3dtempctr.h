#ifndef	ENDPOINT_ACTUATOR_FX3DTEMPCTR_H_
#define	ENDPOINT_ACTUATOR_FX3DTEMPCTR_H_

#include "endpoint_actuator_tempcontrol.h"

class	EndpointActuatorFX3DTempCTR : public EndpointActuatorTempControl
{
public:
	EndpointActuatorFX3DTempCTR(ObjectManager& _manager);
	EndpointActuatorFX3DTempCTR(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

protected:

//	Static members
public:
	static	const char*	Type();
};

#endif
