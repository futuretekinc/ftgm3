#ifndef	SERIAL_PORT_HALF_H_
#define	SERIAL_PORT_HALF_H_

#include <termios.h>
#include <sys/signal.h>
#include "serial_port.h"

class	SerialPortHalf : public SerialPort
{
public:
	SerialPortHalf();
	SerialPortHalf(std::string const& _comm_port, std::string const& _ctrl_port, uint32_t _baudrate, uint32_t _data_bit, std::string const& _parity_bit);

	virtual	bool		GetOptions(JSONNode& _options);
	virtual	bool		SetOptions(JSONNode& _options, bool _check = false);

	const std::string&	GetControlPort();
			bool		SetControlPort(const std::string& _port, bool _check = false);

			bool		Open();
			bool		Close();

			bool		Read(uint8_t *buffer, uint32_t buffer_len, uint32_t& read_len);
			bool		Read(uint8_t *buffer, uint32_t buffer_len, uint32_t _timeout, uint32_t& read_len);
			bool		Write(uint8_t *buffer, uint32_t buffer_len);
protected:

		virtual	bool		SetOption(JSONNode& _option, bool _check = false);

		std::string		ctrl_port_;
			int			ctrl_;
};

#endif
