#include "thread.h"
#include "trace.h"

Thread::Thread()
: thread_(0), func_(0)
{
}

Thread::~Thread()
{
}

bool	Thread::joinable()
{
	return	(thread_ != 0);
}

bool	Thread::join()
{
	int	status;

	pthread_join(thread_, NULL);
	thread_ = 0;

	return	true;
}

bool	Thread::create(void *(*_func)(void*), void *_args)
{
	if (pthread_create(&thread_, NULL, _func, _args) < 0)
	{
		TRACE_ERROR("Failed to create pthread!");
		return	false;	
	}

	func_ = _func;
	args_ = _args;

	return	true;
}
