#ifndef	DEVICE_SNMP_H_
#define	DEVICE_SNMP_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/oid.h>
#include <map>
#include "device_ip.h"
#include "property.h"

class	DeviceSNMP : public DeviceIP
{
public:
	struct	OID
	{
		oid			id[MAX_OID_LEN];
		size_t		length;
		
		OID();
		operator std::string() const;
	};

	DeviceSNMP();
	DeviceSNMP(Properties const& _properties);
	DeviceSNMP(std::string const& _module);
	DeviceSNMP(std::string const& _module, ValueIP const& _ip);
	~DeviceSNMP();

	virtual	OID		GetOID(std::string const& _id);
	virtual	OID		GetOID(std::string const& _name, uint32_t index);

	virtual	bool	GetProperties(Properties& _properties);
	virtual	bool	SetProperty(Property const& _property);

	virtual	void	Print(std::ostream& os) const;

	static	bool	AddMIBPath(std::string const& _path);
	static	bool	ReadAllMIBs();
	static	bool	ReadMIB(std::string const& _file_name);

protected:
			bool	Open();
			bool	Close();

			bool	IsOpened();

			bool	ReadValue(std::string const& _endpoint_id, Value* _value);
	static	bool	Convert(struct variable_list *_variable, Value* _value);

	std::string		module_;
	std::string		community_;
	uint32_t		timeout_;

	struct snmp_session*	session_;
	std::map<std::string, OID>	oid_map_;
};

#endif
