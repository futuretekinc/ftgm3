#include <iostream>
#include <cstddef>
#include <chrono>
#include <unistd.h>
#include <sys/time.h>
#include "defined.h"
#include "message_queue.h"
#include "time.h"

using namespace std;

MessageQueue::MessageQueue()
{
	lock_.lock();
}

MessageQueue::~MessageQueue()
{
	while(queue_.size() > 0)
	{
		Message *message = queue_.front();
		queue_.pop();

		delete message;
	}
}

void MessageQueue::Push
(
	Message* _message
)
{
	queue_.push(_message);
}

Message* MessageQueue::Front()
{
	return	queue_.front();
}

Message* MessageQueue::Pop
(
	bool _do_release
)
{
	Message *message = queue_.front();

	queue_.pop();	

	if (message != NULL)
	{
		if (_do_release)
		{
			delete message;
			message = NULL;
		}
	}

	return	message;
}

uint32_t	MessageQueue::Count()
{
	return	queue_.size();
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
