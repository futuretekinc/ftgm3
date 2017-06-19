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
: Device(_manager, _type), dev_name_(""), baudrate_(B115200), buffer_(1024)
{
}

DeviceSerial::DeviceSerial(ObjectManager& _manager, std::string const& _type, std::string const& _port)
: Device(_manager, _type), dev_name_(_port), buffer_(1024)
{
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


bool	DeviceSerial::GetProperties(JSONNode& _properties, Fields const& _fields) 
{
	if (Device::GetProperties(_properties))
	{
		if (_fields.dev_name)
		{
			_properties.push_back(JSONNode(TITLE_NAME_DEV_NAME, dev_name_));
		}

		return	true;
	}

	return	false;
}

bool	DeviceSerial::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;

	if (_property.name() == TITLE_NAME_DEV_NAME)
	{
		ret_value = SetPort(_property.as_string(), _check);
	}
	else
	{
		ret_value = Device::SetProperty(_property, _check);
	}

	return	ret_value;
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
	new_term_ios.c_cflag = baudrate_ | CS8 | CLOCAL | CREAD;
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
