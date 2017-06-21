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

	virtual	bool		GetOptions(JSONNode& _options);
	virtual	bool		SetOptions(JSONNode& _options, bool _check = false);

	const std::string&	GetPort();
			bool		SetPort(const std::string& _port, bool _check = false);

			uint32_t	GetBaudrate();
			bool		SetBaudrate(uint32_t _baudrate, bool _check = false);

		 std::string	GetParityBit();
			bool		SetParityBit(std::string const& _parity_bit, bool _check = false);

			uint32_t	GetDataBit();
			bool		SetDataBit(uint32_t _data_bit, bool _check = false);

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
	uint32_t	baudrate_;
	uint32_t	parity_bit_;
	uint32_t	data_bit_;
	struct termios old_term_ios_;
	Buffer		buffer_;
};

#endif
