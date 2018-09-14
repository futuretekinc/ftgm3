#ifndef	DEVICE_SONIC_205_H_
#define	DEVICE_SONIC_205_H_

#include "device_modbus.h"


#define SONIC_205_SENSOR_COUNT 3

class	DeviceSONIC205 : public DeviceModbus
{
public:
	DeviceSONIC205(ObjectManager& _manager);
	DeviceSONIC205(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

	bool	ReadValue(std::string const& _id, time_t& _time, std::string& _value);
	std::string* strSplit(std::string strOrigin, std::string strTok, int size);
	double cal_data(double mantissa, double exponent);
protected:

	void	Preprocess();
	void	Process();

	Timer		correction_timer_;
	uint32_t	correction_interval_;

	time_t		time_;
	double		data_[SONIC_205_SENSOR_COUNT];

// Static members
public:
	static	const char*	Type();
};
#endif
