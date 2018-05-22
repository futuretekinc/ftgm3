#ifndef	RULE_MANAGER_H_
#define	RULE_MANAGER_H_

#include <string>
#include <vector>
#include "process_object.h"
#include "rule.h"

class	RuleManager : public ProcessObject 
{
public:

	RuleManager();
	~RuleManager();

			Rule*	CreateRule(JSONNode const& _properties);

			int		GetRuleCount();
			Rule*	GetRule(int _index);
			Rule*	GetRule(std::string const& _id);

			int		GetRuleList(std::list<Rule*>& _list);
			bool	Attach(Rule* _rule);
private:
	std::vector<Rule*> 	rules_;
};

#endif
