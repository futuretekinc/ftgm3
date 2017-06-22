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
#include "serial_port.h"
#include "serial_port_half.h"

DeviceSerial::DeviceSerial(ObjectManager& _manager, std::string const& _type, bool _half)
: Device(_manager, _type), serial_()
{
	serial_.SetMode(_half);
}

DeviceSerial::~DeviceSerial()
{
}

bool	DeviceSerial::GetOptions(JSONNode& _options)
{
	JSONNode	options;
	if (!Device::GetOptions(options))
	{
		return	false;
	}

	JSONNode	serial_options;
	serial_.GetOptions(serial_options);
	
	for(JSONNode::iterator it = serial_options.begin(); it != serial_options.end(); it++)
	{
		options.push_back(*it);
	}

	_options = options;

	return	true;
}

bool	DeviceSerial::SetOption(JSONNode const& _option, bool _check)
{
	bool ret_value = true;

	if (!serial_.SetOption(_option, _check))
	{
		ret_value = Device::SetOption(_option, _check);	
	}

	return	ret_value;
}

void	DeviceSerial::Preprocess()
{
	serial_.Open();

	Device::Preprocess();
}

void	DeviceSerial::Process()
{
	Device::Process();
}

void	DeviceSerial::Postprocess()
{
	Device::Postprocess();

	serial_.Close();
}
