#ifndef	DEVICE_SERIAL_H_
#define	DEVICE_SERIAL_H_

#include "device.h"
#include <termios.h>
#include <sys/signal.h>
#include "buffer.h"

class	DeviceSerial : public Device
{
public:
	DeviceSerial(ObjectManager& _manager, std::string const& _type);
	DeviceSerial(ObjectManager& _manager, std::string const& _type, std::string const& _port);

	virtual	bool		GetProperties(JSONNode& _properties, Fields const& _fields) ;

	virtual	bool		SetProperty(JSONNode const& _property, bool _check = false);

	const std::string&	GetPort();
			bool		SetPort(const std::string& _port, bool _check = true);

			bool		Write(uint8_t *buffer, uint32_t buffer_len);
			bool		Read(uint8_t *buffer, uint32_t buffer_len, uint32_t& read_len);
protected:
 	static	void 		SignalHandler(int status, siginfo_t *ioinfo, void *context );

			bool		Open();
			bool		Close();
		
			void		Preprocess();
			void		Process();
			void		Postprocess();

	std::string	dev_name_;
	int			fd_;
	int			baudrate_;
	struct termios old_term_ios_;
	Buffer		buffer_;
};

#endif
