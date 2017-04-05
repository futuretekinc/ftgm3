#ifndef	DEVICE_H_
#define	DEVICE_H_

#include "active_object.h"

class	Device : public ActiveObject
{
public:
	Device();
	
protected:
	ValueID		parent_id_;
	Time		live_check_interval_;
};

#endif
