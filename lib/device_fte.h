#ifndef	DEVICE_FTE_H_
#define	DEVICE_FTE_H_

#include "device_snmp.h"

class	DeviceFTE : public DeviceSNMP
{
public:
	DeviceFTE();
	DeviceFTE(ValueIP const& _ip);


		OID 	GetOID(std::string const& _id);
protected:

};

#endif
