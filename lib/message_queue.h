#ifndef	MESSAGE_QUEUE_H_
#define	MESSAGE_QUEUE_H_

#include <queue>
//#include <mutex>
#include "message.h"

class	MessageQueue
{
public:
	MessageQueue();
	~MessageQueue();

	void		Push(Message *_message);
	Message*	Pop(bool _do_relese =false);
	Message*	Front();
	uint32_t	Count();

	bool TimedWait(int _milliseconds);

private:
	std::queue<Message *>	queue_;
//	std::timed_mutex		lock_;
};

#endif
