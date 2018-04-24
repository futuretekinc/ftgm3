#ifndef	DEVICE_HS_1000M_H_
#define	DEVICE_HS_1000M_H_

#include "device_modbus.h"


class	DeviceHS1000M : public DeviceModbus
{
public:
	DeviceHS1000M(ObjectManager& _manager);
	DeviceHS1000M(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

	bool	ReadValue(std::string const& _id, time_t& _time, std::string& _value);
	uint8_t EOR_checksum(uint8_t* buf, uint32_t buf_len);

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
