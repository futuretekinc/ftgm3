#ifndef	DEVICE_SNMP_H_
#define	DEVICE_SNMP_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/oid.h>
#include <map>
#include "device_ip.h"
#include "property.h"
#include "snmp_master.h"

class	DeviceSNMP : public DeviceIP
{
public:
	DeviceSNMP(ObjectManager& _manager, std::string const& _type);
	DeviceSNMP(ObjectManager& _manager, JSONNode const& _properties);
	DeviceSNMP(ObjectManager& _manager, std::string const& _type, JSONNode const& _properties);
	DeviceSNMP(ObjectManager& _manager, std::string const& _type, std::string const& _module);
	DeviceSNMP(ObjectManager& _manager, std::string const& _type, std::string const& _module, std::string const& _ip);
	~DeviceSNMP();

			bool	IsIncludedIn(std::string const& _type);

	const	std::string&	GetModule();
			bool		SetModule(std::string const& _module, bool _check  = false);
	const	std::string&	GetCommunity();
			bool		SetCommunity(std::string const& _community, bool _check = false);
			uint32_t	GetTimeout();
			bool		SetTimeout(uint32_t _timeout);
			bool		SetTimeout(std::string const& _timeout, bool _check =false);

	virtual	bool		GetProperty(uint32_t _type, JSONNode& _property) ;
	virtual	bool		SetProperty(JSONNode const& _property, bool _check = false);

	virtual	bool		GetOptions(JSONNode& _properties);
	virtual	bool		SetOption(JSONNode& _properties, bool _check = false);

	virtual	Endpoint*	CreateEndpoint(JSONNode const& _properties);

	virtual	SNMP::OID	GetOID(std::string const& _id);
	virtual	SNMP::OID	GetOID(std::string const& _name, uint32_t index);

	virtual	bool	InsertToDB(Kompex::SQLiteStatement*	_statement);

protected:
	friend	class	SNMPMaster;

			void	Preprocess();
			void	Postprocess();

			bool	Open();
			bool	Close();

			bool	IsOpened();

			bool	ReadValue(std::string const& _endpoint_id, time_t& _time, std::string& _value);
			bool	ReadValue(std::string const& _endpoint_id, time_t& _time, bool& _value);
			bool	ReadValue(SNMP::OID const& _oid, time_t& _time, std::string& _value);
			bool	ReadValue(SNMP::OID const& _oid, std::string& _value);

			bool	WriteValue(std::string const& _id, std::string const& _value);
			bool	WriteValue(std::string const& _id, bool _value);
			bool	WriteValue(SNMP::OID const& _oid, std::string const& _value);
			bool	WriteValue(SNMP::OID const& _oid, bool _value);

	std::string		module_;
	std::string		community_;
	uint32_t		timeout_;

	SNMP::Session		session_;
	std::map<std::string, SNMP::OID>	oid_map_;

	std::string		async_value_;

//	Static functions
public:
	static	const char*	Type();
	static	bool		AddMIBPath(std::string const& _path);
	static	bool		ReadAllMIBs();
	static	bool		ReadMIB(std::string const& _file_name);

};

#endif
