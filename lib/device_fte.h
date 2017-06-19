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

			bool		IsIncludedIn(std::string const& _type);

	virtual	Endpoint*	CreateEndpoint(JSONNode const& _properties);

			OID 		GetOID(std::string const& _id);
			OID 		GetOID(std::string const& _type, uint32_t _index);

			bool		ReadValue(std::string const& _endpoint_id, time_t& _time, std::string& _value);

	static	const	std::string&	Type();
protected:
	virtual	bool		Attach(std::string const& _endpoint_id);
	virtual	bool		Detach(std::string const& _endpoint_id);


	std::map<std::string, uint32_t>	endpoint_table_;

};

#endif
