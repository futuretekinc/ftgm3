#ifndef	PROCESS_OBJECT_H_
#define	PROCESS_OBJECT_H_

#include <string>
#include "active_object.h"

class	ProcessObject : public ActiveObject
{
public:
	ProcessObject();
	ProcessObject(std::string const& _id);
	~ProcessObject();

};

#endif
