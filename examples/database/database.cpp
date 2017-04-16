#include <iostream>
#include <string>
#include <unistd.h>
#include "trace.h"
#include "device.h"
#include "database_manager.h"

bool	CreateDevice1(DatabaseManager& dbm);
bool	CreateDevice2(DatabaseManager& dbm);

int main(int argc, char *argv[])
{
	DatabaseManager	db_manager;

	db_manager.Start();

	CreateDevice1(db_manager);
	CreateDevice2(db_manager);

	db_manager.Stop();

	return	0;
}

bool	CreateDevice1(DatabaseManager& dbm)
{
	Properties	device_properties;

	device_properties.Append("id", "devid-0001");
	device_properties.Append("name", "devname-0001");
	device_properties.Append("type", "snmp");

	if (!dbm.IsDeviceExist("devid-0001"))
	{
		std::cout << "Device[" << "devid-0001" << "] is not exist" << std::endl;

		Device*	dev01 = Device::Create(device_properties);
		if (dev01 == NULL)
		{
			std::cout << "Failed to create device!" << std::endl;	
			return	false;
		}
		else
		{
			std::cout << "Device created!" << std::endl;
		}
		if (!dbm.AddDevice(dev01))
		{
			std::cout << "Failed to device to DB!" << std::endl;	
		}
		else
		{
			std::cout << "Device added to DB!" << std::endl;

			dbm.DeleteDevice(dev01->GetName());
		}

		delete dev01;
	}
	else
	{
		std::cout << "Device[" << "devid-0001" << "] is exist" << std::endl;
		return	false;
	}

	return	true;
}

bool	CreateDevice2(DatabaseManager& dbm)
{
	Properties	device_properties;
	std::string	dev_id = "devid-0002";
	std::string dev_name = "devname-0002";
	std::string dev_type = "snmp";
	std::string	ep_id0 = "ep-00";
	std::string	ep_id1 = "ep-01";

	JSONNode	endpoint_array(JSON_ARRAY);
	JSONNode	root;
	JSONNode	ep = JSONNode("id", ep_id0);
	endpoint_array.push_back(ep);

	ep = JSONNode("id", ep_id0);
	endpoint_array.push_back(JSONNode("id", ep_id0));

	std::cout << endpoint_array.write() << std::endl;
	root.push_back(JSONNode("id", dev_id));
	root.push_back(JSONNode("name", dev_name));
	root.push_back(JSONNode("type", dev_type));

	device_properties.Append(root);
#if 0
	if (!dbm.IsDeviceExist("devid-0002"))
	{
		std::cout << "Device[" << "devid-0001" << "] is not exist" << std::endl;

		Device*	dev01 = Device::Create(device_properties);
		if (dev01 == NULL)
		{
			std::cout << "Failed to create device!" << std::endl;	
			return	false;
		}
		else
		{
			std::cout << "Device created!" << std::endl;
		}
		if (!dbm.AddDevice(dev01))
		{
			std::cout << "Failed to device to DB!" << std::endl;	
		}
		else
		{
			std::cout << "Device added to DB!" << std::endl;

			dbm.DeleteDevice(dev01->GetName());
		}

		delete dev01;
	}
	else
	{
		std::cout << "Device[" << "devid-0001" << "] is exist" << std::endl;
		return	false;
	}
#endif
	return	true;
}
