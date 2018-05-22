#include "rule_manager.h"
#include "exception.h"

RuleManager::RuleManager()
{
	name_ 	= "rule_manager";
	enable_ = true;

	trace.SetEnable(true);
	trace.SetClassName(GetClassName());
}

RuleManager::~RuleManager()
{
	for(std::vector<Rule*>::iterator it = rules_.begin() ; it != rules_.end() ; it++)
	{
		delete *it;
	}
}

Rule*	RuleManager::CreateRule(JSONNode const& _properties)
{
	try
	{
		Rule*	rule = new Rule(_properties);		
		if (rule != NULL)
		{
			Attach(rule);
		}

		return	rule;
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR("Invalid rule properties!");
		TRACE_ERROR("What : " << e.what());
		return	NULL;
	}
}

int		RuleManager::GetRuleCount()
{
	return	rules_.size();
}

Rule*	RuleManager::GetRule(int _index)
{
	return	rules_[_index];
}

Rule*	RuleManager::GetRule(std::string const& _id)
{
	for(std::vector<Rule*>::iterator it = rules_.begin() ; it != rules_.end() ; it++)
	{
		Rule*	rule = *it;

		if (rule->GetID() == _id)
		{
			return	rule;
		}
	}
	
	return	NULL;
}

int		RuleManager::GetRuleList(std::list<Rule*>& _list)
{
	for(std::vector<Rule*>::iterator it = rules_.begin() ; it != rules_.end() ; it++)
	{
		_list.push_back(*it);
	}

	return	_list.size();
}

bool	RuleManager::Attach(Rule* _rule)
{
	rules_.push_back(_rule);

	return	true;
}
