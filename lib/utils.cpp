#include "defined.h"
#include "utils.h"

bool	GetValue(JSONNode const& _node, bool& _value)
{
	if (_node.type() == JSON_STRING)
	{
		if (_node.as_string() == "yes")	
		{
			_value = true;				
		}
		else if (_node.as_string() == "no")	
		{
			_value = false;				
		}
		else
		{
			return	false;	
		}
	}
	else if (_node.type() == JSON_NUMBER)
	{
		_value = (_node.as_int() != 0)	;
	}
	else
	{
		return	false;	
	}

	return	true;
}

bool	GetValue(JSONNode& _node, std::string& _value)
{
	if (_node.type() == JSON_STRING)
	{
		_value = _node.as_string();
		return	true;
	}

	return	false;
}

bool	GetValue(JSONNode const& _node, std::string const& _name, bool& _value, bool _empty_allow)
{
	const JSONNode*	item = NULL;

	if (_node.type() == JSON_NODE)
	{
		auto field = _node.find(_name);
		if (field == _node.end())
		{
			return	_empty_allow;	
		}

		item = &(*field);
	}

	if (item->type() == JSON_STRING)
	{
		if (item->as_string() == "yes")	
		{
			_value = true;				
		}
		else if (item->as_string() == "no")	
		{
			_value = false;				
		}
		else
		{
			return	false;	
		}
	}
	else if (item->type() == JSON_NUMBER)
	{
		_value = (item->as_int() != 0)	;
	}
	else
	{
		return	false;	
	}

	return	true;
}

bool	GetValue(JSONNode const& _node, std::string const& _name, std::string& _value, bool _empty_allow)
{
	bool	ret_value = false;

	if (_node.type() == JSON_NODE)
	{
		auto field = _node.find(_name);
		if (field != _node.end())
		{
			if (field->type() == JSON_STRING)
			{
				_value = field->as_string();
				ret_value = true;
			}
		}
		else if (_empty_allow)
		{
			ret_value = _empty_allow;			
		}
	}

	return	ret_value;
}

bool	GetValue(JSONNode const& _node, std::string const& _name, uint32_t& _value, bool _empty_allow)
{
	bool	ret_value = false;

	if (_node.type() == JSON_NODE)
	{
		auto field = _node.find(_name);
		if (field != _node.end())
		{
			if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
			{
				_value = field->as_int();
				ret_value = true;
			}
		}
		else 
		{
			ret_value = _empty_allow;			
		}
	}

	return	ret_value;
}

bool	GetValue(JSONNode const& _node, std::string const& _name, ValueFloat& _value, bool _empty_allow)
{
	bool	ret_value = false;

	if (_node.type() == JSON_NODE)
	{
		auto field = _node.find(_name);
		if (field != _node.end())
		{
			if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
			{
				_value = field->as_float();
				ret_value = true;
			}
		}
		else 
		{
			ret_value = _empty_allow;			
		}
	}

	return	ret_value;
}

bool	GetValue(JSONNode const& _node, std::string const& _name, float& _value, bool _empty_allow)
{
	bool	ret_value = false;

	if (_node.type() == JSON_NODE)
	{
		auto field = _node.find(_name);
		if (field != _node.end())
		{
			if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
			{
				_value = field->as_float();
				ret_value = true;
			}
		}
		else 
		{
			ret_value = _empty_allow;			
		}
	}

	return	ret_value;
}

bool	GetValue(JSONNode const& _node, std::string const& _name, time_t& _value, bool _empty_allow)
{
	bool	ret_value = false;

	if (_node.type() == JSON_NODE)
	{
		auto field = _node.find(_name);
		if (field != _node.end())
		{
			if ((field->type() == JSON_NUMBER) || (field->type() == JSON_STRING))
			{
				_value = field->as_int();
				ret_value = true;
			}
		}
		else 
		{
			ret_value = _empty_allow;			
		}
	}

	return	ret_value;
}

bool	GetValue(JSONNode const& _node, std::string const& _name, Date& _value, bool _empty_allow)
{
	bool	ret_value = false;

	if (_node.type() == JSON_NODE)
	{
		auto field = _node.find(_name);
		if (field != _node.end())
		{
			if ((field->type() == JSON_NUMBER) ||(field->type() == JSON_STRING))
			{
				time_t	time = field->as_int();
				_value = Date(time);

				ret_value = true;
			}
		}
		else 
		{
			ret_value = _empty_allow;			
		}
	}

	return	ret_value;
}

bool	GetValue(JSONNode const& _root, std::string const& _name, std::list<JSONNode>& _list, bool _empty_allow)
{
	bool	ret_value = false;

	if (_root.type() == JSON_NODE)
	{
		auto field = _root.find(_name);
		if (field != _root.end())
		{
			if (field->type() == JSON_ARRAY)
			{
				for(auto item = field->begin() ; item != field->end() ; item++)
				{
					_list.push_back(*item);
				}

				ret_value = true;
			}
		}
		else
		{
			ret_value = _empty_allow;
		}
	}

	return	ret_value;
}

bool	GetValue(JSONNode const& _node, std::string const& _name, JSONNode& _value, bool _empty_allow)
{
	bool	ret_value = false;

	if (_node.type() == JSON_NODE)
	{
		auto field = _node.find(_name);
		if (field != _node.end())
		{
			if (field->type() == JSON_NODE)
			{
				_value = field->as_node();
				ret_value = true;
			}
		}
		else if (_empty_allow)
		{
			ret_value = _empty_allow;			
		}
	}

	return	ret_value;
}

