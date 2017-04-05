#ifndef	OBJECT_H_
#define	OBJECT_H_

#include "value.h"
#include "time2.h"

class	Object
{
public:
	Object();
	Object(const ValueID& _id);
	virtual	~Object() = 0;  

protected:
	ValueID		id_;
	ValueName	name_;	
	Date		date_;
	bool		enable_;
};

#endif
