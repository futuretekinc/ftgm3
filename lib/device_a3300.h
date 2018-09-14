#ifndef	DEVICE_A3300_H_
#define	DEVICE_A3300_H_

#include "device_modbus.h"


#define A3300_SENSOR_COUNT 11

class	DeviceA3300 : public DeviceModbus
{
public:
	DeviceA3300(ObjectManager& _manager);
	DeviceA3300(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

	bool	ReadValue(std::string const& _id, time_t& _time, std::string& _value);
	double  Binary32ToDouble(int value);
	double  convertFloat(int16_t* reg_temp);
protected:

	void	Preprocess();
	void	Process();

	Timer		correction_timer_;
	uint32_t	correction_interval_;

	time_t		time_;
	double	data_[A3300_SENSOR_COUNT];
	
// Static members
public:
	static	const char*	Type();
};
#endif
