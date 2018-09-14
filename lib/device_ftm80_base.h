#ifndef	DEVICE_FTM80_BASE_H_
#define	DEVICE_FTM80_BASE_H_

#include "device_u2u.h"

#define	BASE_SENSOR_MAX	10

class	Device_ftm80_base : public DeviceU2U
{
public:
	struct	FTM80_OBJECT
	{
		uint32_t        oid_;
   		int             data_;
	}FTM80_OID_DATA[BASE_SENSOR_MAX];
	Device_ftm80_base(ObjectManager& _manager);
	Device_ftm80_base(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

	bool	ReadValue(std::string const& _id, time_t& _time, std::string& _value);
	bool    WriteValue(std::string const& _epid, bool _value);
protected:

	void	Preprocess();
	void	Process();

	Timer		correction_timer_;
	uint32_t	correction_interval_;

	time_t		time_;

// Static members
public:
	static	const char*	Type();
};
#endif
