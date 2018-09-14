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
#include "serial_u2u.h"


#define SILABS_VID                      0x10c4
#define CP2110_PID                      0xea80
#define CP2114_PID                      0xeab0

Serialu2u::Serialu2u()
: port_(""), ctrl_port_(""), baudrate_(115200), parity_bit_("none"), data_bit_(CS8), device_serial_(NULL),Vid(SILABS_VID),Pid(CP2110_PID)
{
}

Serialu2u::Serialu2u(std::string const& _port, uint32_t _baudrate, uint32_t _data_bit, std::string const& _parity_bit)
: port_(_port), ctrl_port_(""), fd_(0), mode_(false), device_serial_(NULL),Vid(SILABS_VID),Pid(CP2110_PID)
{
	SetBaudrate(_baudrate);
	SetDataBit(_data_bit);
	SetParityBit(_parity_bit);
}

Serialu2u::Serialu2u(std::string const& _port, std::string const& _ctrl_port, uint32_t _baudrate, uint32_t _data_bit, std::string const& _parity_bit)
: port_(_port), ctrl_port_(_ctrl_port), fd_(0), mode_(false),device_serial_(NULL),Vid(SILABS_VID),Pid(CP2110_PID)
{
	SetBaudrate(_baudrate);
	SetDataBit(_data_bit);
	SetParityBit(_parity_bit);
}

bool	Serialu2u::GetOptions(JSONNode& _options)
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

bool	Serialu2u::SetOptions(JSONNode const& _options, bool _check)
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

bool	Serialu2u::SetOption(JSONNode const& _option, bool _check)
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


	return	ret_value;
}

const std::string&	Serialu2u::GetPort()
{
	return	port_;
}

bool	Serialu2u::SetPort(std::string const& _port, bool _check)
{
	if (!_check)
	{
		port_ = _port;
	}

	return	true;
}

const std::string&	Serialu2u::GetControlPort()
{
	return	ctrl_port_;
}

bool	Serialu2u::SetControlPort(std::string const& _ctrl_port, bool _check)
{
	if (!_check)
	{
		ctrl_port_ = _ctrl_port;
	}

	return	true;
}

uint32_t	Serialu2u::GetBaudrate()
{
	return	baudrate_;
}


bool		Serialu2u::SetBaudrate(uint32_t _baudrate, bool _check)
{
	if (!_check)
	{
		baudrate_ = _baudrate;
	}

	return	true;
}

std::string	Serialu2u::GetParityBit()
{
	
	return	parity_bit_;
}

bool	Serialu2u::SetParityBit(std::string const& _parity_bit, bool _check)
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

uint32_t	Serialu2u::GetDataBit()
{
	return	data_bit_;
}

bool	Serialu2u::SetDataBit(uint32_t _data_bit, bool _check)
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

bool	Serialu2u::GetMode()
{
	return	mode_;
}

bool	Serialu2u::SetMode(bool _mode, bool _check)
{
	mode_ = _mode;

	return	true;
}

bool	Serialu2u::Open()
{
	   HID_UART_STATUS status;
	   
	   uint32_t numDevices;
	   
	   status = HidUart_GetNumDevices(&numDevices, Vid, Pid);
	   if (status != HID_UART_SUCCESS)
		   return status;
	   
	   bool found = false;
	   uint16_t index = 0;
	   
	   for (uint16_t i = 0; i < numDevices; i++)
	   {
		   HID_UART_DEVICE_STR serial;
		   if (HidUart_GetString(i, Vid, Pid, serial, HID_UART_GET_SERIAL_STR) == HID_UART_SUCCESS)
		   {
			   if (port_ == serial)
			   {
				   found = true;
				   index = i;
		      	   }
	  	   }
    	   }
	   if (!found)
		   return HID_UART_DEVICE_NOT_FOUND;
	   status = HidUart_Open(&device_serial_, index, Vid, Pid);
	   
	   // Set read timeouts for quickest response time
	   if (status == HID_UART_SUCCESS)
	   {
		   status = HidUart_SetTimeouts(device_serial_, 0, 1000);
	   }
	   else
	   {
		   return status;
	   }
	   
	   
	   
	   uint32_t baudrate = baudrate_;
	   
	   uint8_t parity_bit = HID_UART_NO_PARITY ;
   	   if (parity_bit_ == "even")
   	   {
   		   parity_bit = HID_UART_EVEN_PARITY;
   	   }
   	   else if (parity_bit_ == "odd")
   	   {
   		   parity_bit = HID_UART_ODD_PARITY;
   	   }
	   
	   uint8_t	data_bit =  HID_UART_EIGHT_DATA_BITS;
	   switch(data_bit_)
	   {
		   case	5:	data_bit = HID_UART_FIVE_DATA_BITS;	
		   case	6:	data_bit = HID_UART_SIX_DATA_BITS;	
		   case	7:	data_bit = HID_UART_SEVEN_DATA_BITS;	
		   case	8:	data_bit = HID_UART_EIGHT_DATA_BITS;	
	   }

	   uint8_t stop_bit = HID_UART_SHORT_STOP_BIT;
	   uint8_t flow_control = HID_UART_NO_FLOW_CONTROL;

	   status = HidUart_SetUartConfig(device_serial_, baudrate, data_bit, parity_bit, stop_bit, flow_control);
	   if (status != HID_UART_SUCCESS)
		   TRACE_ERROR("Failed setting UART config :" << port_); 
	   
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

bool	Serialu2u::Close()
{
	if (mode_)
	{
		if (ctrl_ != 0)
		{
			close(ctrl_);	
			ctrl_ = 0;
		}	
	}

	if (device_serial_ != NULL)
	{
		HidUart_Close(device_serial_);
		device_serial_ = NULL;
	}	

	return	true;
}

void	Serialu2u::SetDirectionOut(bool _out)
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
			TRACE_ERROR("The dev is not open!");
		}
	}
}

bool	Serialu2u::Write(uint8_t *buffer, uint32_t buffer_len)
{
	HID_UART_STATUS status;
	uint32_t numBytesWritten;

	if (device_serial_ == NULL )
	{
		TRACE_ERROR("The dev is not open!");
		return	false;
	}

	uint32_t 	output_time = 1000000 / baudrate_ * (buffer_len) * 2 / 3;


	TRACE_INFO_DUMP((char *)buffer, buffer_len);

	uint64_t	start_time = Date::GetCurrent().GetMicroSecond();
	SetDirectionOut(true);

	status = HidUart_Write(device_serial_, buffer, buffer_len, &numBytesWritten);
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

bool	Serialu2u::Read(uint8_t *buffer, uint32_t buffer_len, uint32_t& read_len)
{
	HID_UART_STATUS status;
	uint8_t rx[HID_UART_MAX_READ_SIZE];
	uint8_t rx_temp[HID_UART_MAX_READ_SIZE];
	uint32_t rx_temp_size = 0;
	uint32_t	numBytesRead;
	
	if (device_serial_ == NULL)
	{
		TRACE_ERROR("The dev is not open!");
		return	false;
	}

	while(1)
	{
		status = HidUart_Read(device_serial_, rx, HID_UART_MAX_READ_SIZE, &numBytesRead);
		if ((status == HID_UART_SUCCESS || status == HID_UART_READ_TIMED_OUT) && numBytesRead > 0)
		{
			memcpy(rx_temp+rx_temp_size, rx, numBytesRead);
			rx_temp_size = rx_temp_size + numBytesRead;
			if(rx_temp_size < buffer_len)
			{
				continue;
			}
			else
			{
				memcpy(buffer, rx_temp, rx_temp_size);
				break;
			}
		}

	}
	//	read_len = numBytesRead;
	read_len = rx_temp_size;
	return	true;
}

bool	Serialu2u::Read(uint8_t *buffer, uint32_t buffer_len, uint32_t _timeout, uint32_t& read_len)
{

	return	true;
}

void 	Serialu2u::SignalHandler(int status, siginfo_t *ioinfo, void *context )
{
}

