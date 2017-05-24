#ifndef	DEVICE_SIM_H_
#define	DEVICE_SIM_H_

#include "device_snmp.h"
#include <vector>
#include <map>
#include "endpoint.h"

class	DeviceSIM : public DeviceSNMP
{
public:
	DeviceSIM(ObjectManager& _manager);
	DeviceSIM(ObjectManager& _manager, Properties const& _properties);
	DeviceSIM(ObjectManager& _manager, ValueIP const& _ip);

			bool		IsIncludedIn(ValueType const& _type);

	virtual	Endpoint*	CreateEndpoint(Properties const& _properties);

			OID 		GetOID(std::string const& _id);
			OID 		GetOID(Endpoint::Type _type, uint32_t _index);

			bool		ReadValue(std::string const& _endpoint_id, Value* _value);

	static	const	ValueType&	Type();
protected:
	virtual	bool		Attach(ValueID const& _endpoint_id);
	virtual	bool		Detach(ValueID const& _endpoint_id);


	std::map<std::string, uint32_t>	endpoint_table_;

};

#endif
