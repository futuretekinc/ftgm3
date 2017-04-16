#include "object_manager.h"
#include <fstream>
#include <libjson/libjson.h>
#include "device.h"

ObjectManager::ObjectManager()
: ActiveObject()
{
}

ObjectManager::~ObjectManager()
{
	for(auto it = device_map_.begin() ; it != device_map_.end() ; it++)
	{
		delete it->second;
	}
}

bool ObjectManager::Load(std::string const&  _file_name)
{
	std::fstream	fs(_file_name, std::fstream::in);
	if (fs)
	{
		fs.seekg (0, fs.end);
		int length = fs.tellg();
		fs.seekg (0, fs.beg);

		char * buffer = new char [length + 1];
		fs.read(buffer, length);
		buffer[length] = 0;
		fs.close();

		Load(buffer);

		delete buffer;
	}

	return	false;
}

bool ObjectManager::Load(const char *_buffer)
{
	if (!libjson::is_valid(_buffer))
	{
		std::cout << "Invalid json format" << std::endl;
		return	false;
	}

	JSONNode	json = libjson::parse(_buffer);

	return	Load(json);
}

bool	ObjectManager::Load(JSONNode const& _json)
{
	if (_json.name() == "devices")
	{
		if (_json.type() != JSON_ARRAY)
		{
			return	false;
		}

		for(auto it = _json.begin(); it != _json.end() ; it++)
		{
			Device *device = Device::Create(*it);

			if (device != NULL)
			{
				device_map_[device->GetID()] = device;
			}
		}
	}
	else if (_json.type() != JSON_NODE)
	{
		std::cout << "Invalid json format" << std::endl;
		return	false;
	}
	else
	{
		for(auto it = _json.begin(); it != _json.end() ; it++)
		{
			if (!Load(*it))
			{
				std::cout << "Invalid format" << std::endl;
				return	false;
			}
		}
	}

	return	true;
}

void	ObjectManager::Preprocess()
{
}

void	ObjectManager::Process()
{
}

void	ObjectManager::Postprocess()
{
}
