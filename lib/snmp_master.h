#ifndef	SNMP_MASTER_H_
#define	SNMP_MASTER_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/oid.h>
#include "active_object.h"
#include "locker.h"

class	DeviceSNMP;

struct	SNMPSession
{
	SNMPSession() : session(0) {};

	struct snmp_session*	session;
};

class	SNMPMaster : public ActiveObject
{
public:
	struct	OID
	{
		oid			id[MAX_OID_LEN];
		size_t		length;
		
		OID();
		operator std::string() const;
	};

	SNMPMaster();
	~SNMPMaster();

	bool	ReadAllMIBs();
	bool	ReadMIB(std::string const& _file_name);

	bool	Open(DeviceSNMP* _device, std::string const& _ip, std::string const& _community);
	bool	Close(DeviceSNMP* _device);

	bool	ReadValue(SNMPSession* _session, OID const& _oid, uint32_t _timeout, time_t& _time, std::string& _value);

	static	int		AsyncResponse(int operation, struct snmp_session *sp, int reqid, struct snmp_pdu *pdu, void *magic);
	static	bool	Convert(struct variable_list *_variable, std::string& _value);

protected:
	Locker	locker_;
};

#endif
