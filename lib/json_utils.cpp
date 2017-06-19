#include <fstream>
#include "defined.h"
#include "exception.h"
#include "utils.h"

std::string	JSONNodeGetString(JSONNode const& _node, std::string const& field)
{
	JSONNode::const_iterator it = _node.find(field);

	if (it != _node.end())
	{
		return	it->as_string();
	}

	throw ObjectNotFound(field);
}

std::string	JSONNodeGetString(JSONNode const& _node, std::string const& field, std::string const& _default)
{
	JSONNode::const_iterator it = _node.find(field);

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
	JSONNode::const_iterator field = _node.find(TITLE_NAME_TIME);
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
	JSONNode::const_iterator field = _node.find(TITLE_NAME_TIME);
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
	JSONNode::const_iterator field = _node.find(TITLE_NAME_START_TIME);
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
	JSONNode::const_iterator field = _node.find(TITLE_NAME_END_TIME);
	if (field != _node.end())
	{
		if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
		{
			return	time_t(field->as_int());
		}
	}
	
	return	_default;
}

time_t		JSONNodeGetLastTime(JSONNode const& _node)
{
	JSONNode::const_iterator field = _node.find(TITLE_NAME_LAST_TIME);
	if (field != _node.end())
	{
		if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
		{
			return	time_t(field->as_int());
		}
	}
	
	throw ObjectNotFound(TITLE_NAME_LAST_TIME);
}

time_t		JSONNodeGetTimeOfExpire(JSONNode const& _node)
{
	JSONNode::const_iterator field = _node.find(TITLE_NAME_TIME_OF_EXPIRE);
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

std::string	JSONNodeGetMsgType(JSONNode const& _node)
{
	return	JSONNodeGetString(_node, TITLE_NAME_MSG_TYPE);
}

uint32_t	JSONNodeGetCount(JSONNode const& _node, uint32_t _default)
{
	JSONNode::const_iterator field = _node.find(TITLE_NAME_COUNT);
	if (field != _node.end())
	{
		if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
		{
			return	time_t(field->as_int());
		}
	}

	throw _default;
}

JSONNode	JSONNodeGetNode(JSONNode const& _node, std::string const& _name)
{
	JSONNode::const_iterator field = _node.find(_name);
	if (field != _node.end())
	{
		return	*field;
	}

	throw ObjectNotFound(TITLE_NAME_TRACE);
}

JSONNode	JSONNodeGetTraceNode(JSONNode const& _node)
{
	JSONNode::const_iterator field = _node.find(TITLE_NAME_TRACE);
	if (field != _node.end())
	{
		return	*field;
	}

	throw ObjectNotFound(TITLE_NAME_TRACE);
}

JSONNode	JSONNodeGetValueNode(JSONNode const& _node)
{
	JSONNode::const_iterator field = _node.find(TITLE_NAME_VALUE);
	if (field != _node.end())
	{
		return	*field;
	}

	throw ObjectNotFound(TITLE_NAME_VALUE);
}

bool		JSONNodeIsExistValue(JSONNode const& _node)
{
	JSONNode::const_iterator field = _node.find(TITLE_NAME_VALUE);

	return	(field != _node.end());
}

void	JSONNodeUpdate(JSONNode& _node, std::string const& _name, std::string const& _value)
{
	JSONNode::iterator it = _node.find(_name);
	if (it != _node.end())
	{
		(*it) = _value;
	}
	else
	{
		_node.push_back(JSONNode(_name, _value));
	}
}

void	JSONNodeUpdate(JSONNode& _node, std::string const& _name, uint32_t _value)
{
	JSONNode::iterator it = _node.find(_name);
	if (it != _node.end())
	{
		(*it) = _value;
	}
	else
	{
		_node.push_back(JSONNode(_name, _value));
	}
}


JSONNode	JSONNodeLoadFromFile(std::string const& _file_name)
{
	std::fstream	fs(_file_name.c_str(), std::fstream::in);
	if (!fs.is_open())
	{
		THROW_INVALID_ARGUMENT("The " << _file_name << " cat not open!");
	}

	fs.seekg (0, fs.end);
	int length = fs.tellg();
	fs.seekg (0, fs.beg);

	char * buffer = new char [length + 1];
	fs.read(buffer, length);
	buffer[length] = 0;
	fs.close();

	if (!libjson::is_valid(buffer))
	{
		delete buffer;
		THROW_INVALID_ARGUMENT("The " << _file_name << " is invalid!");
	}

	JSONNode  node = libjson::parse(buffer);
	delete buffer;

	return	node;
}
