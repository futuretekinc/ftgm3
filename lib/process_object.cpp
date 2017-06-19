#include <fstream>
#include "defined.h"
#include "json.h"
#include "exception.h"
#include "process_object.h"


ProcessObject::ProcessObject()
:  ActiveObject()
{
}

ProcessObject::ProcessObject(std::string const& _id)
: ActiveObject(_id)
{
}

ProcessObject::~ProcessObject()
{
}


