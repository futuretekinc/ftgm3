#ifndef	ACTIVE_OBJECT_H_
#define	ACTIVE_OBJECT_H_

#include "thread.h"
#include "object.h"
#include "message_queue.h"
#include "locker.h"

class	ActiveObject : public Object
{
public:
	ActiveObject();
	ActiveObject(std::string const& _id);
	~ActiveObject();

					operator JSONNode() const;

	virtual	bool	SetEnable(bool _enable);

			Stat	GetStat() const;
			
			bool	SetLoopInterval(uint32_t _interval);
			bool	SetLoopInterval(std::string const& _interval, bool _check = false);

	virtual	bool	Start(uint32_t _wait_for_init_time);	// ms
	virtual	bool	Stop(bool _wait = true);
	virtual	bool	Run(uint32_t _timeout);

	virtual	bool	IsRunning();

	virtual	bool	GetProperties(JSONNode& _properties, Fields const& _fields = PROPERTY_ALL);

	virtual	bool	SetProperty(JSONNode const& _property, bool _check =false);

	virtual	bool	Post(Message* _message);

	virtual	bool	OnMessage(Message* _message);

	static	bool	IsIncludeIn(Object *_object);
protected:

	virtual	void	Preprocess();
	virtual	void	Process();
	virtual	void	Postprocess();

	Thread			thread_;
	bool			stop_;
	double			loop_interval_;
	MessageQueue	message_queue_;
	Locker			locker_;
	Locker			waiting_for_initialization_;
	Locker			activated_;

	static	void*	ThreadMain(void* _object);
};

#endif
