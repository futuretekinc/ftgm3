#ifndef	SERIAL_PORT_H_
#define	SERIAL_PORT_H_

#include <termios.h>
#include <sys/signal.h>
#include "active_object.h"

class	SerialPort
{
public:
	SerialPort();
	SerialPort(std::string const& _port, uint32_t _baudrate, uint32_t _data_bit, std::string const& _parity_bit);

	virtual	bool		GetOptions(JSONNode& _options);
	virtual	bool		SetOptions(JSONNode& _options, bool _check = false);
	virtual	bool		SetOption(JSONNode& _option, bool _check);

	const std::string&	GetPort();
			bool		SetPort(const std::string& _port, bool _check = false);

			uint32_t	GetBaudrate();
			bool		SetBaudrate(uint32_t _baudrate, bool _check = false);

		 std::string	GetParityBit();
			bool		SetParityBit(std::string const& _parity_bit, bool _check = false);

			uint32_t	GetDataBit();
			bool		SetDataBit(uint32_t _data_bit, bool _check = false);

	virtual	bool		Open();
	virtual	bool		Close();

	virtual	bool		Read(uint8_t *buffer, uint32_t buffer_len, uint32_t& read_len);
	virtual	bool		Read(uint8_t *buffer, uint32_t buffer_len, uint32_t _timeout, uint32_t& read_len);
	virtual	bool		Write(uint8_t *buffer, uint32_t buffer_len);
protected:
 	static	void 		SignalHandler(int status, siginfo_t *ioinfo, void *context );

		
	std::string		port_;
	int				fd_;
	int				ioctl_;
	uint32_t		baudrate_;
	std::string		parity_bit_;
	uint32_t		data_bit_;
	struct termios 	old_term_ios_;
};

#endif
