#ifndef	ACTIVE_OBJECT_H_
#define	ACTIVE_OBJECT_H_

#include <thread>
#include "object.h"
#include "message_queue.h"

class	ActiveObject : public Object
{
public:
	ActiveObject();
	ActiveObject(ValueID const& _id);
	~ActiveObject();

			bool	Load(JSONNode const& _json);
					operator JSONNode() const;

	virtual	bool	SetEnable(bool _enable);

			Stat	GetStat() const;
			
			bool	SetLoopInterval(Time const& _interval);

	virtual	bool	Start(uint32_t _wait_for_init_time = 1000);	// ms
	virtual	bool	Stop(bool _wait = false);
	virtual	void	Run();

	virtual	bool	IsRunning();

	virtual	bool	GetProperties(Properties& _properties, Properties::Fields const& _fields = PROPERTY_ALL);
	virtual	bool	GetProperties(JSONNode& _properties, Properties::Fields const& _fields = PROPERTY_ALL);

			bool	SetProperty(Property const& _property, Properties::Fields const& _fields = PROPERTY_ALL);

	virtual	bool	Post(Message* _message);

	virtual	bool	OnMessage(Message* _message);

	static	bool	IsIncludeIn(Object *_object);
protected:

	virtual	void	Preprocess();
	virtual	void	Process();
	virtual	void	Postprocess();

	std::thread		thread_;	
	bool			stop_;
	Time			loop_interval_;
	MessageQueue	message_queue_;

	static	void	ThreadMain(ActiveObject* _object);
};

#endif
