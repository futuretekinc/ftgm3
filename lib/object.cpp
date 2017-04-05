#include "object.h"

Object::Object()
{
}


Object::Object(const ValueID& _id)
:	id_(_id), enable_(false)
{
	name_ = id_.ToString();
	date_ = Date::GetCurrentDate();
}

