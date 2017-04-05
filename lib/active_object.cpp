#include <unistd.h>
#include "defined.h"
#include "active_object.h"
#include "timer.h"

ActiveObject::ActiveObject()
: loop_interval_(ACTIVE_OBJECT_LOOP_INTERVAL)
{
}

void	ActiveObject::Start()
{
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
