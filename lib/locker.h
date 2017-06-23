#ifndef	LOCKER_H_
#define	LOCKER_H_

#include <pthread.h>

class	Locker
{
public:
			Locker();

	bool	Lock();
	bool	Unlock();
	bool	IsLocked();

private:
	pthread_mutex_t	mutex_;
	bool	locked_;
};
#endif
