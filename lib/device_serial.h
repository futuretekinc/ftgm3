#ifndef	DEVICE_SERIAL_H_
#define	DEVICE_SERIAL_H_

#include "device.h"
#include <termios.h>
#include <sys/signal.h>
#include "buffer.h"

class	DeviceSerial : public Device
{
public:
	DeviceSerial(ObjectManager& _manager, ValueType const& _type);
	DeviceSerial(ObjectManager& _manager, ValueType const& _type, std::string const& _port);

	virtual	bool	GetProperties(Properties& _properties) const;

	const 	std::string&	GetPort();
			bool		SetPort(const std::string& _port, bool _store = true);

			bool		Write(uint8_t *buffer, uint32_t buffer_len);
			bool		Read(uint8_t *buffer, uint32_t buffer_len, uint32_t& read_len);
protected:
	virtual	bool		SetPropertyInternal(Property const& _property, bool create = false);
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
