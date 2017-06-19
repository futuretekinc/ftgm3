#include "endpoint.h"

Endpoint::ValueMap::ValueMap(uint32_t _limit)
:	limit_(_limit)
{
}

Endpoint::ValueMap::~ValueMap()
{
}

bool Endpoint::ValueMap::Add(time_t _time, std::string const& _value)
{
	if (size() >= limit_)
	{
		erase(begin());
	}

	insert(std::make_pair(_time, _value));

	return	true;
}
