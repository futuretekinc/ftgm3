#ifndef	ACTIVE_OBJECT_H_
#define	ACTIVE_OBJECT_H_

#include "object.h"
#include <thread>

class	ActiveObject : public Object
{
public:
	ActiveObject();
	
			void	Start();
			void	Stop();

			bool	IsRunning();

	virtual	bool	GetProperties(Properties& _properties);
	virtual	bool	SetProperty(Property const& _property, bool create = false);
	virtual	void	Print(std::ostream& os) const;

protected:
	virtual	void	Preprocess();
	virtual	void	Process();
	virtual	void	Postprocess();

	std::thread		thread_;	
	bool			stop_;
	Time			loop_interval_;

	static	void	ThreadMain(ActiveObject* _object);
};

#endif
