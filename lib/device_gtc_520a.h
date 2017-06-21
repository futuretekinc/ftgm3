#ifndef	DEVICE_GTC_520A_H_
#define	DEVICE_GTC_520A_H_

#include "device_modbus.h"


class	DeviceGTC520A : public DeviceModbus
{
public:
	DeviceGTC520A(ObjectManager& _manager);

	virtual	bool	ReadValue(std::string const& _epid, time_t& _time, std::string& _value);
protected:

	void	Process();

};
#endif
