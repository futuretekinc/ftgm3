#include <iostream>
#include <libjson/libjson.h>
#include "property.h"
#include "device_snmp.h"

int main(int argc, char *argv[])
{
	Properties	properties;	
	Properties	properties2;	

	properties.Append("id", 	"dev-0001");
	properties.Append("name", 	"device1");

	auto it = properties.begin();
	while(it != properties.end())
	{
		std::cout << it->first << " : " << it->second->ToString() << std::endl;
		it++;
	}

	DeviceSNMP snmp(properties);
	std::cout << "SNMP Device Properties" << std::endl;
	std::cout << snmp << std::endl;

	std::cout << "Device Count : " << Device::Count() << std::endl;
	properties.Clear();

	snmp.GetProperties(properties);

	std::cout << properties;

	Device* device = Device::Get("dev-0001");
	if (device != NULL)
	{
		std::cout << "Device found!" << std::endl;	
	}

	JSONNode json_node = properties.GetJSON();
	std::cout << "JSON : " << json_node.write_formatted() << std::endl;

	const char*	json_text = "{ \"id\" : \"1234\", \"name\" : \"json_object\", \"enable\" : 1 }";

	JSONNode	json = libjson::parse(json_text);

	properties2.Append(json);

	properties.Append("endpoint", properties2);

	std::cout << properties;

	json_node = properties.GetJSON();
	std::cout << "JSON : " << json_node.write_formatted() << std::endl;
	return	0;
}
