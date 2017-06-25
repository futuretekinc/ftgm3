#ifndef	MESSAGE_QUEUE_H_
#define	MESSAGE_QUEUE_H_

#include <queue>
#include <list>
#include "locker.h"
#include "message.h"

class	MessageQueue
{
public:
	MessageQueue(Object& _parent);
	~MessageQueue();

	void		Push(Message *_message);
	Message*	Pop(bool _do_relese =false);
	Message*	Front();
	uint32_t	Count();

	bool TimedWait(int _milliseconds);

private:
	Object&					parent_;
	std::list<Message *>	queue_;
	Locker					locker_;
};

#endif
