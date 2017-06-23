#ifndef	ACTIVE_OBJECT_H_
#define	ACTIVE_OBJECT_H_

#include "thread.h"
#include "object.h"
#include "message_queue.h"

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

	virtual	bool	Start(uint32_t _wait_for_init_time = 1000);	// ms
	virtual	bool	Stop(bool _wait = true);
	virtual	void	Run();

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
	uint32_t		loop_interval_;
	MessageQueue	message_queue_;

	static	void*	ThreadMain(void* _object);
};

#endif
