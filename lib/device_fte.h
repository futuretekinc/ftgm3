#ifndef	DEVICE_FTE_H_
#define	DEVICE_FTE_H_

#include "device_snmp.h"
#include <vector>
#include <map>
#include "endpoint.h"

class	DeviceFTE : public DeviceSNMP
{
public:
	DeviceFTE();
	DeviceFTE(Properties const& _properties);
	DeviceFTE(ValueIP const& _ip);

			Type		GetType()	const {	return	FTE;	}
			bool		IsIncludedIn(Type _type);

	virtual	Endpoint*	CreateEndpoint(Properties const& _properties);

			OID 		GetOID(std::string const& _id);
			OID 		GetOID(Endpoint::Type _type, uint32_t _index);

			bool		ReadValue(std::string const& _endpoint_id, Value* _value);

protected:
	virtual	bool		Attach(Endpoint* _endpoint);
	virtual	bool		Detach(Endpoint* _endpoint);


	std::map<std::string, uint32_t>	endpoint_table_;

};

#endif
