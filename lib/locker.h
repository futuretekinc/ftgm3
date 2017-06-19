#ifndef	LOCKER_H_
#define	LOCKER_H_


class	Locker
{
public:
			Locker();

	bool	Lock();
	bool	Unlock();
	bool	IsLocked();

private:
	bool		locked_;
};
#endif
