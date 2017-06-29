#ifndef	SNMP_MASTER_H_
#define	SNMP_MASTER_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/oid.h>
#include "active_object.h"
#include "locker.h"

class	DeviceSNMP;

namespace SNMP
{

struct	OID
{
	oid			id[MAX_OID_LEN];
	size_t		length;

	OID();
	operator std::string() const;
};

class	Session;

class	Master : public ActiveObject
{
	friend class	Session;
public:
	Master();
	~Master();

	bool	ReadAllMIBs();
	bool	ReadMIB(std::string const& _file_name);

	bool	Open(Session& _session, std::string const& _ip, std::string const& _community);
	bool	Close(Session& _session);

protected:

	bool	ReadValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, time_t& _time, std::string& _value);
	bool	SyncReadValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, time_t& _time, std::string& _value);
	bool	AsyncRequestReadValue(snmp_session* _session, OID const& _oid, uint32_t _timeout);

	static	int		AsyncResponse(int operation, struct snmp_session *sp, int reqid, struct snmp_pdu *pdu, void *magic);
	static	bool	Convert(struct variable_list *_variable, std::string& _value);

			void	Process();

	Locker	locker_;

	std::list<Session*>	session_list_;
};

class	Session	
{
	friend class	Master;
public:
	Session();
	~Session();

	bool		Open(std::string const& _ip, std::string const& _community);
	bool		Close();

	bool		IsOpened();

	bool		SetTimeout(uint32_t _timeout);
	uint32_t	GetTimeout();

	bool		ReadValue(OID const& _oid, time_t& _time, std::string& _value);
	bool		AsyncReadValue(OID const& _oid, time_t& _time, std::string& _value);

protected:

	Master&					master_;
	struct snmp_session*	session_;
	uint32_t				timeout_;
	Locker					finished_;

	time_t					time_;
	std::string				value_;
};

}
#endif
