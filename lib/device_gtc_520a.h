#ifndef	DEVICE_GTC_520A_H_
#define	DEVICE_GTC_520A_H_

#include "device_modbus.h"


class	DeviceGTC520A : public DeviceModbus
{
public:
	DeviceGTC520A(ObjectManager& _manager);
	DeviceGTC520A(ObjectManager& _manager, JSONNode const& _properties);

	bool	ReadValue(std::string const& _id, time_t& _time, std::string& _value);

	static	const	std::string&	Type();
protected:

	void	Preprocess();
	void	Process();

	Timer		correction_timer_;
	uint32_t	correction_interval_;

	time_t		time_;
	int16_t		registers_[2];
};
#endif
