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
	DeviceFTE(ObjectManager& _manager, Properties const& _properties);
	DeviceFTE(ObjectManager& _manager, JSONNode const& _properties);
	DeviceFTE(ObjectManager& _manager, ValueIP const& _ip);

			bool		IsIncludedIn(ValueType const& _type);

	virtual	Endpoint*	CreateEndpoint(Properties const& _properties);

			OID 		GetOID(std::string const& _id);
			OID 		GetOID(ValueType const& _type, uint32_t _index);

			bool		ReadValue(std::string const& _endpoint_id, Value* _value);

	static	const	ValueType&	Type();
protected:
	virtual	bool		Attach(ValueID const& _endpoint_id);
	virtual	bool		Detach(ValueID const& _endpoint_id);


	std::map<std::string, uint32_t>	endpoint_table_;

};

#endif
