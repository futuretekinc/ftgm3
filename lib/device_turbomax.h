#ifndef	DEVICE_TURBOMAX_H_
#define	DEVICE_TURBOMAX_H_

#include "device_modbus_tcp.h"


class	DeviceTurboMAX : public DeviceModbusTCP
{
public:
	DeviceTurboMAX(ObjectManager& _manager);
	DeviceTurboMAX(ObjectManager& _manager, JSONNode const& _properties);

	bool	ReadValue(uint32_t	_index, time_t& _time, uint32_t& _value);

	bool	ReadValue(std::string const& _id, time_t& _time, std::string& _value);
	bool	ReadValue(std::string const& _id, time_t& _time, uint32_t& _value);

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
