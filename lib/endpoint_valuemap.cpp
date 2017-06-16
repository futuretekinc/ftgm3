#include "endpoint.h"

Endpoint::ValueMap::ValueMap(uint32_t _limit)
:	limit_(_limit)
{
}

Endpoint::ValueMap::~ValueMap()
{
}

bool Endpoint::ValueMap::Add(Date const& _date, std::string const& _value)
{
	if (_date.IsValid())
	{ 
		if (size() >= limit_)
		{
			erase(begin());
		}

		insert(std::make_pair(_date, _value));
		return	true;
	}

	return	false;
}

Endpoint::ValueList::ValueList(uint32_t _limit)
:	limit_(_limit)
{
}

Endpoint::ValueList::~ValueList()
{
}

bool Endpoint::ValueList::Add(std::string const& _value)
{
	if (size() < limit_)
	{
		push_back(_value);
		return	true;
	}

	return	false;
}

