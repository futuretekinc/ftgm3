#include "defined.h"
#include "snmp_master.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "time2.h"
#include "device_snmp.h"
#include "utils.h"

using namespace SNMP;

OID::OID()
{
	memset(id, 0, sizeof(id));
	length = 0;
}

OID::operator std::string() const
{
	std::ostringstream	oss;

	for(int i = 0 ; i < length ; i++)
	{
		oss << "." << id[i];
	}

	return	oss.str();
}

GetMessage::GetMessage(snmp_session* _session, OID const& _oid, uint32_t _timeout)
: Message(MSG_TYPE_SNMP_GET), session_(_session), oid_(_oid), timeout_(_timeout)
{
}

SetMessage::SetMessage(snmp_session* _session, OID const& _oid, uint32_t _timeout, std::string const& _value)
: Message(MSG_TYPE_SNMP_SET), session_(_session), oid_(_oid), timeout_(_timeout), value_(_value)
{
}

Master::Master()
: ActiveObject(), locker_(), request_count_(0)
{
	name_ 	= "SNMP Master";
	enable_ = true;
	trace.SetClassName(GetClassName());

	init_snmp("ftgm");
	init_mib();
}

Master::~Master()
{
	shutdown_mib();
}

bool	Master::ReadAllMIBs()
{
	return	(read_all_mibs() != 0);
}

bool	Master::ReadMIB(std::string const& _file_name)
{
	return (read_mib(_file_name.c_str()) != 0);
}


bool	Master::Attach(Session* _session)
{
	for(std::list<Session*>::iterator it = session_list_.begin() ; it != session_list_.end() ; it++)
	{
		if ((*it) == _session)
		{
			TRACE_ERROR("The session is already attached.");
			return	false;	
		}
	}

	session_list_.push_back(_session);

	if (!IsRunning())
	{
		if (!Start(100))
		{
			TRACE_ERROR("SNMP Master start initialization timeout!");	
		}
	}
	
	return	true;
}

bool	Master::Detach(Session* _session)
{
	for(std::list<Session*>::iterator it = session_list_.begin() ; it != session_list_.end() ; it++)
	{
		if ((*it) == _session)
		{
			session_list_.erase(it);

			if (session_list_.size() == 0)
			{
				Stop();	
			}
			return	true;	
		}
	}

	TRACE_ERROR("The session is not exist.");

	return	false;
}

bool	Master::Open(Session* _session, std::string const& _ip, std::string const& _community)
{
	snmp_session	session;
	snmp_session*	session_;
	char			ip[IP_LENGTH_MAX + 1];
	char			community[SNMP_COMMUNITY_LENGTH_MAX + 1];

	if (!_session->IsOpened())
	{
		locker_.Lock();

		snmp_sess_init(&session);

		session.version = SNMP_VERSION_2c;

		session.peername = ip;
		strncpy(session.peername, std::string(_ip).c_str(), IP_LENGTH_MAX);

		session.community = (uint8_t *)community;
		strncpy((char *)session.community, _community.c_str(), SNMP_COMMUNITY_LENGTH_MAX);
		session.community_len = _community.size();

		session.callback = AsyncResponse;
		session.callback_magic	= (void *)_session;

		_session->session_ = snmp_open(&session);

		locker_.Unlock();

		if (_session->session_ == NULL)
		{
			TRACE_ERROR("Failed to open session.");
			return	false;
		}

	}

	return	true;
}

bool	Master::Close(Session* _session)
{
	if (_session->session_ != NULL)
	{
		snmp_close(_session->session_);
		_session->session_ = NULL;

	}

	return	true;
}

uint32_t	Master::GetSessionCount()
{
	return	session_list_.size();
}

bool	Master::ReadValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, time_t& _time, std::string& _value)
{
	bool	ret_value = false;

	netsnmp_pdu*	response_pdu = NULL;
	netsnmp_pdu*	request_pdu = snmp_pdu_create(SNMP_MSG_GET);
	if (request_pdu != NULL)
	{
		request_pdu->time = _timeout;
		snmp_add_null_var(request_pdu, _oid.id, _oid.length);   

		locker_.Lock();
		int snmp_ret = snmp_synch_response(_session, request_pdu, &response_pdu);
		locker_.Unlock();

		if ((snmp_ret != STAT_SUCCESS) || (response_pdu->errstat != SNMP_ERR_NOERROR))
		{
			TRACE_ERROR("Failed to get SNMP!");
			if (response_pdu != NULL)
			{	
				TRACE_ERROR("Error : " << snmp_errstring(response_pdu->errstat) << "]!");
			}
		}
		else
		{ 
			if (response_pdu != NULL)
			{
				if (response_pdu->errstat == SNMP_ERR_NOERROR)
				{
					_time = Date::GetCurrent();
					ret_value = Master::Convert(response_pdu->variables, _value);
					if (!ret_value)
					{
						TRACE_ERROR("Failed to convert object[" << std::string(_oid) << "] value[" << _value <<"]");	
					}
				}
			}
		}

		if (response_pdu != NULL)
		{
			snmp_free_pdu(response_pdu);	
		}
	}
	else
	{
		TRACE_ERROR("Failed to create SNMP PDU!");	
	}

	return	ret_value;
}

bool	Master::SyncReadValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, time_t& _time, std::string& _value)
{
	bool	ret_value = false;

	netsnmp_pdu*	response_pdu = NULL;
	netsnmp_pdu*	request_pdu = snmp_pdu_create(SNMP_MSG_GET);
	if (request_pdu != NULL)
	{
		request_pdu->time = _timeout;
		snmp_add_null_var(request_pdu, _oid.id, _oid.length);   

		locker_.Lock();
		int snmp_ret = snmp_synch_response(_session, request_pdu, &response_pdu);
		locker_.Unlock();

		if ((snmp_ret != STAT_SUCCESS) || (response_pdu->errstat != SNMP_ERR_NOERROR))
		{
			TRACE_ERROR("Failed to get SNMP!");
			if (response_pdu != NULL)
			{	
				TRACE_ERROR("Error : " << snmp_errstring(response_pdu->errstat) << "]!");
			}
		}
		else
		{ 
			if (response_pdu != NULL)
			{
				if (response_pdu->errstat == SNMP_ERR_NOERROR)
				{
					_time = Date::GetCurrent();
					ret_value = Master::Convert(response_pdu->variables, _value);
					if (!ret_value)
					{
						TRACE_ERROR("Failed to convert object[" << std::string(_oid) << "] value[" << _value <<"]");	
					}
				}
			}
		}

		if (response_pdu != NULL)
		{
			snmp_free_pdu(response_pdu);	
		}
	}
	else
	{
		TRACE_ERROR("Failed to create SNMP PDU!");	
	}

	return	ret_value;
}

bool	Master::AsyncRequestReadValue(snmp_session* _session, OID const& _oid, uint32_t _timeout)
{
	bool	ret_value = false;

	netsnmp_pdu*	request_pdu = snmp_pdu_create(SNMP_MSG_GET);
	if (request_pdu != NULL)
	{
		request_pdu->time = _timeout;
		snmp_add_null_var(request_pdu, _oid.id, _oid.length);   

		locker_.Lock();
		int snmp_ret = snmp_send(_session, request_pdu);
		locker_.Unlock();

		if (snmp_ret == 0)
		{
			TRACE_ERROR("Failed to get SNMP[" << snmp_ret << "]!");
			snmp_free_pdu(request_pdu);
		}
		else
		{
			request_count_++;
			ret_value = true;	
		}
	}
	else
	{
		TRACE_ERROR("Failed to create SNMP PDU!");	
	}

	return	ret_value;
}

bool	Master::AsyncRequestWriteValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, uint32_t _value)
{
	bool	ret_value = false;

	netsnmp_pdu*	request_pdu = snmp_pdu_create(SNMP_MSG_SET);
	if (request_pdu != NULL)
	{
		std::ostringstream	oss;

		oss << _value;

		request_pdu->time = _timeout;
		snmp_add_var(request_pdu, _oid.id, _oid.length, 's', oss.str().c_str());   

		locker_.Lock();
		int snmp_ret = snmp_send(_session, request_pdu);
		locker_.Unlock();

		if (snmp_ret == 0)
		{
			TRACE_ERROR("Failed to get SNMP[" << snmp_ret << "]!");
			snmp_free_pdu(request_pdu);
		}
		else
		{
			request_count_++;
			ret_value = true;	
		}
	}
	else
	{
		TRACE_ERROR("Failed to create SNMP PDU!");	
	}

	return	ret_value;
}

bool	Master::AsyncRequestWriteValue(snmp_session* _session, OID const& _oid, uint32_t _timeout, std::string const& _value)
{
	bool	ret_value = false;

	netsnmp_pdu*	request_pdu = snmp_pdu_create(SNMP_MSG_SET);
	if (request_pdu != NULL)
	{
		request_pdu->time = _timeout;
		snmp_add_var(request_pdu, _oid.id, _oid.length, 's', _value.c_str());   

		locker_.Lock();
		int snmp_ret = snmp_send(_session, request_pdu);
		locker_.Unlock();

		if (snmp_ret == 0)
		{
			TRACE_ERROR("Failed to get SNMP[" << snmp_ret << "]!");
			snmp_free_pdu(request_pdu);
		}
		else
		{
			request_count_++;
			ret_value = true;	
		}
	}
	else
	{
		TRACE_ERROR("Failed to create SNMP PDU!");	
	}

	return	ret_value;
}

bool	Master::AsyncRequestReadValue(Session& _session, OID const& _oid, uint32_t _timeout)
{
	GetMessage	*message = new GetMessage(_session.session_, _oid, _timeout);

	Post(message);

	return	true;
}


bool	Master::AsyncRequestWriteValue(Session& _session, OID const& _oid, uint32_t _timeout, uint32_t _value)
{
	TRACE_ERROR("Not supported");
	return	false;
}

bool	Master::AsyncRequestWriteValue(Session& _session, OID const& _oid, uint32_t _timeout, std::string const& _value)
{
	SetMessage	*message = new SetMessage(_session.session_, _oid, _timeout, _value);

	Post(message);

	return	true;
}

/*
 * response handler
 */

int	Master::AsyncResponse(int operation, struct snmp_session *sp, int reqid, struct snmp_pdu *_pdu, void *_magic)
{
	Session *session= (Session*)_magic;

	TRACE_INFO2(NULL, "Async Response received!");
	session->master_->locker_.Lock();
	if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE) 
	{
		if (_pdu->errstat == SNMP_ERR_NOERROR)
		{
			session->success_ = Convert(_pdu->variables, session->value_);
			session->time_ = time_t(Date::GetCurrent());
			session->finished_.Unlock();
		}
	}
	else
	{
		session->success_ = false;
		session->finished_.Unlock();
	}

	if (session->master_->request_count_ > 0)
	{
		session->master_->request_count_--;
	}
	session->master_->locker_.Unlock();

	return 1;
}

bool	Master::Convert
(
	struct variable_list *_variable,
	std::string& _value
)
{
	if (_variable == NULL) 
	{   
		TRACE_ERROR2(NULL, "Failed to convert!");
		return	false;
	}

	switch(_variable->type)
	{   
	case    ASN_INTEGER:
		{   
			int	value = 0;
			switch (_variable->val_len)
			{   
				case    1:  value = (*(int8_t *)_variable->val.integer); break;
				case    2:  value = (*(int16_t *)_variable->val.integer); break;
				case    4:  value = (*(int32_t *)_variable->val.integer); break;
			}   

			_value = ToString(value);
		}   
		break;

	case    ASN_OCTET_STR:
		{   
			if (_variable->val_len != 0)
			{   
				char* buffer = new char [_variable->val_len + 1]; 

				memcpy(buffer, _variable->val.string, _variable->val_len);
				buffer[_variable->val_len] = 0;

				_value = buffer;
				delete buffer;
			}   
		}   
		break;
	
	default:
		TRACE_ERROR2(NULL, "Failed to convert[" << (int)_variable->type << "]");
		if (_variable->val_len != 0)
		{
			TRACE_INFO_DUMP2(NULL, (const char *)_variable->val.string, _variable->val_len);
		}
		return	false;
	}   

	return  true;
}

void	Master::Preprocess()
{
	ActiveObject::Preprocess();
}

void	Master::Process()
{
	if (request_count_ > 0)
	{
		int fds = 0, block = 1;
		fd_set fdset;
		struct timeval timeout;

		FD_ZERO(&fdset);
		snmp_select_info(&fds, &fdset, &timeout, &block);
		timeout.tv_sec = 0;
		timeout.tv_usec = 1000;
		fds = select(fds, &fdset, NULL, NULL, block ? NULL : &timeout);
		if (fds < 0) 
		{
//			TRACE_ERROR("Failed to select!");
		}
		else if (fds > 0)
		{
			snmp_read(&fdset);
		}
		else
		{
			snmp_timeout();
		}
	}

	ActiveObject::Process();
}


bool	Master::OnMessage(Message* _base)
{
	if(_base->GetType() == MSG_TYPE_SNMP_GET)
	{
		GetMessage	*message = dynamic_cast<GetMessage *>(_base);
		if (message != NULL)
		{
			AsyncRequestReadValue(message->session_, message->oid_, message->timeout_);
		}
	}
	else if(_base->GetType() == MSG_TYPE_SNMP_SET)
	{
		SetMessage	*message = dynamic_cast<SetMessage *>(_base);
		if (message != NULL)
		{
			AsyncRequestWriteValue(message->session_, message->oid_, message->timeout_, message->value_);
		}
	}
	else
	{
		return	ActiveObject::OnMessage(_base);
	}

	return	true;
}
