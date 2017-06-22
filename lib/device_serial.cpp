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
: Device(_manager, _type)
{
	if (!_half)
	{
		serial_ = new SerialPort;
	}
	else
	{
		serial_ = new SerialPortHalf;
	}
}

DeviceSerial::~DeviceSerial()
{
	delete serial_;
}

bool	DeviceSerial::GetProperties(JSONNode& _properties, Fields const& _fields)
{
	if (!Device::GetProperties(_properties, _fields))
	{
		return	false;
	}

	if (_fields.options)
	{
		JSONNode	options;

		serial_->GetOptions(options);
		
		_properties.push_back(JSONNode(TITLE_NAME_OPTIONS, options.write()));
	}

	return	true;
}

bool	DeviceSerial::SetProperty(JSONNode const& _property, bool _check)
{
	bool ret_value = true;

	if (_property.name() == TITLE_NAME_OPTIONS)
	{
		if (!libjson::is_valid(_property.as_string()))
		{
			ret_value = false;
		}
		else
		{
			JSONNode	options = libjson::parse(_property.as_string());
			ret_value = serial_->SetOptions(options, _check);
		}
	}
	else
	{
		ret_value = Device::SetProperty(_property, _check);	
	}

	return	ret_value;
}

void	DeviceSerial::Preprocess()
{
	serial_->Open();

	Device::Preprocess();
}

void	DeviceSerial::Process()
{
	Device::Process();
}

void	DeviceSerial::Postprocess()
{
	Device::Postprocess();

	serial_->Close();
}
