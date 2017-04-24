#ifndef	LOCKER_H_
#define	LOCKER_H_

#include <mutex>

class	Locker
{
public:
			Locker();

	bool	Lock();
	bool	Unlock();
	bool	IsLocked();

private:
	bool		locked_;
	std::mutex	mutex_;
};
#endif
