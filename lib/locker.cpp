#include "locker.h"

Locker::Locker()
{
	locked_ = false;
}

bool	Locker::Lock()
{
	mutex_.lock();

	locked_ = true;

	return	true;
}

bool	Locker::Unlock()
{
	mutex_.unlock();

	locked_ = false;

	return	true;
}

bool	Locker::IsLocked()
{
	return	locked_;
}
