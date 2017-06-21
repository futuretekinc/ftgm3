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
#include "device_serial.h"

DeviceSerial::DeviceSerial(ObjectManager& _manager, std::string const& _type)
: Device(_manager, _type), dev_name_(""), baudrate_(B115200), parity_bit_(0), data_bit_(CS8), buffer_(1024)
{
}

DeviceSerial::DeviceSerial(ObjectManager& _manager, std::string const& _type, std::string const& _port)
: Device(_manager, _type), dev_name_(_port), buffer_(1024)
{
}

bool	DeviceSerial::GetOptions(JSONNode& _options)
{
	JSONNode	options;

	options.push_back(JSONNode(TITLE_NAME_NAME, dev_name_));
	options.push_back(JSONNode(TITLE_NAME_BAUDRATE, GetBaudrate()));
	options.push_back(JSONNode(TITLE_NAME_PARITY_BIT, GetParityBit()));
	options.push_back(JSONNode(TITLE_NAME_DATA_BIT, GetDataBit()));
	
	_options = options;

	return	true;
}

bool	DeviceSerial::SetOptions(JSONNode& _options, bool _check)
{
	bool ret_value = true;

	for(JSONNode::iterator it = _options.begin() ; it != _options.end() ; it++)
	{
		if (it->name() == TITLE_NAME_NAME)
		{
			ret_value = SetPort(it->as_string(), _check);	
		}
		else if (it->name() == TITLE_NAME_BAUDRATE)
		{
			ret_value = SetBaudrate(it->as_int(), _check);	
		}
		else if (it->name() == TITLE_NAME_PARITY_BIT)
		{
			ret_value = SetParityBit(it->as_string(), _check);	
		}
		else if (it->name() == TITLE_NAME_DATA_BIT)
		{
			ret_value = SetDataBit(it->as_int(), _check);	
		}

		if (ret_value != true)
		{
			break;
		}
	}

	return	ret_value;
}

const std::string&	DeviceSerial::GetPort()
{
	return	dev_name_;
}

bool	DeviceSerial::SetPort(std::string const& _dev_name, bool _check)
{
	if (!_check)
	{
		dev_name_ = _dev_name;

		JSONNodeUpdate(updated_properties_, TITLE_NAME_DEV_NAME, dev_name_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

uint32_t	DeviceSerial::GetBaudrate()
{
	switch(baudrate_)
	{
	case	B0:		return	0;
	case	B50:	return	50;
	case	B75:	return	75;
	case	B110:	return	110;
	case	B134:	return	134;
	case	B150:	return	150;
	case	B200:	return	200;
	case	B300:	return	300;
	case	B600:	return	600;
	case	B1200:	return	1200;
	case	B1800:	return	1800;
	case	B2400:	return	2400;
	case	B4800:	return	4800;
	case	B9600:	return	9600;
	case	B19200:	return	19200;
	case	B38400:	return	38400;
	case 	B57600:	return	57600;
	case	B115200:return	115200;
	case	B230400:return	230400;
	default:	0;
	}
}


bool		DeviceSerial::SetBaudrate(uint32_t _baudrate, bool _check)
{
	uint32_t	baudrate = 0;

	if (_baudrate < 50)
	{
		baudrate	=	B0;
	}
	else if (_baudrate < 75)
	{
		baudrate	=	B50;
	}
	else if (_baudrate < 110)
	{
		baudrate	=	B75;
	}
	else if (_baudrate < 134)
	{
		baudrate	=	B110;
	}
	else if (_baudrate < 150)
	{
		baudrate	=	B134;
	}
	else if (_baudrate < 200)
	{
		baudrate	=	B150;	
	}
	else if (_baudrate < 300)
	{
		baudrate	=	B200;
	}
	else if (_baudrate < 600)
	{
		baudrate	=	B300;
	}
	else if (_baudrate < 1200)
	{
		baudrate	=	B600;
	}
	else if (_baudrate < 1800)
	{
		baudrate	=	B1200;
	}
	else if (_baudrate < 2400)
	{
		baudrate	=	B1800;
	}
	else if (_baudrate < 4800)
	{
		baudrate	=	B2400;
	}
	else if (_baudrate < 9600)
	{
		baudrate	=	B4800;
	}
	else if (_baudrate < 19200)
	{
		baudrate	=	B9600;
	}
	else if (_baudrate < 38400)
	{
		baudrate	=	B19200;
	}
	else if (_baudrate < 57600)
	{
		baudrate	=	B38400;
	}
	else if (_baudrate < 115200)
	{
		baudrate	=	B57600;
	}
	else if (_baudrate < 230400)
	{
		baudrate	=	B115200;
	}
	else if (_baudrate == 230400)
	{
		baudrate	=	B230400;
	}
	else
	{
		return	false;
	}

	if (!_check)
	{
		baudrate_ = baudrate;
	}

	return	true;
}

std::string	DeviceSerial::GetParityBit()
{
	switch(parity_bit_)
	{
	case	PARENB: return	"even";
	case	PARODD: return	"odd";
	}

	return	"none";
}

bool	DeviceSerial::SetParityBit(std::string const& _parity_bit, bool _check)
{
	uint32_t	parity_bit = 0;

	if (_parity_bit == "even")
	{
		parity_bit = PARENB;
	}
	else if (_parity_bit == "odd")
	{
		parity_bit = PARODD;
	}
	else if (_parity_bit == "none")
	{
		parity_bit = 0;
	}
	else
	{
		return	false;
	}

	if (!_check)
	{
		parity_bit_ = parity_bit;	
	}

	return	true;
}

uint32_t	DeviceSerial::GetDataBit()
{
	switch(data_bit_)
	{
	case	CS5:	5;	
	case	CS6:	6;	
	case	CS7:	7;	
	case	CS8:	8;	
	}
}

bool	DeviceSerial::SetDataBit(uint32_t _data_bit, bool _check)
{
	uint32_t	data_bit = CS8;

	switch(_data_bit)
	{
	case	5:	data_bit = CS5;	
	case	6:	data_bit = CS6;	
	case	7:	data_bit = CS7;	
	case	8:	data_bit = CS8;	
	default:
		return	false;
	}

	if (!_check)
	{
		data_bit_ = data_bit;	
	}

	return	true;
}

bool	DeviceSerial::Open()
{
	if (fd_ > 0)
	{
		TRACE_ERROR("Failed to open dev[" << dev_name_ << "]");
		return	false;
	}

	fd_ = open(dev_name_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd_ <= 0)
	{
		fd_ = 0;
		TRACE_ERROR("Failed to open dev[" << dev_name_ << "]");
		return	false;
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
	new_term_ios.c_cflag = baudrate_ | data_bit_ | parity_bit_ | CLOCAL | CREAD;
	new_term_ios.c_iflag = IGNPAR | ICRNL;
	new_term_ios.c_oflag = 0;
	new_term_ios.c_lflag = 0;
	new_term_ios.c_cc[VMIN]=1;
	new_term_ios.c_cc[VTIME]=0;

	tcflush(fd_, TCIFLUSH);
	tcsetattr(fd_, TCSANOW, &new_term_ios);

	return	true;
}

bool	DeviceSerial::Close()
{
	if (fd_ != 0)
	{
		tcsetattr(fd_, TCSANOW, &old_term_ios_);

		close(fd_);	
		fd_ = 0;
	}	

	return	true;
}

bool	DeviceSerial::Write(uint8_t *buffer, uint32_t buffer_len)
{
	if (fd_ == 0)
	{
		TRACE_ERROR("The dev is not open!");
		return	false;
	}

	write(fd_, buffer, buffer_len);

	return	true;
}

bool	DeviceSerial::Read(uint8_t *buffer, uint32_t buffer_len, uint32_t& read_len)
{
	if (fd_ == 0)
	{
		TRACE_ERROR("The dev is not open!");
		return	false;
	}

	read_len = buffer_.Get((char *)buffer, buffer_len);

	return	true;
}

void 	DeviceSerial::SignalHandler(int status, siginfo_t *ioinfo, void *context )
{
}

void	DeviceSerial::Preprocess()
{
	Open();
}

void	DeviceSerial::Process()
{
	if (fd_ != 0)
	{
		char	buffer[256];
		int		read_len;

		read_len = read(fd_, buffer, sizeof(buffer));
		if (read_len > 0)
		{
			buffer_.Put(buffer, read_len);
		}
	}

	Device::Process();
}

void	DeviceSerial::Postprocess()
{
	Close();
}
