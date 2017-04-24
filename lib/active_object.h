#ifndef	ACTIVE_OBJECT_H_
#define	ACTIVE_OBJECT_H_

#include "object.h"
#include <thread>
#include "message_queue.h"

class	ActiveObject : public Object
{
public:
	ActiveObject();
	~ActiveObject();

	virtual	void	SetEnable(bool _enable);

			Stat	GetStat() const;

	virtual	void	Start();
	virtual	void	Stop();
	virtual	void	Run();

			bool	IsRunning();

	virtual	bool	GetProperties(Properties& _properties) const;
	virtual	bool	SetProperty(Property const& _property, bool create = false);
	virtual	void	Print(std::ostream& os) const;

	virtual	bool	Post(Message* _message);

	virtual	void	OnMessage(Message* _message);

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
