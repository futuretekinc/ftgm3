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

	DeviceSNMP(ObjectManager& _manager, std::string const& _type);
	DeviceSNMP(ObjectManager& _manager, JSONNode const& _properties);
	DeviceSNMP(ObjectManager& _manager, std::string const& _type, JSONNode const& _properties);
	DeviceSNMP(ObjectManager& _manager, std::string const& _type, std::string const& _module);
	DeviceSNMP(ObjectManager& _manager, std::string const& _type, std::string const& _module, std::string const& _ip);
	~DeviceSNMP();

			bool	IsIncludedIn(std::string const& _type);

	const	std::string&	GetModule();
			bool			SetModule(std::string const& _module, bool _check  = false);
	const	std::string&	GetCommunity();
			bool			SetCommunity(std::string const& _community, bool _check = false);
			uint32_t		GetTimeout();
			bool			SetTimeout(uint32_t _timeout);
			bool			SetTimeout(std::string const& _timeout, bool _check =false);

	virtual	bool	GetProperties(JSONNode& _properties, Fields const& _fields = PROPERTY_ALL) ;

	virtual	bool	SetProperty(JSONNode const& _property, bool _check = false);

	virtual	bool		GetOptions(JSONNode& _properties);
	virtual	bool		SetOption(JSONNode& _properties, bool _check = false);

	virtual	Endpoint*	CreateEndpoint(JSONNode const& _properties);

	virtual	OID		GetOID(std::string const& _id);
	virtual	OID		GetOID(std::string const& _name, uint32_t index);

	virtual	bool	InsertToDB(Kompex::SQLiteStatement*	_statement);

	static	const std::string&	Type();
	static	bool	AddMIBPath(std::string const& _path);
	static	bool	ReadAllMIBs();
	static	bool	ReadMIB(std::string const& _file_name);

protected:

			void	Preprocess();
			void	Postprocess();

			bool	Open();
			bool	Close();

			bool	IsOpened();

			bool	ReadValue(std::string const& _endpoint_id, time_t& _time, std::string& _value);
			bool	ReadValue(std::string const& _endpoint_id, time_t& _time, bool& _value);
			bool	ReadValue(OID const& _oid, std::string& _value);
	static	bool	Convert(struct variable_list *_variable, std::string& _value);

	std::string		module_;
	std::string		community_;
	uint32_t		timeout_;

	struct snmp_session*	session_;
	std::map<std::string, OID>	oid_map_;
};

#endif
