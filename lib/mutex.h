#ifndef	MUTEX_H_
#define	MUTEX_H_

#include <pthread.h>

class	Mutex
{
public:
	Mutex();
	~Mutex();
	
	void	lock();
	void	unlock();

protected:
	pthread_mutex_t	mutex_;
};

#endif
