#include "snmp_master.h"

using namespace SNMP;

static Master*	_master = NULL;

Session::Session()
: session_(0) , finished_(), locker_()
{
	if (_master == NULL)
	{
		_master = new Master;	
	}

	master_ = _master;

	master_->Attach(this);
}

Session::~Session()
{
	Close();

	master_->Detach(this);
}

bool	Session::Open(std::string const& _ip, std::string const& _community)
{
	return	master_->Open(this, _ip, _community);
}

bool	Session::Close()
{
	return	master_->Close(this);
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
	bool	ret_value = false;

	if (session_ == NULL)
	{
		return	false;
	}

	locker_.Lock();

	ret_value = master_->ReadValue(session_, _oid, timeout_, _time, _value);

	locker_.Unlock();

	return	ret_value;
}

bool	Session::AsyncReadValue(OID const& _oid, time_t& _time, std::string& _value)
{
	bool	ret_value = false;

	//locker_.Lock();
	if (session_ != NULL)
	{
		finished_.Lock();
			
		if (master_->AsyncRequestReadValue(*this, _oid, timeout_))
		{
			if (finished_.TryLock(timeout_ * 1000))
			{
				if (success_)
				{
					_time = time_;
					_value = value_;

					ret_value = true;
				}
				else
				{
					TRACE_ERROR("Failed to request read value!");	
				}
			}
			else
			{
				TRACE_ERROR("Tiemout[" << timeout_ << "]! Failed to request read value!");	
			}
		}
		else
		{
			TRACE_ERROR("Failed to request read value!");	
		}

		finished_.Unlock();
	}
	else
	{
		TRACE_ERROR("Failed to read value.");
	}

	//locker_.Unlock();

	return	ret_value;
}

bool	Session::AsyncWriteValue(OID const& _oid, std::string const& _value)
{
	bool	ret_value = false;

	TRACE_ENTRY;
	//locker_.Lock();
	if (session_ != NULL)
	{
		finished_.Lock();
			
		if (master_->AsyncRequestWriteValue(*this, _oid, timeout_, _value))
		{
			if (finished_.TryLock(timeout_ * 1000))
			{
				if (success_)
				{
					ret_value = true;
				}
				else
				{
					TRACE_ERROR("Failed to request read value!");	
				}
			}
			else
			{
				TRACE_ERROR("Tiemout[" << timeout_ << "]! Failed to request read value!");	
			}
		}
		else
		{
			TRACE_ERROR("Failed to request read value!");	
		}

		finished_.Unlock();
	}
	else
	{
		TRACE_ERROR("Failed to read value.");
	}

	//locker_.Unlock();

	return	ret_value;
}
