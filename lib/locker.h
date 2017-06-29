#ifndef	LOCKER_H_
#define	LOCKER_H_

#include <pthread.h>
#include <stdint.h>

class	Locker
{
public:
			Locker();

	bool	Lock();
	bool	TryLock(uint32_t _timeout = 0);
	bool	Unlock();
	bool	IsLocked();

private:
	pthread_mutex_t	mutex_;
	bool	locked_;
};
#endif
