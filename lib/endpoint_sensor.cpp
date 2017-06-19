#include "device.h"
#include "defined.h"
#include "object_manager.h"
#include "endpoint_sensor.h"

EndpointSensor::EndpointSensor(ObjectManager& _manager, std::string const& _type)
:	Endpoint(_manager, _type)
{
}

EndpointSensor::EndpointSensor(ObjectManager& _manager, std::string const& _type, std::string const& _unit)
: Endpoint(_manager, _type, _unit)
{
}

void	EndpointSensor::Preprocess()
{
	Date	date;

	date = Date::GetCurrent() + Time(correction_interval_ * TIME_SECOND);

	correction_timer_.Set(date);

	Endpoint::Preprocess();
}

void	EndpointSensor::Process()
{
	if (correction_timer_.RemainTime() == 0)
	{
		CorrectionProcess();
		correction_timer_ += Time(correction_interval_ * TIME_SECOND);	
	}

	Endpoint::Process();
}

void	EndpointSensor::Postprocess()
{
	Endpoint::Postprocess();
}

bool	EndpointSensor::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<EndpointSensor *>(_object) != NULL;
}
