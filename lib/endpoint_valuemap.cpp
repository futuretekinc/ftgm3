#include "endpoint.h"

Endpoint::ValueMap::ValueMap(uint32_t _limit)
:	limit_(_limit)
{
	for(auto it = begin() ; it != end() ; it++)
	{
		delete it->second;	
	}
}

Endpoint::ValueMap::~ValueMap()
{
}

bool Endpoint::ValueMap::Add(Date const& _date, Value const* _value)
{
	if (_date.IsValid() && (size() < limit_))
	{
		insert(std::make_pair(_date, _value->Duplicate()));
		return	true;
	}

	return	false;
}

