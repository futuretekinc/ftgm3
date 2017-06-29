#include "snmp_master.h"

using namespace SNMP;

static Master	_master;

Session::Session()
: master_(_master), session_(0) , finished_()
{
}

Session::~Session()
{
}

bool	Session::Open(std::string const& _ip, std::string const& _community)
{
	return	master_.Open(*this, _ip, _community);
}

bool	Session::Close()
{
	return	master_.Close(*this);
}

bool	Session::IsOpened()
{
	return	session_ != NULL;
}

bool	Session::SetTimeout(uint32_t _timeout)
{
	timeout_ = _timeout;

	return	true;
}

uint32_t	Session::GetTimeout()
{
	return	timeout_;
}

bool	Session::ReadValue(OID const& _oid, time_t& _time, std::string& _value)
{
	if (session_ == NULL)
	{
		return	false;
	}

	return	master_.ReadValue(session_, _oid, timeout_, _time, _value);
}

bool	Session::AsyncReadValue(OID const& _oid, time_t& _time, std::string& _value)
{
	bool	ret_value = false;

	if (session_ != NULL)
	{
		finished_.Lock();

		if (master_.AsyncRequestReadValue(session_, _oid, timeout_))
		{
			if (finished_.TryLock(timeout_))
			{
				_time = time_;
				_value = value_;

				ret_value = true;
			}
		}
		else
		{
			TRACE_ERROR("Failed to request read value!");	
		}
	}
	else
	{
		TRACE_ERROR("Failed to read value.");
	}

	finished_.Unlock();

	return	true;
}
