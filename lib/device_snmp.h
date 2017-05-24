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

	DeviceSNMP(ObjectManager& _manager, ValueType const& _type);
	DeviceSNMP(ObjectManager& _manager, Properties const& _properties);
	DeviceSNMP(ObjectManager& _manager, ValueType const& _type, Properties const& _properties);
	DeviceSNMP(ObjectManager& _manager, ValueType const& _type, std::string const& _module);
	DeviceSNMP(ObjectManager& _manager, ValueType const& _type, std::string const& _module, ValueIP const& _ip);
	~DeviceSNMP();

			bool	IsIncludedIn(ValueType const& _type);

	const	std::string&	GetModule();
			bool			SetModule(std::string const& _module, bool _store = true);
	const	std::string&	GetCommunity();
			bool			SetCommunity(std::string const& _community, bool _store = true);
			uint32_t		GetTimeout();
			bool			SetTimeout(uint32_t _timeout, bool _store = true);

	virtual	bool	GetProperties(Properties& _properties) const;

	virtual	Endpoint*	CreateEndpoint(Properties const& _properties);

	virtual	OID		GetOID(std::string const& _id);
	virtual	OID		GetOID(std::string const& _name, uint32_t index);

	virtual	bool	InsertToDB(Kompex::SQLiteStatement*	_statement);

	static	const	ValueType&	Type();
	static	bool	AddMIBPath(std::string const& _path);
	static	bool	ReadAllMIBs();
	static	bool	ReadMIB(std::string const& _file_name);

protected:
	virtual	bool	SetPropertyInternal(Property const& _property, bool create = false);

			void	Preprocess();
			void	Postprocess();

			bool	Open();
			bool	Close();

			bool	IsOpened();

			bool	ReadValue(std::string const& _endpoint_id, Value* _value);
			bool	ReadValue(OID const& _oid, Value* _value);
	static	bool	Convert(struct variable_list *_variable, Value* _value);

	std::string		module_;
	std::string		community_;
	uint32_t		timeout_;

	struct snmp_session*	session_;
	std::map<std::string, OID>	oid_map_;
};

#endif
