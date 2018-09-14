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
#include "device_u2u.h"
#include "serial_port.h"
//#include "serial_port_half.h"

DeviceU2U::DeviceU2U(ObjectManager& _manager, std::string const& _type, bool _half)
: Device(_manager, _type), serial_u2u_()
{
	serial_u2u_.SetMode(_half);
}

DeviceU2U::~DeviceU2U()
{
}

bool	DeviceU2U::GetOptions(JSONNode& _options)
{
	JSONNode	options;
	if (!Device::GetOptions(options))
	{
		return	false;
	}

	JSONNode	serial_options;
	serial_u2u_.GetOptions(serial_options);
	
	for(JSONNode::iterator it = serial_options.begin(); it != serial_options.end(); it++)
	{
		options.push_back(*it);
	}

	_options = options;

	return	true;
}

bool	DeviceU2U::SetOption(JSONNode const& _option, bool _check)
{
	bool ret_value = true;

	if (!serial_u2u_.SetOption(_option, _check))
	{
		ret_value = Device::SetOption(_option, _check);	
	}

	return	ret_value;
}

void	DeviceU2U::Preprocess()
{
	serial_u2u_.Open();

	Device::Preprocess();
}

void	DeviceU2U::Process()
{
	Device::Process();
}

void	DeviceU2U::Postprocess()
{
	Device::Postprocess();

	serial_u2u_.Close();
}


bool    DeviceU2U::RequestandWait(uint8_t* _request, uint32_t _request_len, uint8_t* _response, uint32_t _max_response_len, uint32_t& _response_len, uint32_t _timeout)
{
	Timer           response_timeout;
	
	/*if (!serial_u2u_.Write(_request, _request_len))
	{
		TRACE_ERROR("Failed to send frame!");
		return  false;
	}*/
	/*response_timeout.Add(timeout_);
	_response_len = 0;
	while(response_timeout.RemainTime() > 0)
	{
		uint32_t        read_len = 0;
		if (serial_u2u_.Read(&_response[_response_len], (_max_response_len - _response_len), read_len))
		{
			_response_len += read_len;
			if (_response_len >= _max_response_len)
			{
    				break;
	  		}
 		}
	}*/
	serial_u2u_.Read(_response, _max_response_len, _response_len);
	TRACE_INFO("Read");
	TRACE_INFO_DUMP((char *)_response, _response_len);
	if (_response_len < _max_response_len)
	{
		TRACE_INFO("_response_len : " << _response_len << " , " << _max_response_len);
		TRACE_INFO_DUMP((char *)_response, 12);
		TRACE_ERROR("Failed to receive frame!");
		return  false;
	}
	return  true;
}
