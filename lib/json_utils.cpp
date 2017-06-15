#include "defined.h"
#include "utils.h"

std::string	JSONNodeGetID(JSONNode const& _node)
{
	auto it = _node.find(TITLE_NAME_ID);

	if (it != _node.end())
	{
		return	it->as_string();
	}

	return	std::string("");
}

time_t		JSONNodeGetStartTime(JSONNode const& _node, time_t _default)
{
	auto field = _node.find(TITLE_NAME_START_TIME);
	if (field != _node.end())
	{
		if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
		{
			return	time_t(field->as_int());
		}
	}
	
	return	_default;
}

time_t		JSONNodeGetEndTime(JSONNode const& _node, time_t _default)
{
	auto field = _node.find(TITLE_NAME_END_TIME);
	if (field != _node.end())
	{
		if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
		{
			return	time_t(field->as_int());
		}
	}
	
	return	_default;
}

time_t		JSONNodeGetTimeOfExpire(JSONNode const& _node)
{
	auto field = _node.find(TITLE_NAME_TIME_OF_EXPIRE);
	if (field != _node.end())
	{
		if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
		{
			return	time_t(field->as_int());
		}
	}

	throw ObjectNotFound(TITLE_NAME_TIME_OF_EXPIRE);
}

std::string	JSONNodeGetValue(JSONNode const& _node)
{
	auto field = _node.find(TITLE_NAME_VALUE);
	if (field != _node.end())
	{
		return	field->as_string();
	}

	throw ObjectNotFound(TITLE_NAME_TIME_OF_EXPIRE);
}

std::string	JSONNodeGetMsgID(JSONNode const& _node)
{
	auto it = _node.find(TITLE_NAME_MSG_ID);

	if (it != _node.end())
	{
		return	it->as_string();
	}

	throw ObjectNotFound(TITLE_NAME_MSG_ID);
}

std::string	JSONNodeGetMsgID(JSONNode const& _node, std::string _default)
{
	auto it = _node.find(TITLE_NAME_MSG_ID);

	if (it != _node.end())
	{
		return	it->as_string();
	}

	return	_default;
}
