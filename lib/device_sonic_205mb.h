#ifndef	DEVICE_SONIC205MB_H_
#define	DEVICE_SONIC205MB_H_

#include "device_modbus.h"


class	DeviceSONIC205MB : public DeviceModbus
{
public:
	DeviceSONIC205MB(ObjectManager& _manager);
	DeviceSONIC205MB(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();
	double Binary32ToDouble(int value);
	bool	ReadValue(std::string const& _id, time_t& _time, std::string& _value);
	double cal_data(uint32_t mantissa,  int16_t exponent);

protected:

	void	Preprocess();
	void	Process();

	Timer		correction_timer_;
	uint32_t	correction_interval_;

	time_t		time_;
	int16_t	registers_[17];
	double d_data_[3];
	
// Static members
public:
	static	const char*	Type();
};
#endif
