#ifndef	ACTIVE_OBJECT_H_
#define	ACTIVE_OBJECT_H_

#include "object.h"
#include <thread>

class	ActiveObject : public Object
{
public:
	ActiveObject();
	
	void	Start();
	void	Stop();

	bool	IsRunning();

protected:
	std::thread*	thread_id_;	
	bool			running_;
	bool			stop_;
	Time			loop_interval_;

	static	void	ThreadMain(ActiveObject* _object);
};

#endif
