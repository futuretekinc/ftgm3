#include <iostream>
#include "device_fte.h"

DeviceFTE::DeviceFTE()
: DeviceSNMP("FTE-E")
{
}

DeviceFTE::DeviceFTE(ValueIP const& _ip)
: DeviceSNMP("FTE-E", _ip)
{
}

DeviceSNMP::OID DeviceFTE::GetOID(std::string const& _id)
{
	if (_id.size() < 8)
	{
		return	oid_map_[""];
	}

	if (DeviceSNMP::GetOID(_id).length == 0)
	{

		std::string type_string = _id.substr(_id.size() - 8, 2);
		std::string index_string= _id.substr(_id.size() - 2, 2);

		uint32_t	type = strtoul(type_string.c_str(), 0, 16) & 0x3F;
		uint32_t	index = strtoul(index_string.c_str(), 0, 16);

		OID oid;
		
		oid.id[oid.length++] = 1;
		oid.id[oid.length++] = 3;
		oid.id[oid.length++] = 6;
		oid.id[oid.length++] = 1;
		oid.id[oid.length++] = 4;
		oid.id[oid.length++] = 1;
		oid.id[oid.length++] = 42251;
		oid.id[oid.length++] = 1;
		oid.id[oid.length++] = 3;
		oid.id[oid.length++] = (type << 8);
		oid.id[oid.length++] = 2;
		oid.id[oid.length++] = 1;
		oid.id[oid.length++] = 6;
		oid.id[oid.length++] = index;

		oid_map_[_id] = oid;
	}

	return	oid_map_[_id];
}

