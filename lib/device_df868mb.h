#ifndef	DEVICE_DF868MB_H_
#define	DEVICE_DF868MB_H_

#include "device_modbus.h"


class	DeviceDF868MB : public DeviceModbus
{
public:
	DeviceDF868MB(ObjectManager& _manager);
	DeviceDF868MB(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();
	double Binary32ToDouble(int value);
	bool	ReadValue(std::string const& _id, time_t& _time, std::string& _value);

protected:

	void	Preprocess();
	void	Process();

	Timer		correction_timer_;
	uint32_t	correction_interval_;

	time_t		time_;
	int16_t	registers_[10];
	double d_data_[3];

// Static members
public:
	static	const char*	Type();
};
#endif
