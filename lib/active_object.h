#ifndef	ACTIVE_OBJECT_H_
#define	ACTIVE_OBJECT_H_

#include "object.h"
#include <thread>
#include "message_queue.h"

class	ActiveObject : public Object
{
public:
	ActiveObject(Object* _parent = NULL);
	ActiveObject(ValueID const& _id, Object* _parent = NULL);
	~ActiveObject();

			bool	Load(JSONNode const& _json);
					operator JSONNode() const;

	virtual	bool	SetEnable(bool _enable, bool _store = true);

			Stat	GetStat() const;
			
			bool	SetLoopInterval(Time const& _interval, bool _store = true);

	virtual	bool	Start();
	virtual	bool	Stop(bool _wait = false);
	virtual	void	Run();

	virtual	bool	IsRunning();

	virtual	bool	GetProperties(Properties& _properties) const;

	virtual	bool	Post(Message* _message);

	virtual	void	OnMessage(Message* _message);

protected:
	virtual	bool	SetPropertyInternal(Property const& _property, bool create = false);

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
