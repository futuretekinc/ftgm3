
#ifndef	RULE_H_
#define	RULE_H_

#include <string>
#include <vector>
#include "object.h"

class	ObjectManager;
class	RuleManager;


class	Condition : public Object
{
public:
	enum Type 
	{
		OVER,
		UNDER,
		MATCH,
		RANGE_OUT,
		RANGE_IN
	};

	Condition(std::string const& _target_id);
	~Condition();

	virtual	Type	GetType() = 0;
			bool	IsSatisfied()	{ return satisfied_;};
	
	virtual operator JSONNode();
	virtual	JSONNode GetJSON();

	static 	Condition*	Create(JSONNode const& _properties);
			std::string&	TargetID()	{	return	target_id_;	}

	virtual	bool	Apply(Date const& _time, std::string const& _value) = 0;
protected:
	std::string target_id_;
	bool		activated_;
	bool		satisfied_;
};

class	LinearSingleCondition : public Condition
{
public:
	LinearSingleCondition(std::string const& _target_id, double _value);

	virtual	JSONNode GetJSON();
protected:
	double	value_;
};

class	LinearOverCondition : public LinearSingleCondition
{
public:
	LinearOverCondition(std::string const& _target_id, double _value);

			Type	GetType() {	return	OVER;	};
			bool	Apply(Date const& _time, std::string const& _value);
private:
};

class	LinearUnderCondition : public LinearSingleCondition
{
public:
	LinearUnderCondition(std::string const& _target_id, double _value);

			Type	GetType() {	return	UNDER;	};
			bool	Apply(Date const& _time, std::string const& _value);
private:
};

class	LinearMatchCondition : public LinearSingleCondition
{
public:
	LinearMatchCondition(std::string const& _target_id, double _value);

			Type	GetType() {	return	MATCH;	};
			bool	Apply(Date const& _time, std::string const& _value);
private:
};

class	LinearRangeCondition : public Condition 
{
public:
	LinearRangeCondition(std::string const& _target_id, double _min_value, double _max_value);
	~LinearRangeCondition();

	virtual	JSONNode	GetJSON();
protected:
	double	lower_value_;
	double	upper_value_;
};

class	LinearRangeOutCondition : public LinearRangeCondition
{
public:
	LinearRangeOutCondition(std::string const& _target_id, double _min_value, double _max_value);
	~LinearRangeOutCondition();

			Type	GetType() { return RANGE_OUT; };
			bool	Apply(Date const& _time, std::string const& _value);
private:
};

class	LinearRangeInCondition : public LinearRangeCondition
{
public:
	LinearRangeInCondition(std::string const& _target_id, double _min_value, double _max_value);
	~LinearRangeInCondition();

			Type	GetType() { return RANGE_IN; };
			bool	Apply(Date const& _time, std::string const& _value);
private:
};

class	Action : public Object
{
public:
	enum Type 
	{
		SET
	};

	Action(std::string const& _target_id);
	~Action();

	virtual	Type	GetType() = 0;
	virtual operator JSONNode();
	virtual	JSONNode GetJSON();

	static Action*	Create(JSONNode const& _properties);

	virtual	bool	Process(ObjectManager* _object_manager) = 0;
	virtual bool    Process_after(ObjectManager* _object_manager) = 0;
	std::string	GetTargetID(){ return target_id_;};
protected:
	std::string	target_id_;
};

class	DiscreteSingleAction : public Action
{
public:
	DiscreteSingleAction(std::string const& _target_id, std::string const& _value);
	~DiscreteSingleAction();

	virtual	JSONNode GetJSON();
protected:
	std::string	value_;
};

class	DiscreteSetAction: public DiscreteSingleAction
{
public:
	DiscreteSetAction(std::string const& _target_id, std::string const& _value);

	virtual	Type	GetType() 	{	return	SET;	};
	virtual	bool	Process(ObjectManager* _object_manager);
	virtual bool    Process_after(ObjectManager* _object_manager);
};

class	Rule : public Object
{
public:
	Rule(RuleManager& _manager);
	Rule(RuleManager& _manager, JSONNode const& _properties);
	~Rule();

	static	Rule*	Create(JSONNode const& _properties);
	virtual operator JSONNode();

			bool	GetProperty(uint32_t _type, JSONNode& _property);
	static	bool	GetPropertyFieldList(std::list<std::string>& _field_list);

			bool	IsIncludedInCondition(std::string const& _id);
			bool	Process(std::string const& _endpoint_id, Date const& _time, std::string const& _value, std::string const& _rule_id);
			bool    SendEvent(std::string const& _rule_id, std::string const& _value, std::string const& _action_target_id);
private:
	RuleManager&	manager_;
	std::vector<class Condition *>	conditions_;
	std::vector<class Action *>	actions_;
};

#endif
