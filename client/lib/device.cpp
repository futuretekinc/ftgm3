#include <iostream>
#include <iomanip>
#include <libjson/libjson.h>
#include <map>
#include "exception.h"
#include "shell.h"
#include "object_manager.h"
#include "data_manager.h"
#include "device.h"
#include "time2.h"
#include "rcs_client.h"
#include "utils.h"
#include "json.h"

using namespace std;

static Fields	default_fields = PROPERTY_ID_FLAG | PROPERTY_NAME_FLAG | PROPERTY_TYPE_FLAG | PROPERTY_STAT_FLAG;

static RetValue	CmdDeviceAdd( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdDeviceDel( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdDeviceGet( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdDeviceSet( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdDeviceStart( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdDeviceStop( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdDeviceList( std::string* _arguments, uint32_t	_count, Shell* _shell);
static RetValue	CmdDevice ( std::string* _arguments, uint32_t	_count, Shell* _shell);

Shell::Command	object_manager_command_device
(
	"device", 
	"device\n"
	"  Device management.\n"
	"COMMANDS:\n"
	"  [empty]\n"
	"    Show device list.\n"
	"  add [<PROPERTY> ...]\n"
	"    Add new device\n"
	"  del <EPID> [<EPID> ...]\n"
	"    Delete device\n"
	"  get <PROPERTY> [<PROPERTY> ...] <EPID> [<EPID> ...]\n"
	"    Get device properties\n"
	"  set <EPID> <PROPERTY> <VALUE> [<PROPERTY> <VALUE> ...]\n"
	"    Delete device\n"
	"  list [<PROPETY> ...]\n"
	"    Show device list.\n"
	"PARAMETERS"
	"  EPID      device id\n"
	"  PROPERTY  property type\n"
	"  VALUE     property value\n",
	"device management.",
	CmdDevice
 );

RetValue	CmdDevice ( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RetValue ret_value = RET_VALUE_OK;	
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	if (_count == 1)
	{
		CmdDeviceList(NULL, 0, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_ADD)
	{
		ret_value = CmdDeviceAdd(&_arguments[2], _count - 2, _shell);	
	}
	else if (_arguments[1] == MSG_TYPE_RCS_DEL)
	{
		ret_value = CmdDeviceDel(&_arguments[2], _count - 2, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_GET)
	{
		ret_value = CmdDeviceGet(&_arguments[2], _count - 2, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_SET)
	{
		ret_value = CmdDeviceSet(&_arguments[2], _count - 2, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_START)
	{
		ret_value = CmdDeviceStart(&_arguments[2], _count - 2, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_STOP)
	{
		ret_value = CmdDeviceStop(&_arguments[2], _count - 2, _shell);
	}
	else if (_arguments[1] == MSG_TYPE_RCS_LIST)
	{
		CmdDeviceList(&_arguments[2], _count - 2, _shell);
	}
	else
	{
		THROW_INVALID_ARGUMENT("Not supported subcommand[" << _arguments[1] << "]!");	
	}

	return	RET_VALUE_OK;
}

RetValue	CmdDeviceAdd( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	JSONNode	properties;

	if ((_count < 2) || (_count % 2 != 0))
	{
		throw InvalidArgument("Invalid argument");	
	}

	for(uint32_t i = 0 ; i+1 < _count ; i+=2)
	{
		if (_arguments[i].substr(0, 2) != "--")
		{
			throw InvalidArgument("Invalid argument");	
		}

		properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].length() - 2), _arguments[i+1]));
	}

	if (client->AddDevice(properties))
	{
		_shell->Out() << "The device has been added." << std::endl;		
	}
	else
	{
		_shell->Out() << "Failed to add device device." << std::endl;		
	}

	return	RET_VALUE_OK;
}

RetValue	CmdDeviceDel( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	for(uint32_t i = 0 ; i < _count ; i++)
	{
		if (client->DelDevice(_arguments[i]))
		{
			_shell->Out() << "The device[" << _arguments[i] << "] has been deleted." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to delete device[" << _arguments[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

RetValue	CmdDeviceGet( std::string* _arguments, uint32_t	_count, Shell* _shell)
{

	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<JSONNode>		device_id_vector;
	Fields		fields;
	uint32_t	epid_offset = 0;
	std::map<std::string, uint32_t>	fields_size;

	if (_count > 1)
	{
		for(uint32_t i = 0 ; i < _count ; i++)
		{
			if (_arguments[i].substr(0, 2) != "--")
			{
				epid_offset = i;
				break;
			}
			else if (_arguments[i].size() < 3)
			{
				THROW_INVALID_ARGUMENT("Invalid argment!");
			}
			else if (_arguments[i] == "--all")
			{
				fields.Set(PROPERTY_ALL_FLAG);
			}
			else
			{
				fields.Set(_arguments[i].substr(2, _arguments[i].size() - 2));
			}
		}
	}
	else
	{
		fields = default_fields;	
	}

	if (epid_offset == _count)
	{
		if (client->GetDevice(device_id_vector) == false)
		{
			_shell->Out() << "Failed to get device list." << std::endl;
		}
	}
	else
	{
		for(uint32_t i = epid_offset ; i < _count ; i++)
		{
			JSONNode	node;
			node.push_back(JSONNode(TITLE_NAME_ID, _arguments[i]));
			device_id_vector.push_back(node);
		}
	}

	if (device_id_vector.size() != 0)
	{
		std::vector<JSONNode>	device_properties_vector;
		JSONNode	properties;

		for(uint32_t i = 0 ; i < device_id_vector.size() ; i++)
		{
			std::string	id = JSONNodeGetID(device_id_vector[i]);

			if (client->GetDevice(id, fields, properties))
			{
				device_properties_vector.push_back(properties);
			}
		}

		uint32_t	title_size = 16;

		for(uint32_t i = 0 ; i < device_properties_vector.size() ; i++)
		{
			for(JSONNode::iterator it = device_properties_vector[i].begin() ; it != device_properties_vector[i].end() ; it++)
			{
				if (title_size < it->name().size())
				{
					title_size = it->name().size();
				}
			}
		}

		for(uint32_t i = 0 ; i < device_properties_vector.size() ; i++)
		{
			std::string	id = JSONNodeGetID(device_properties_vector[i]);

			_shell->Out() << "< Device[" << id  << "] Properties >" << std::endl;
			for(JSONNode::iterator it = device_properties_vector[i].begin() ; it != device_properties_vector[i].end() ; it++)
			{
				_shell->Out() << std::setw(title_size) << it->name() << " : " << it->as_string() << std::endl;
			}
		}

	}

	return	RET_VALUE_OK;
}

RetValue	CmdDeviceSet( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	JSONNode	properties;

	if ((_count < 3) || (_count % 2 != 1))
	{
		throw InvalidArgument("Invalid argument");	
	}

	properties.push_back(JSONNode(TITLE_NAME_ID, _arguments[0]));

	for(uint32_t i = 1 ; i+1 < _count ; i+=2)
	{
		if (_arguments[i].substr(0, 2) != "--")
		{
			throw InvalidArgument("Invalid argument");	
		}

		properties.push_back(JSONNode(_arguments[i].substr(2, _arguments[i].length() - 2), _arguments[i+1]));
	}

	if (client->SetDevice(properties))
	{
		_shell->Out() << "The device[" << _arguments[0] << "] properties changed." << std::endl;	

	}
	else
	{
		_shell->Out() << "Failed to set device[" << _arguments[0] << "] properties." << std::endl;	
	}

	return	RET_VALUE_OK;
}

RetValue	CmdDeviceList( std::string* _arguments, uint32_t	_count, Shell* _shell)
{

	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<JSONNode>		device_id_vector;
	Fields						fields;
	std::map<std::string, uint32_t>	fields_size;

	if (_count > 1)
	{
		for(uint32_t i = 0 ; i < _count ; i++)
		{
			if ((_arguments[i].substr(0, 2) != "--") || _arguments[i].size() < 3)
			{
				THROW_INVALID_ARGUMENT("Invalid argment!");
			}

			fields.Set(_arguments[i].substr(2, _arguments[i].size() - 2));
		}
	}
	else
	{
		fields = default_fields;	
	}

	if (client->GetDevice(device_id_vector))
	{
		if (device_id_vector.size() != 0)
		{
			std::vector<JSONNode>	device_properties_vector;
			JSONNode	properties;

			for(uint32_t i = 0 ; i < device_id_vector.size() ; i++)
			{
				std::string	id = JSONNodeGetID(device_id_vector[i]);

				if (client->GetDevice(id, fields, properties))
				{
					device_properties_vector.push_back(properties);
				}
			}

			std::multimap<uint32_t, std::string >	fields_name_map;

			fields.Names(fields_name_map);
		
			for(std::map<uint32_t, std::string>::iterator it = fields_name_map.begin(); it != fields_name_map.end() ; it++)
			{
				fields_size.insert(fields_size.end(), std::pair<std::string, uint32_t>(it->second, it->second.size()));	
			}

			for(uint32_t i = 0 ; i < device_properties_vector.size() ; i++)
			{
				for(std::map<std::string, uint32_t>::iterator it = fields_size.begin() ; it != fields_size.end() ; it++)
				{
					JSONNode::iterator property_it = device_properties_vector[i].find(it->first);
					if (property_it != device_properties_vector[i].end())
					{
						if (fields_size[it->first] < property_it->as_string().size())
						{
							fields_size[it->first] = property_it->as_string().size();
						}
					}
				}
			}

			uint32_t i = 0;
			for(std::map<std::string, uint32_t>::iterator it = fields_size.begin() ; it != fields_size.end() ; it++)
			{
				_shell->Out() << setw(it->second+1) << it->first;
			}
			_shell->Out() << std::endl;

			for(uint32_t i = 0 ; i < device_properties_vector.size() ; i++)
			{
				for(std::map<std::string, uint32_t>::iterator it = fields_size.begin() ; it != fields_size.end() ; it++)
				{
					JSONNode::iterator property_it = device_properties_vector[i].find(it->first);
					if (property_it != device_properties_vector[i].end())
					{
						_shell->Out() << std::setw(it->second+1) << property_it->as_string();
					}
					else
					{
						_shell->Out() << std::setw(it->second+1) << "";
					}
				}
				_shell->Out() << std::endl;
			}

		}
	}
	else
	{
		_shell->Out() << "Failed to get device list." << std::endl;
	}

}

RetValue	CmdDeviceInfo( std::string* _arguments, uint32_t	_count, Shell* _shell)
{

	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());
	std::vector<JSONNode>		device_id_vector;
	Fields		fields;
	uint32_t	epid_offset = 0;
	std::map<std::string, uint32_t>	fields_size;

	if (_count > 1)
	{
		for(uint32_t i = 0 ; i < _count ; i++)
		{
			if (_arguments[i].substr(0, 2) != "--")
			{
				epid_offset = i;
				break;
			}
			else if (_arguments[i].size() < 3)
			{
				THROW_INVALID_ARGUMENT("Invalid argment!");
			}
			else if (_arguments[i] == "--all")
			{
				fields.Set(PROPERTY_ALL_FLAG);
			}
			else
			{
				fields.Set(_arguments[i].substr(2, _arguments[i].size() - 2));
			}
		}
	}
	else
	{
		fields = default_fields;	
	}

	if (epid_offset == _count)
	{
		if (client->GetDevice(device_id_vector) == false)
		{
			_shell->Out() << "Failed to get device list." << std::endl;
		}
	}
	else
	{
		for(uint32_t i = epid_offset ; i < _count ; i++)
		{
			JSONNode	node;
			node.push_back(JSONNode(TITLE_NAME_ID, _arguments[i]));
			device_id_vector.push_back(node);
		}
	}

	if (device_id_vector.size() != 0)
	{
		std::vector<JSONNode>	device_properties_vector;
		JSONNode	properties;

		for(uint32_t i = 0 ; i < device_id_vector.size() ; i++)
		{
			std::string	id = JSONNodeGetID(device_id_vector[i]);

			if (client->GetDevice(id, fields, properties))
			{
				device_properties_vector.push_back(properties);
			}
		}

		uint32_t	title_size = 16;

		for(uint32_t i = 0 ; i < device_properties_vector.size() ; i++)
		{
			for(JSONNode::iterator it = device_properties_vector[i].begin() ; it != device_properties_vector[i].end() ; it++)
			{
				if (title_size < it->name().size())
				{
					title_size = it->name().size();
				}
			}
		}

		for(uint32_t i = 0 ; i < device_properties_vector.size() ; i++)
		{
			std::string	id = JSONNodeGetID(device_properties_vector[i]);

			_shell->Out() << "< Device[" << id  << "] Properties >" << std::endl;
			for(JSONNode::iterator it = device_properties_vector[i].begin() ; it != device_properties_vector[i].end() ; it++)
			{
				_shell->Out() << std::setw(title_size) << it->name() << " : " << it->as_string() << std::endl;
			}
		}

	}

	return	RET_VALUE_OK;
}

RetValue	CmdDeviceStart( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	for(uint32_t i = 0 ; i < _count ; i++)
	{
		if (client->StartDevice(_arguments[i]))
		{
			_shell->Out() << "The device[" << _arguments[i] << "] has been started." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to start device[" << _arguments[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

RetValue	CmdDeviceStop( std::string* _arguments, uint32_t	_count, Shell* _shell)
{
	RCSClient*	client = dynamic_cast<RCSClient*>(_shell->GetObject());

	for(uint32_t i = 0 ; i < _count ; i++)
	{
		if (client->StopDevice(_arguments[i]))
		{
			_shell->Out() << "The device[" << _arguments[i] << "] has been stopped." << std::endl;		
		}
		else
		{
			_shell->Out() << "Faild to stop device[" << _arguments[i] << "]" << std::endl;		
		}
	}

	return	RET_VALUE_OK;
}

