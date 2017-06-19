#ifndef	THREAD_H_
#define	THREAD_H_

#include <pthread.h>

class	Thread
{
public:
	Thread();
	~Thread();

	bool	join();
	bool	joinable();
	bool	create(void *(*_func)(void*), void *_args);

protected:
	pthread_t	thread_;
	void*		(*func_)(void*);
	void*		args_;
};

#endif
