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
#include "serial_port_half.h"

SerialPortHalf::SerialPortHalf()
: SerialPort(),  ctrl_port_("")
{
}

SerialPortHalf::SerialPortHalf(std::string const& _comm_port, std::string const& _ctrl_port, uint32_t _baudrate, uint32_t _data_bit, std::string const& _parity_bit)
: SerialPort(_comm_port, _baudrate, _data_bit, _parity_bit), ctrl_port_(_ctrl_port)
{
}

bool	SerialPortHalf::GetOptions(JSONNode& _options)
{
	JSONNode	options;

	SerialPort::GetOptions(options);

	options.push_back(JSONNode(TITLE_NAME_CONTROL, ctrl_port_));
	
	_options = options;

	return	true;
}

bool	SerialPortHalf::SetOption(JSONNode const& _option, bool _check)
{
	bool	ret_value = true;

	if (_option.name() == TITLE_NAME_CONTROL)
	{
		ret_value = SetControlPort(_option.as_string(), _check);	
	}
	else 
	{
		ret_value = SerialPort::SetOption(_option, _check);	
	}

	return	ret_value;
}

const std::string&	SerialPortHalf::GetControlPort()
{
	return	ctrl_port_;
}

bool	SerialPortHalf::SetControlPort(std::string const& _ctrl_port, bool _check)
{
	if (!_check)
	{
		ctrl_port_ = _ctrl_port;
	}

	return	true;
}

bool	SerialPortHalf::Open()
{

	bool	ret_value;

	ret_value = SerialPort::Open();
	if (ret_value == true)
	{
		ctrl_ = open(ctrl_port_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
		if (ctrl_ <= 0)
		{
			ctrl_ = 0;
			TRACE_ERROR("Failed to open control [ " << ctrl_port_ << "]");
			ret_value = false;
		}
		else
		{
			const char*	read_enable = "0\n";
			::write(ctrl_, read_enable, strlen(read_enable));
		}
	}

	return	ret_value;
}

bool	SerialPortHalf::Close()
{
	if (ctrl_ != 0)
	{
		close(ctrl_);	
		ctrl_ = 0;
	}	

	return	SerialPort::Close();
}

bool	SerialPortHalf::Write(uint8_t *buffer, uint32_t buffer_len)
{
	const char*	write_enable = "1\n";

	if (ctrl_ == 0)
	{
		TRACE_ERROR("The dev is not open!");
		return	false;
	}

	::write(ctrl_, write_enable, strlen(write_enable));

	SerialPort::Write(buffer, buffer_len);

	return	true;
}

bool	SerialPortHalf::Read(uint8_t *buffer, uint32_t buffer_len, uint32_t& read_len)
{
	const char*	read_enable = "0\n";

	if (ctrl_ == 0)
	{
		TRACE_ERROR("The dev is not open!");
		return	false;
	}

	::write(ctrl_, read_enable, strlen(read_enable));

	read_len = SerialPort::Read(buffer, buffer_len, read_len);

	return	true;
}

bool	SerialPortHalf::Read(uint8_t *buffer, uint32_t buffer_len, uint32_t _timeout, uint32_t& read_len)
{
	const char*	read_enable = "0\n";

	if (ctrl_ == 0)
	{
		TRACE_ERROR("The dev is not open!");
		return	false;
	}

	::write(ctrl_, read_enable, strlen(read_enable));

	read_len = SerialPort::Read(buffer, buffer_len, _timeout, read_len);

	return	true;
}

