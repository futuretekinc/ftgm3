#include <iostream>
#include <cstddef>
//#include <chrono>
#include <unistd.h>
#include <sys/time.h>
#include "defined.h"
#include "trace.h"
#include "message_queue.h"
#include "time.h"
#include "object.h"

using namespace std;

MessageQueue::MessageQueue(Object& _object)
: parent_(_object), locker_()
{
	TRACE_INFO("Create Queue : " << queue_.size());
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

	TRACE_ENTRY;
	if (_message == NULL)
	{
		TRACE_ERROR("Woops! Pushed message is NULL!");	
	}
	else
	{
		queue_.push_back(_message);
	
		TRACE_INFO("Message Pushed : " << parent_.GetClassName() << " - " << _message);
		uint32_t	count = 0;
		for(std::list<Message*>::iterator it = queue_.begin() ; it != queue_.end() ; it++)
		{
			TRACE_INFO( "" << count << " : " << *it);
		}
	}
	
	TRACE_EXIT;
	locker_.Unlock();
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
	locker_.Lock();

	Message *message = NULL;

	if (queue_.size() != 0)
	{
		message = queue_.front();
		queue_.pop_front();

		TRACE_INFO("Message poped: " << parent_.GetClassName() << " - " << message);
		uint32_t	count = 0;
		for(std::list<Message*>::iterator it = queue_.begin() ; it != queue_.end() ; it++)
		{
			TRACE_INFO( "" << count << " : " << *it);
		}

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
