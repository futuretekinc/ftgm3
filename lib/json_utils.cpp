#include "defined.h"
#include "exception.h"
#include "utils.h"

std::string	JSONNodeGetString(JSONNode const& _node, std::string const& field)
{
	auto it = _node.find(field);

	if (it != _node.end())
	{
		return	it->as_string();
	}

	throw ObjectNotFound(field);
}

std::string	JSONNodeGetString(JSONNode const& _node, std::string const& field, std::string const& _default)
{
	auto it = _node.find(field);

	if (it != _node.end())
	{
		return	it->as_string();
	}

	return	_default;
}

std::string	JSONNodeGetID(JSONNode const& _node)
{
	return	JSONNodeGetString(_node, TITLE_NAME_ID);
}

std::string	JSONNodeGetType(JSONNode const& _node)
{
	return	JSONNodeGetString(_node, TITLE_NAME_TYPE);
}

std::string	JSONNodeGetName(JSONNode const& _node)
{
	return	JSONNodeGetString(_node, TITLE_NAME_NAME);
}

std::string	JSONNodeGetName(JSONNode const& _node, std::string const& _default)
{
	return	JSONNodeGetString(_node, TITLE_NAME_NAME, _default);
}

time_t		JSONNodeGetTime(JSONNode const& _node)
{
	auto field = _node.find(TITLE_NAME_TIME);
	if (field != _node.end())
	{
		if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
		{
			return	time_t(field->as_int());
		}
	}
	
	throw ObjectNotFound(TITLE_NAME_TIME);
}

time_t		JSONNodeGetTime(JSONNode const& _node, time_t _default)
{
	auto field = _node.find(TITLE_NAME_TIME);
	if (field != _node.end())
	{
		if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
		{
			return	time_t(field->as_int());
		}
	}
	
	return	_default;
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
	return	JSONNodeGetString(_node, TITLE_NAME_VALUE);
}

std::string	JSONNodeGetMsgID(JSONNode const& _node)
{
	return	JSONNodeGetString(_node, TITLE_NAME_MSG_ID);
}

std::string	JSONNodeGetMsgID(JSONNode const& _node, std::string _default)
{
	return	JSONNodeGetString(_node, TITLE_NAME_MSG_ID, _default);
}

uint32_t	JSONNodeGetCount(JSONNode const& _node, uint32_t _default)
{
	auto field = _node.find(TITLE_NAME_COUNT);
	if (field != _node.end())
	{
		if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
		{
			return	time_t(field->as_int());
		}
	}

	throw _default;
}

JSONNode	JSONNodeGetTraceNode(JSONNode const& _node)
{
	auto field = _node.find(TITLE_NAME_TRACE);
	if (field != _node.end())
	{
		return	*field;
	}

	throw ObjectNotFound(TITLE_NAME_TRACE);
}

JSONNode	JSONNodeGetValueNode(JSONNode const& _node)
{
	auto field = _node.find(TITLE_NAME_VALUE);
	if (field != _node.end())
	{
		return	*field;
	}

	throw ObjectNotFound(TITLE_NAME_VALUE);
}

bool		JSONNodeIsExistValue(JSONNode const& _node)
{
	auto field = _node.find(TITLE_NAME_VALUE);

	return	(field != _node.end());
}
