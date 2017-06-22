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

	virtual	bool		GetProperties(JSONNode& _properties, Fields const& _fields = PROPERTY_ALL);

	virtual	bool		SetProperty(JSONNode const& _property, bool _check = false);

protected:
			void		Preprocess();
			void		Process();
			void		Postprocess();

	SerialPort*			serial_;	
};

#endif
