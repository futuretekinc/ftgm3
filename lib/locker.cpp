#include "locker.h"

Locker::Locker()
{
	mutex_ = PTHREAD_MUTEX_INITIALIZER;
	locked_= false;
}

bool	Locker::Lock()
{
	pthread_mutex_lock(&mutex_);

	locked_ = true;

	return	true;
}

bool	Locker::Unlock()
{
	locked_ = false;

	pthread_mutex_unlock(&mutex_);

	return	true;
}

bool	Locker::IsLocked()
{
	return	locked_;
}
