#include "rule_manager.h"
#include "object_manager.h"
#include "exception.h"

RuleManager::RuleManager(ObjectManager* _object_manager)
: object_manager_(_object_manager)
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

bool	RuleManager::Attach(ObjectManager* _object_manager)
{
	object_manager_ = _object_manager;

	return	true;
}

ObjectManager*	RuleManager::GetObjectManager(void)
{
	return	object_manager_;
}

Rule*	RuleManager::CreateRule(JSONNode const& _properties)
{
	try
	{
		Rule*	rule = new Rule(*this, _properties);		
		if (rule != NULL)
		{
			rules_.push_back(rule);
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

bool	RuleManager::DestroyRule(std::string const& _id)
{
	for(std::vector<Rule*>::iterator it = rules_.begin() ; it != rules_.end() ; it++)
	{
		Rule*	rule = *it;

		if (rule->GetID() == _id)
		{
			delete rule;
			rules_.erase(it);

			return	true;
		}
	}

	return	false;
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


bool	RuleManager::Updated(std::string const& _id, Date const& _time, std::string const& _value)
{
	for(std::vector<Rule*>::iterator it = rules_.begin() ; it != rules_.end() ; it++)
	{
		Rule*	rule = *it;

			TRACE_INFO("Rule[" << rule->GetID() << "] check!");
		if (rule->IsIncludedInCondition(_id))
		{
			if (rule->Process(_id, _time, _value, rule->GetID()))
			{
				TRACE_INFO("Rule[" << rule->GetID() << "] applied!");
			}
		}
	}

	return	true;
}
