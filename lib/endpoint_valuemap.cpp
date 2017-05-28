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
	if (_date.IsValid())
	{ 
		if (size() >= limit_)
		{
			erase(begin());
		}

		insert(std::make_pair(_date, _value->Duplicate()));
		return	true;
	}

	return	false;
}

Endpoint::ValueList::ValueList(uint32_t _limit)
:	limit_(_limit)
{
	for(auto it = begin() ; it != end() ; it++)
	{
		delete (*it);	
	}
}

Endpoint::ValueList::~ValueList()
{
}

bool Endpoint::ValueList::Add(Value const* _value)
{
	if (size() < limit_)
	{
		push_back(_value->Duplicate());
		return	true;
	}

	return	false;
}

