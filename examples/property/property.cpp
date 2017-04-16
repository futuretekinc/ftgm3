#include <iostream>
#include <iomanip>
#include <libjson/libjson.h>
#include "property.h"
#include "device_snmp.h"
#include "endpoint.h"

int main(int argc, char *argv[])
{

	std::cout << std::endl << std::setw(80) << std::setfill('#') << '#' << std::endl;
	std::cout << "[ Properties Test ]" << std::endl;

	Properties	properties;	
	std::cout << "Properties properties;" << std::endl;
	std::cout << std::endl << "print properties" << std::endl;
	std::cout << "=> " << std::endl;
	std::cout << "properties.Append(\"id\", \"dev-0001\");" << std::endl;
	std::cout << "=> " << properties.Append("id", 	"dev-0001") << std::endl;
	std::cout << std::endl << "print properties" << std::endl;
	std::cout << "=> " << std::endl;
	std::cout << properties << std::endl;
	std::cout << "properties.Append(\"name\", \"device1\");" << std::endl;
	std::cout << "=> " << properties.Append("name", 	"device1") << std::endl;
	std::cout << std::endl << "print properties" << std::endl;
	std::cout << "=> " << std::endl;
	std::cout << properties << std::endl;

	std::cout << std::endl << std::setw(80) << std::setfill('#') << '#' << std::endl;
	std::cout << "[ Create Deivce with properties ]" << std::endl;
	std::cout << "DeviceSNMP snmp(properties);" << std::endl;
	DeviceSNMP snmp(properties);
	std::cout << std::endl << "print snmp" << std::endl;
	std::cout << " =>" << std::endl;
	std::cout << snmp << std::endl;

	std::cout << std::endl << std::setw(80) << std::setfill('#') << '#' << std::endl;
	std::cout << "[ Get SNMP Device Properties ]" << std::endl;
	std::cout << "Properties	properties2;" << std::endl;
	Properties	properties2;	
	std::cout << "snmp.GetProperties(properties2);" << std::endl;
	std::cout << "=> " << snmp.GetProperties(properties2) << std::endl;

	std::cout << "print properties2" << std::endl;
	std::cout << "=>" << std::endl;
	std::cout << properties2;

	std::cout << std::endl << std::setw(80) << std::setfill('#') << '#' << std::endl;
	std::cout << "[ Get SNMP Device from global set ]" << std::endl;
	std::cout << "Get Device count =>" << Device::Count() << std::endl;
	std::cout << "Get(\"dev-0001\");" << std::endl;
	std::cout << "=> ";
	Device* device = Device::Get("dev-0001");
	if (device != NULL)
	{
		std::cout << "Device found!" << std::endl;	
	}
	else
	{
		std::cout << "Device not found!" << std::endl;	
	}

	std::cout << std::endl << std::setw(80) << std::setfill('#') << '#' << std::endl;
	std::cout << "[ Get JSON from properties ]" << std::endl;
	std::cout << "JSONNode json_node = properties.GetJSON();" << std::endl;
	JSONNode json_node = ToJSON(properties);
	std::cout << "print json_node" << std::endl;
	std::cout << "=> " << std::endl;
	std::cout << json_node.write_formatted() << std::endl;

	
	std::cout << std::endl << std::setw(80) << std::setfill('#') << '#' << std::endl;
	std::cout << "[ Test of creation properties from JSON stream ]" << std::endl;
	const char*	json_text = "{   \"type\" : \"snmp\","
								"\"id\" : \"1234\","
								"\"name\" : \"json_object\","
								"\"enable\" : 1 , "
								"\"endpoints\" :"
								"["
								"    {"
								"        \"type\" : \"sensor_temperature\","
								"        \"id\" : \"ep1\""
								"    },"
								"    {"
								"         \"type\" : \"sensor_temperature\","
								"         \"id\" : \"ep2\""
								"    }"
								"]"
							"}";
	std::cout << "Input text" << std::endl;
	std::cout << "=> " << std::endl;
	std::cout << json_text << std::endl;

	std::cout << "JSONNode json = libjson::parse(text)" << std::endl;
	JSONNode	json = libjson::parse(json_text);
	std::cout << "print json" << std::endl;
	std::cout << "=> " << std::endl;
	std::cout << json.write_formatted() << std::endl;

	std::cout << std::endl << std::setw(80) << std::setfill('#') << '#' << std::endl;
	std::cout << "[ Create properties from JSON ]" << std::endl;
	Properties	properties3;	
	std::cout << "properties3.Append(json)" << std::endl;
	std::cout << "=> " << properties3.Append(json) << std::endl;
	std::cout << "print properties3" << std::endl;
	std::cout << "=> " << std::endl;
	std::cout << properties3 << std::endl;

	std::cout << std::endl << std::setw(80) << std::setfill('#') << '#' << std::endl;
	std::cout << "[ Create device2 from properties3 ]" << std::endl;
	std::cout << "device2 = Device::Create(properties3)" << std::endl;
	Device *device2 = Device::Create(properties3);
	std::cout << "print device2" << std::endl;
	std::cout << "=> " << std::endl;
	std::cout << *device2 << std::endl;

	std::cout << std::endl << std::setw(80) << std::setfill('#') << '#' << std::endl;
	std::cout << "[ Verify creation endpoint ]" << std::endl;
	std::cout << "Endpoint Count => " << Endpoint::Count() << std::endl;
	Endpoint* endpoint = Endpoint::Get("ep1");
	if (endpoint != NULL)
	{
		std::cout << "endpoint ep1 found!" << std::endl;	
	}

	std::cout << std::endl << std::setw(80) << std::setfill('#') << '#' << std::endl;
	std::cout << "[ Verify detach endpoint ]" << std::endl;
	std::cout << "device2->Detach(\"ep2\")" << std::endl;
	bool detached = device2->Detach("ep2");
	std::cout << "Endpoint Count => " << Endpoint::Count() << std::endl;
	delete device2;
	return	0;
}
