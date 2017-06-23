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
	SerialPort(std::string const& _comm_port, std::string const& _ctrl_port, uint32_t _baudrate, uint32_t _data_bit, std::string const& _parity_bit);

	virtual	bool		GetOptions(JSONNode& _options);
	virtual	bool		SetOptions(JSONNode const& _options, bool _check = false);
	virtual	bool		SetOption(JSONNode const& _option, bool _check);

	const std::string&	GetPort();
			bool		SetPort(const std::string& _port, bool _check = false);

	const std::string&	GetControlPort();
			bool		SetControlPort(const std::string& _port, bool _check = false);

			uint32_t	GetBaudrate();
			bool		SetBaudrate(uint32_t _baudrate, bool _check = false);

		 std::string	GetParityBit();
			bool		SetParityBit(std::string const& _parity_bit, bool _check = false);

			uint32_t	GetDataBit();
			bool		SetDataBit(uint32_t _data_bit, bool _check = false);

			bool		GetMode();
			bool		SetMode(bool _mode, bool _check = false);

	virtual	bool		Open();
	virtual	bool		Close();

	virtual	bool		Read(uint8_t *buffer, uint32_t buffer_len, uint32_t& read_len);
	virtual	bool		Read(uint8_t *buffer, uint32_t buffer_len, uint32_t _timeout, uint32_t& read_len);
	virtual	bool		Write(uint8_t *buffer, uint32_t buffer_len);

protected:
	virtual	void		SetDirectionOut(bool _out);

 	static	void 		SignalHandler(int status, siginfo_t *ioinfo, void *context );

		
	std::string		port_;
	std::string		ctrl_port_;
	uint32_t		baudrate_;
	std::string		parity_bit_;
	uint32_t		data_bit_;
	bool			mode_;

	int				fd_;
	int				ctrl_;
	struct termios 	old_term_ios_;
};

#endif
