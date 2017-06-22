#ifndef	DEVICE_SERIAL_H_
#define	DEVICE_SERIAL_H_

#include "device.h"
#include <termios.h>
#include <sys/signal.h>
#include "buffer.h"
#include "serial_port.h"

class	DeviceSerial : public Device
{
public:
	DeviceSerial(ObjectManager& _manager, std::string const& _type, bool _half = false);
	~DeviceSerial();

protected:
	virtual	bool		GetOptions(JSONNode& _optoins);
	virtual	bool		SetOption(JSONNode const& _option, bool _check = false);

			void		Preprocess();
			void		Process();
			void		Postprocess();

	SerialPort			serial_;	
};

#endif
