
#include "defined.h"
#include "rule.h"
#include "json_utils.h"
#include "exception.h"

Condition::Condition(std::string const& _target_id)
: target_id_(_target_id), activated_(false)
{
}

Condition::~Condition()
{
}

Condition::operator JSONNode()
{
	return	GetJSON();
}

JSONNode	Condition::GetJSON()
{
	JSONNode	json;

	switch(GetType())
	{
	case	OVER: json.push_back(JSONNode("type", "over")); break;
	case	UNDER: json.push_back(JSONNode("type", "under")); break;
	case	MATCH: json.push_back(JSONNode("type", "match")); break;
	case	RANGE_IN: json.push_back(JSONNode("type", "range_in")); break;
	case	RANGE_OUT: json.push_back(JSONNode("type", "range_out")); break;
	}
	json.push_back(JSONNode("target_id", target_id_));

	return	json;
}

Condition*	Condition::Create(JSONNode const& _properties)
{
	Condition* condition = NULL;

	std::string type = JSONNodeGetType(_properties);
	JSONNode target_node = JSONNodeGetNode(_properties, "target_id");

	if (type == "over")
	{
		JSONNode value_node = JSONNodeGetNode(_properties, "value");

		condition = new LinearOverCondition(target_node.as_string(), value_node.as_float());
	}
	else if (type == "under")
	{
		JSONNode value_node = JSONNodeGetNode(_properties, "value");

		condition = new LinearUnderCondition(target_node.as_string(), value_node.as_float());
	}
	else if (type == "match")
	{
		JSONNode value_node = JSONNodeGetNode(_properties, "value");

		condition = new LinearMatchCondition(target_node.as_string(), value_node.as_float());
	}
	else if (type == "range_out")
	{
		JSONNode upper_value_node = JSONNodeGetNode(_properties, "upper_value");
		JSONNode lower_value_node = JSONNodeGetNode(_properties, "lower_value");

		condition = new LinearRangeOutCondition(target_node.as_string(), lower_value_node.as_float(), upper_value_node.as_float());
	}
	else if (type == "range_in")
	{
		JSONNode upper_value_node = JSONNodeGetNode(_properties, "upper_value");
		JSONNode lower_value_node = JSONNodeGetNode(_properties, "lower_value");

		condition = new LinearRangeInCondition(target_node.as_string(), lower_value_node.as_float(), upper_value_node.as_float());
	}

	return	condition;
}

LinearSingleCondition::LinearSingleCondition(std::string const& _target_id, double _value)
: Condition(_target_id), value_(_value)
{
}

JSONNode	LinearSingleCondition::GetJSON()
{
	JSONNode	json = Condition::GetJSON();

	json.push_back(JSONNode("value", value_));

	return	json;
}

LinearOverCondition::LinearOverCondition(std::string const& _target_id, double _value)
: LinearSingleCondition(_target_id, _value)
{
}

bool LinearOverCondition::IsSatisfied()
{
	return	false;
}

LinearUnderCondition::LinearUnderCondition(std::string const& _target_id, double _value)
: LinearSingleCondition(_target_id, _value)
{
}

bool LinearUnderCondition::IsSatisfied()
{
	return	false;
}

LinearMatchCondition::LinearMatchCondition(std::string const& _target_id, double _value)
: LinearSingleCondition(_target_id, _value)
{
}

bool LinearMatchCondition::IsSatisfied()
{
	return	false;
}

LinearRangeCondition::LinearRangeCondition(std::string const& _target_id, double _lower_value, double _upper_value)
: Condition(_target_id), lower_value_(_lower_value), upper_value_(_upper_value)
{
}

LinearRangeCondition::~LinearRangeCondition()
{
}

JSONNode	LinearRangeCondition::GetJSON()
{
	JSONNode	json = Condition::GetJSON();

	json.push_back(JSONNode("lower_value", lower_value_));
	json.push_back(JSONNode("upper_value", upper_value_));

	return	json;
}

LinearRangeOutCondition::LinearRangeOutCondition(std::string const& _target_id, double _lower_value, double _upper_value)
: LinearRangeCondition(_target_id, _lower_value, _upper_value)
{
}

LinearRangeOutCondition::~LinearRangeOutCondition()
{
}

bool LinearRangeOutCondition::IsSatisfied()
{
	return	false;
}

LinearRangeInCondition::LinearRangeInCondition(std::string const& _target_id, double _lower_value, double _upper_value)
: LinearRangeCondition(_target_id, _lower_value, _upper_value)
{
}

LinearRangeInCondition::~LinearRangeInCondition()
{
}

bool LinearRangeInCondition::IsSatisfied()
{
	return	false;
}

Action::Action(std::string const& _target_id)
: target_id_(_target_id)
{
}

Action::~Action()
{
}

Action::operator JSONNode()
{
	return	GetJSON();
}

JSONNode	Action::GetJSON()
{
	JSONNode	json;

	switch(GetType())
	{
	case	SET: json.push_back(JSONNode("type", "set")); break;
	}
	json.push_back(JSONNode("target_id", target_id_));

	return	json;
}

Action*	Action::Create(JSONNode const& _properties)
{
	Action* action= NULL;

	std::string type = JSONNodeGetType(_properties);
	JSONNode target_node = JSONNodeGetNode(_properties, "target_id");

	if (type == "set")
	{
		JSONNode value_node = JSONNodeGetNode(_properties, "value");

		action = new DiscreteSetAction(target_node.as_string(), value_node.as_float());
	}

	return	action;
}

DiscreteSingleAction::DiscreteSingleAction(std::string const& _target_id, double _value)
: Action(_target_id), value_(_value)
{
}

DiscreteSingleAction::~DiscreteSingleAction()
{
}

JSONNode DiscreteSingleAction::GetJSON()
{
	JSONNode	json = Action::GetJSON();

	json.push_back(JSONNode("value", value_));

	return	json;
}

DiscreteSetAction::DiscreteSetAction(std::string const& _target_id, double _value)
: DiscreteSingleAction(_target_id, _value)
{
}


Rule::Rule()
{
}

Rule::Rule(JSONNode const& _rule_properties)
{
	try
	{
		JSONNode	condition_properties = JSONNodeGetNode(_rule_properties, "conditions");
		if (condition_properties.type() != JSON_ARRAY)
		{
			if (condition_properties.type() == JSON_STRING) 
			{
				condition_properties = libjson::parse(condition_properties.as_string());
				if (condition_properties.type() != JSON_ARRAY)
				{
					throw InvalidArgument("Conditions is not array");
				}

			}
			else
			{
				throw InvalidArgument("Conditions is not array");
			}
		}

		for(JSONNode::iterator it = condition_properties.begin() ; it != condition_properties.end() ; it++)
		{
			Condition* condition = Condition::Create(*it);
			if (condition == NULL)
			{
				throw Asserted("Condition creation failed.");
			}

			conditions_.push_back(condition);		
		}
	}
	catch(ObjectNotFound& e)
	{
		JSONNode	condition_properties = JSONNodeGetNode(_rule_properties, "condition");
		Condition* condition = Condition::Create(condition_properties);
		if (condition == NULL)
		{
			throw Asserted("Condition creation failed.");
		}

		conditions_.push_back(condition);		
	}

	try
	{
		JSONNode	action_properties = JSONNodeGetNode(_rule_properties, "actions");
		if (action_properties.type() != JSON_ARRAY)
		{	
			if (action_properties.type() == JSON_STRING) 
			{
				action_properties = libjson::parse(action_properties.as_string());
				if (action_properties.type() != JSON_ARRAY)
				{
					throw InvalidArgument("Conditions is not array");
				}
			}
			else
			{
				throw InvalidArgument("Actions is not array");
			}
		}

		for(JSONNode::iterator it = action_properties.begin() ; it != action_properties.end() ; it++)
		{
			Action* action = Action::Create(*it);
			if (action == NULL)
			{
				throw Asserted("Action creation failed.");
			}

			actions_.push_back(action);		
		}
	}
	catch(ObjectNotFound& e)
	{
		JSONNode	action_properties = JSONNodeGetNode(_rule_properties, "action");
		Action* action = Action::Create(action_properties);
		if (action == NULL)
		{
			throw Asserted("Action creation failed.");
		}

		actions_.push_back(action);		
	}
}

Rule::~Rule()
{
}

bool	Rule::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	if (Object::GetPropertyFieldList(_field_list))
	{
		_field_list.push_back(TITLE_NAME_PARENT_ID);
		_field_list.push_back(TITLE_NAME_CONDITIONS);
		_field_list.push_back(TITLE_NAME_ACTIONS);
	}

	return	true;
}

Rule::operator JSONNode()
{
	JSONNode	json;

	json = Object::operator JSONNode() ;

	JSONNode	conditions_json(JSON_ARRAY);

	conditions_json.set_name("conditions");
	for(std::vector<Condition*>::iterator it = conditions_.begin() ; it != conditions_.end() ; it++)
	{
		conditions_json.push_back(JSONNode(**it));
	}

	json.push_back(conditions_json);

	JSONNode	actions_json(JSON_ARRAY);

	actions_json.set_name("actions");
	for(std::vector<Action*>::iterator it = actions_.begin() ; it != actions_.end() ; it++)
	{
		actions_json.push_back(JSONNode(**it));
	}

	json.push_back(actions_json);

	return	json;
}

Rule* Rule::Create(JSONNode const& _properties)
{
	return	NULL;
}

bool	Rule::GetProperty(uint32_t _type, JSONNode& _property)
{
	switch(_type)
	{
	case	PROPERTY_CONDITION_FLAG: 
		{
			JSONNode	conditions_json(JSON_ARRAY);

			for(std::vector<Condition*>::iterator it = conditions_.begin() ; it != conditions_.end() ; it++)
			{
				conditions_json.push_back(JSONNode(**it));
			}

			_property = JSONNode(TITLE_NAME_CONDITIONS, conditions_json.write());
		}
		break;

	case	PROPERTY_ACTION_FLAG:
		{
			JSONNode	actions_json(JSON_ARRAY);

			for(std::vector<Action*>::iterator it = actions_.begin() ; it != actions_.end() ; it++)
			{
				actions_json.push_back(JSONNode(**it));
			}
			_property = JSONNode(TITLE_NAME_ACTIONS, actions_json.write());
		}
		break;

	default:	
		{
			return	Object::GetProperty(_type, _property);
		}
		break;
	}
	
	return	true;	
}

