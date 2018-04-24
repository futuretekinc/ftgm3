#ifndef	DEVICE_MB7092_H_
#define	DEVICE_MB7092_H_

#include "device_modbus.h"


class	DeviceMB7092 : public DeviceModbus
{
public:
	DeviceMB7092(ObjectManager& _manager);
	DeviceMB7092(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

	bool	ReadValue(std::string const& _id, time_t& _time, std::string& _value);

protected:

	void	Preprocess();
	void	Process();

	Timer		correction_timer_;
	uint32_t	correction_interval_;

	time_t		time_;
	uint8_t		data_[6];

// Static members
public:
	static	const char*	Type();
};
#endif
