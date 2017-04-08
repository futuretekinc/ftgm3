#include <iostream>
#include <string>
#include "device_fte.h"


int main(int argc, char *argv[])
{
	DeviceSNMP*	device = new DeviceFTE();

	DeviceSNMP::OID oid = device->GetOID("01000001");
	std::cout << "OID1 : " << std::string(oid) << std::endl;	

	DeviceSNMP::OID oid2 = device->GetOID("000001");
	std::cout << "OID2 : " << std::string(oid2) << std::endl;	

	DeviceSNMP::AddMIBPath(".");
	DeviceSNMP::ReadAllMIBs();

	DeviceSNMP::OID oid3 = device->GetOID("FTE-E::tempValue", 1);
	std::cout << "OID3 : " << std::string(oid3) << std::endl;	

	DeviceSNMP::OID oid4 = device->GetOID("tempValue", 1);
	std::cout << "OID4 : " << std::string(oid4) << std::endl;	

	delete device;
	return	0;
}
