#ifndef	DEVICE_TEST_H_
#define	DEVICE_TEST_H_

#include "device_modbus.h"


#define TEST_SENSOR_COUNT 11

class	DeviceTEST : public DeviceModbus
{
public:
	DeviceTEST(ObjectManager& _manager);
	DeviceTEST(ObjectManager& _manager, JSONNode const& _properties);

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
	double	data_[TEST_SENSOR_COUNT];
	
// Static members
public:
	static	const char*	Type();
};
#endif
