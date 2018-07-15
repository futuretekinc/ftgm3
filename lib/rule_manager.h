#ifndef	RULE_MANAGER_H_
#define	RULE_MANAGER_H_

#include <string>
#include <vector>
#include "process_object.h"
#include "rule.h"

class	ObjectManager;

class	RuleManager : public ProcessObject 
{
public:

	RuleManager(ObjectManager* objectManager);
	~RuleManager();

			bool	Attach(ObjectManager*	_object_manager);
			ObjectManager*	GetObjectManager(void);

			Rule*	CreateRule(JSONNode const& _properties);
			bool	DestroyRule(std::string const& _id);

			int		GetRuleCount();
			Rule*	GetRule(int _index);
			Rule*	GetRule(std::string const& _id);

			int		GetRuleList(std::list<Rule*>& _list);

			bool	Updated(std::string const& _id, Date const& _time, std::string const& _value);


private:
	ObjectManager*		object_manager_;
	std::vector<Rule*> 	rules_;
};

#endif
