#ifndef	DEVICE_FTE_H_
#define	DEVICE_FTE_H_

#include "device_snmp.h"
#include <vector>
#include <map>
#include "endpoint.h"

class	DeviceFTE : public DeviceSNMP
{
public:
	DeviceFTE(ObjectManager& _manager);
	DeviceFTE(ObjectManager& _manager, JSONNode const& _properties);
	DeviceFTE(ObjectManager& _manager, std::string const& _ip);

			const char*	GetClassName();

			bool		IsIncludedIn(std::string const& _type);

	virtual	Endpoint*	CreateEndpoint(JSONNode const& _properties);

			SNMP::OID 	GetOID(std::string const& _type, uint32_t _index);

			bool		ReadValue(std::string const& _endpoint_id, time_t& _time, std::string& _value);

	static	const char*	Type();

protected:
};

#endif
