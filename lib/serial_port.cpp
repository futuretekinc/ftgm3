#include <strings.h>
#include <iomanip>
#include <fcntl.h>
#include <csignal>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <sys/signal.h>
#include "defined.h"
#include "json.h"
#include "serial_port.h"

SerialPort::SerialPort()
: port_(""), ctrl_port_(""), baudrate_(115200), parity_bit_("none"), data_bit_(CS8), fd_(0)
{
}

SerialPort::SerialPort(std::string const& _port, uint32_t _baudrate, uint32_t _data_bit, std::string const& _parity_bit)
: port_(_port), ctrl_port_(""), fd_(0), mode_(false)
{
	SetBaudrate(_baudrate);
	SetDataBit(_data_bit);
	SetParityBit(_parity_bit);
}

SerialPort::SerialPort(std::string const& _port, std::string const& _ctrl_port, uint32_t _baudrate, uint32_t _data_bit, std::string const& _parity_bit)
: port_(_port), ctrl_port_(_ctrl_port), fd_(0), mode_(true)
{
	SetBaudrate(_baudrate);
	SetDataBit(_data_bit);
	SetParityBit(_parity_bit);
}

bool	SerialPort::GetOptions(JSONNode& _options)
{
	JSONNode	options;

	options.push_back(JSONNode(TITLE_NAME_NAME, port_));
	options.push_back(JSONNode(TITLE_NAME_CONTROL, ctrl_port_));
	options.push_back(JSONNode(TITLE_NAME_MODE, GetMode()));
	options.push_back(JSONNode(TITLE_NAME_BAUDRATE, GetBaudrate()));
	options.push_back(JSONNode(TITLE_NAME_PARITY_BIT, GetParityBit()));
	options.push_back(JSONNode(TITLE_NAME_DATA_BIT, GetDataBit()));
	
	_options = options;

	return	true;
}

bool	SerialPort::SetOptions(JSONNode const& _options, bool _check)
{
	bool ret_value = true;
	
	for(JSONNode::const_iterator it = _options.begin() ; it != _options.end() ; it++)
	{
		ret_value = SetOption(*it, _check);
		if (ret_value != true)
		{
			break;
		}
	}

	return	ret_value;
}

bool	SerialPort::SetOption(JSONNode const& _option, bool _check)
{
	bool ret_value = true;
	
	if (_option.name() == TITLE_NAME_NAME)
	{
		ret_value = SetPort(_option.as_string(), _check);	
	}
	else if (_option.name() == TITLE_NAME_CONTROL)
	{
		ret_value = SetControlPort(_option.as_string(), _check);	
	}
	else if (_option.name() == TITLE_NAME_MODE)
	{
		ret_value = SetMode(_option.as_int(), _check);	
	}
	else if (_option.name() == TITLE_NAME_BAUDRATE)
	{
		ret_value = SetBaudrate(_option.as_int(), _check);	
	}
	else if (_option.name() == TITLE_NAME_PARITY_BIT)
	{
		ret_value = SetParityBit(_option.as_string(), _check);	
	}
	else if (_option.name() == TITLE_NAME_DATA_BIT)
	{
		ret_value = SetDataBit(_option.as_int(), _check);	
	}
	else if(_option.name() == TITLE_NAME_IP)
	{
		TRACE_INFO("IP SAVE : " << _option.as_string());
		ret_value = SetDevIP(_option.as_string(), _check);
	}
	else if(_option.name() == TITLE_NAME_PORT)
	{
		TRACE_INFO("PORT SAVE : " << _option.as_string());	
		ret_value = SetDevPort(_option.as_string(), _check);
	}
	return	ret_value;
}

bool	SerialPort::SetDevIP(std::string const& _dev_ip,  bool _check)
{
	if(!_check)
	{
		dev_ip_ = _dev_ip;
	}
	return true;
}

const std::string&	SerialPort::GetDevIP()
{
	return dev_ip_;
}

bool	SerialPort::SetDevPort(std::string const& _dev_port, bool _check)
{
	if(!_check)
	{
		dev_port_ = _dev_port;
	}
	return true;
}
const std::string&	SerialPort::GetDevPort()
{
	return dev_port_;
}

const std::string&	SerialPort::GetPort()
{
	return	port_;
}

bool	SerialPort::SetPort(std::string const& _port, bool _check)
{
	if (!_check)
	{
		port_ = _port;
	}

	return	true;
}

const std::string&	SerialPort::GetControlPort()
{
	return	ctrl_port_;
}

bool	SerialPort::SetControlPort(std::string const& _ctrl_port, bool _check)
{
	if (!_check)
	{
		ctrl_port_ = _ctrl_port;
	}

	return	true;
}

uint32_t	SerialPort::GetBaudrate()
{
	return	baudrate_;
}


bool		SerialPort::SetBaudrate(uint32_t _baudrate, bool _check)
{
	if (!_check)
	{
		baudrate_ = _baudrate;
	}

	return	true;
}

std::string	SerialPort::GetParityBit()
{
	
	return	parity_bit_;
}

bool	SerialPort::SetParityBit(std::string const& _parity_bit, bool _check)
{
	uint32_t	parity_bit = 0;

	if ((_parity_bit != "even") && (_parity_bit != "odd") && (_parity_bit != "none"))
	{
		return	false;
	}

	if (!_check)
	{
		parity_bit_ = _parity_bit;	
	}

	return	true;
}

uint32_t	SerialPort::GetDataBit()
{
	return	data_bit_;
}

bool	SerialPort::SetDataBit(uint32_t _data_bit, bool _check)
{
	if (_data_bit < 5 || 8 < _data_bit)
	{
		return	false;
	}

	if (!_check)
	{
		data_bit_ = _data_bit;	
	}

	return	true;
}

bool	SerialPort::GetMode()
{
	return	mode_;
}

bool	SerialPort::SetMode(bool _mode, bool _check)
{
	mode_ = _mode;

	return	true;
}

bool	SerialPort::Open()
{
	uint32_t	baudrate_tables[][2] = {	{0, B0}, { 50, B50},{ 75,B75},{110,B110},{134,B134},{150,B150},{200,B200},{300,B300},{600,B600},{1200,B1200},
										{1800,	B1800},{2400,	B2400},{4800,	B4800},{9600,	B9600},{19200,	B19200},{38400,	B38400},{57600,	B57600},{115200,B115200},{230400,B230400}, {460800, B38400}};

	if (fd_ > 0)
	{
		TRACE_ERROR("Failed to open dev[" << port_ << "]");
		return	false;
	}
	TRACE_INFO("BEFORE SERIAL OPEN");

	fd_ = open(port_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd_ <= 0)
	{
		fd_ = 0;
		TRACE_ERROR("Failed to open dev[" << port_ << "]");
		return	false;
	}

	int	baudrate = B38400;

	for(int i = 1 ; i < sizeof(baudrate_tables) / (sizeof(uint32_t) * 2)  ; i++)
	{
		if (baudrate_ < baudrate_tables[i][0])
		{
			baudrate = baudrate_tables[i-1][1];		
			break;
		}
	}

	int	parity_bit = 0;
	if (parity_bit_ == "even")
	{
		parity_bit = PARENB;
	}
	else if (parity_bit_ == "odd")
	{
		parity_bit = PARODD;
	}

	int	data_bit = CS8;

	switch(data_bit_)
	{
	case	5:	data_bit = CS5;	
	case	6:	data_bit = CS6;	
	case	7:	data_bit = CS7;	
	case	8:	data_bit = CS8;	
	}

   	struct sigaction saio;           /* definition of signal action */
	sigemptyset(&saio.sa_mask);
	saio.sa_flags = saio.sa_flags|SA_SIGINFO;
	saio.sa_restorer = NULL;
	saio.sa_sigaction = SignalHandler;

	sigaction(SIGIO, &saio, NULL);

	/* allow the process to receive SIGIO */
	fcntl(fd_, F_SETOWN, getpid());
	/* Make the file descriptor asynchronous (the manual page says only O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	fcntl(fd_, F_SETFL, FASYNC);

	tcgetattr(fd_, &old_term_ios_); /* save current port settings */
	
	/* set new port settings for canonical input processing */
	struct termios	new_term_ios;
	new_term_ios.c_cflag = data_bit | parity_bit | CLOCAL | CREAD;
	new_term_ios.c_iflag = 0;
	new_term_ios.c_oflag = OFILL;
	new_term_ios.c_lflag = 0;
	new_term_ios.c_cc[VMIN]=0;
	new_term_ios.c_cc[VTIME]=0;

	cfsetispeed(&new_term_ios, baudrate);
	cfsetospeed(&new_term_ios, baudrate);

	tcflush(fd_, TCIFLUSH);
	tcsetattr(fd_, TCSANOW, &new_term_ios);

	if (mode_)
	{
		ctrl_ = open(ctrl_port_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
		if (ctrl_ <= 0)
		{
			ctrl_ = 0;
			TRACE_ERROR("Failed to open control [ " << ctrl_port_ << "]");
			return	false;
		}
		else
		{
			const char*	read_enable = "0\n";
			::write(ctrl_, read_enable, strlen(read_enable));
		}
	}

	TRACE_INFO("Serial Port [" << port_ << "] is opened.");
	return	true;
}

bool	SerialPort::Close()
{
	if (mode_)
	{
		if (ctrl_ != 0)
		{
			close(ctrl_);	
			ctrl_ = 0;
		}	
	}

	if (fd_ != 0)
	{
		tcsetattr(fd_, TCSANOW, &old_term_ios_);

		close(fd_);	
		fd_ = 0;
	}	

	return	true;
}

void	SerialPort::SetDirectionOut(bool _out)
{
	if (mode_)
	{
		if (ctrl_ != 0)
		{
			if (_out)
			{
				::write(ctrl_, "1", 2);
			}
			else
			{
				::write(ctrl_, "0", 2);
			}
		}
		else
		{
		//	TRACE_ERROR("The dev is not open!");
		}
	}
}

bool	SerialPort::Write(uint8_t *buffer, uint32_t buffer_len)
{
	if (fd_ == 0)
	{
		TRACE_ERROR("The dev is not open!");
		return	false;
	}

	uint32_t 	output_time = 1000000 / baudrate_ * (buffer_len) * 2 / 3;


	TRACE_INFO_DUMP((char *)buffer, buffer_len);

	uint64_t	start_time = Date::GetCurrent().GetMicroSecond();
	SetDirectionOut(true);

	tcflush(fd_, TCIFLUSH);
	::write(fd_, buffer, buffer_len);
	uint64_t	end_time1 = Date::GetCurrent().GetMicroSecond();
	usleep(output_time);
	if (end_time1 < 32000)
	{
		usleep(output_time);
	}
	uint64_t	end_time2 = Date::GetCurrent().GetMicroSecond();

	SetDirectionOut(false);
	uint64_t	end_time3 = Date::GetCurrent().GetMicroSecond();

	TRACE_INFO("Write Interval : " << end_time1 - start_time << " - " << end_time2 - start_time << " - " << end_time3 - start_time << " - " << output_time);

	return	true;
}

bool	SerialPort::Read(uint8_t *buffer, uint32_t buffer_len, uint32_t& read_len)
{
	if (fd_ == 0)
	{
		TRACE_ERROR("The dev is not open!");
		return	false;
	}


	read_len = read(fd_, (char *)buffer, buffer_len);

	return	true;
}

bool	SerialPort::Read(uint8_t *buffer, uint32_t buffer_len, uint32_t _timeout, uint32_t& read_len)
{
	fd_set set;
	struct timeval timeout;
	int rv;

	FD_ZERO(&set); /* clear the set */
	FD_SET(fd_, &set); /* add our file descriptor to the set */

	timeout.tv_sec 	= 0;
	timeout.tv_usec = _timeout * 1000;

	rv = select(fd_+ 1, &set, NULL, NULL, &timeout);
	if(rv == -1)
	{
		read_len = 0;
		return	false;
		/* an error accured */
	}
	else if(rv == 0)
	{
		read_len = 0;
		/* a timeout occured */
	}
	else
	{
		read_len = read( fd_, (char *)buffer, buffer_len); /* there was data to read */
	}

	return	true;
}

void 	SerialPort::SignalHandler(int status, siginfo_t *ioinfo, void *context )
{
}

