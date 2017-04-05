#include <unistd.h>
#include "defined.h"
#include "active_object.h"
#include "timer.h"

using namespace	std;

ActiveObject::ActiveObject()
: loop_interval_(ACTIVE_OBJECT_LOOP_INTERVAL), thread_(0)
{
}

void	ActiveObject::Start()
{
	if (thread_ == 0)
	{
		thread_ = 
	}
}

void	ActiveObject::Stop()
{
}

bool	ActiveObject::IsRunning()
{
}

void	ActiveObject::ThreadMain(ActiveObject* _object)
{
	Timer	loop_timer;
	
	_object->stop_ = false;
	_object->running_ = true;


	while(_object->running_)
	{
		loop_timer += _object->loop_interval_;

		if (_object->stop_)	
		{
			_object->running_ = false;	
		}

		usleep(loop_timer.RemainTime());
	}
}
