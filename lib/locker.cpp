#include <sys/time.h>
#include <unistd.h>
#include "locker.h"

Locker::Locker()
{
	pthread_mutex_init(&mutex_, NULL);	
	locked_= false;
}

bool	Locker::Lock()
{
	pthread_mutex_lock(&mutex_);

	locked_ = true;

	return	true;
}

bool	Locker::TryLock(uint32_t _timeout)
{
	timeval	time;
	uint64_t	start_time;
	uint64_t	current_time;

	gettimeofday(&time, 0);
	start_time = time.tv_sec * (uint64_t)1000000 + time.tv_usec;

	while(1)
	{
		if (pthread_mutex_trylock(&mutex_) == 0)
		{
			locked_ = true;
			return	true;	
		}

		gettimeofday(&time, 0);
		current_time = time.tv_sec * (uint64_t)1000000 + time.tv_usec;

		if (_timeout < (current_time - start_time) / 1000)
		{
			break;
		}

		usleep(1000);
	}


	return	false;
}

bool	Locker::Unlock()
{
	if (locked_)
	{
		locked_ = false;
		pthread_mutex_unlock(&mutex_);
	}

	return	true;
}

bool	Locker::IsLocked()
{
	return	locked_;
}
