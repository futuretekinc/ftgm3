#include <iostream>
#include <cstddef>
#include <unistd.h>
#include <sys/time.h>
#include "defined.h"
#include "trace.h"
#include "message_queue.h"
#include "time.h"
#include "object.h"

using namespace std;

MessageQueue::MessageQueue(Object& _object)
: parent_(_object), locker_(), queue_()
{
}

MessageQueue::~MessageQueue()
{
	while(queue_.size() > 0)
	{
		Message *message = queue_.front();
		queue_.pop_front();

		delete message;
	}
}

void MessageQueue::Push
(
	Message* _message
)
{
	locker_.Lock();

	if (_message == NULL)
	{
		TRACE_ERROR("Woops! Pushed message is NULL!");	
	}
	else
	{
		queue_.push_back(_message);
	}
	
	locker_.Unlock();
}

Message* MessageQueue::Front()
{
	locker_.Lock();
	Message*	front = queue_.front();
	locker_.Unlock();

	return	front;
}

Message* MessageQueue::Pop
(
	bool _do_release
)
{
	locker_.Lock();

	Message *message = NULL;

	if (queue_.size() != 0)
	{
		message = queue_.front();
		queue_.pop_front();

		if (message != NULL)
		{
			if (_do_release)
			{
				delete message;
				message = NULL;
			}
		}
		else
		{
			TRACE_ERROR("Queue element is NULL!!!");	
			TRACE_ERROR("Queue Size : " << queue_.size());
		}
	}

	locker_.Unlock();
	return	message;
}

uint32_t	MessageQueue::Count()
{
	locker_.Lock();
	uint32_t	size = queue_.size();
	locker_.Unlock();

	return	size;
}

bool MessageQueue::TimedWait
(
	int	_milliseconds
)
{
	struct timeval	timer;
	struct timeval	expired_time;
	struct timeval	timeout;

	timeout.tv_sec = _milliseconds / 1000;
	timeout.tv_usec = _milliseconds % 1000 * 1000;

	gettimeofday(&timer, NULL);
	timeradd(&timer, &timeout, &expired_time);

	while(timercmp(&timer, &expired_time, <))
	{
		if (queue_.size() != 0)
		{
			return	true;	
		}

		usleep(TIME_MILLISECOND);	
		gettimeofday(&timer, NULL);
	}

	return	false;
}
