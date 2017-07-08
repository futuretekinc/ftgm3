#ifndef	SNMP_MASTER_H_
#define	SNMP_MASTER_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/oid.h>
#include "active_object.h"
#include "locker.h"

class	DeviceSNMP;

#define	MSG_TYPE_SNMP			0x00020000
#define	MSG_TYPE_SNMP_GET		(MSG_TYPE_SNMP + 1)
#define	MSG_TYPE_SNMP_SET		(MSG_TYPE_SNMP + 2)

namespace SNMP
{

struct	OID
{
	oid			id[MAX_OID_LEN];
	size_t		length;

	OID();
	operator std::string() const;
};

class	GetMessage : public Message
{
	friend class	Master;
public:
	GetMessage(snmp_session* _session, OID const& _oid, uint32_t _timeout);

protected:
	snmp_session* 	session_; 
	OID				oid_;
	uint32_t	 	timeout_;
};

class	SetMessage : public Message
{
	friend class	Master;
public:
	SetMessage(snmp_session* _session, OID const& _oid, uint32_t _timeout, std::string const& _value);

protected:
	snmp_session* 	session_; 
	OID				oid_;
	uint32_t	 	timeout_;
	std::string		value_;
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

	bool	Attach(Session* _session);
	bool	Detach(Session* _session);

	bool	Open(Session* _session, std::string const& _ip, std::string const& _community);
	bool	Close(Session* _session);
	uint32_t	GetSessionCount();

protected:

	bool	ReadValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, time_t& _time, std::string& _value);
	bool	WriteValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, uint32_t _value);
	bool	SyncReadValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, time_t& _time, std::string& _value);
	bool	AsyncRequestReadValue(snmp_session* _session, OID const& _oid, uint32_t _timeout);
	bool	AsyncRequestWriteValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, uint32_t _value);
	bool	AsyncRequestWriteValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, std::string const& _value);
	bool	AsyncRequestReadValue(Session& _session, OID const& _oid, uint32_t _timeout);
	bool	AsyncRequestWriteValue(Session& _session, OID const& _oid, uint32_t _timeout, uint32_t _value);
	bool	AsyncRequestWriteValue(Session& _session, OID const& _oid, uint32_t _timeout, std::string const& _value);

	static	int		AsyncResponse(int operation, struct snmp_session *sp, int reqid, struct snmp_pdu *pdu, void *magic);
	static	bool	Convert(struct variable_list *_variable, std::string& _value);

			void	Preprocess();
			void	Process();

			bool	OnMessage(Message* _base);

	Locker	locker_;
	uint32_t			request_count_;

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
	bool		AsyncWriteValue(OID const& _oid, std::string const& _value);

protected:

	Master*					master_;
	struct snmp_session*	session_;
	uint32_t				timeout_;
	Locker					locker_;
	Locker					finished_;
	bool					success_;

	time_t					time_;
	std::string				value_;
};

}
#endif
