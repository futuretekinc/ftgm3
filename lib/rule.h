
#ifndef	RULE_H_
#define	RULE_H_

#include <string>
#include <vector>
#include "object.h"

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
	virtual	bool	IsSatisfied() = 0;
	
	virtual operator JSONNode();
	virtual	JSONNode GetJSON();

	static Condition*	Create(JSONNode const& _properties);
private:
	std::string target_id_;
	bool		activated_;
};

class	LinearSingleCondition : public Condition
{
public:
	LinearSingleCondition(std::string const& _target_id, double _value);

	virtual	JSONNode GetJSON();
private:
	double	value_;
};

class	LinearOverCondition : public LinearSingleCondition
{
public:
	LinearOverCondition(std::string const& _target_id, double _value);

			Type	GetType() {	return	OVER;	};
			bool	IsSatisfied();
private:
};

class	LinearUnderCondition : public LinearSingleCondition
{
public:
	LinearUnderCondition(std::string const& _target_id, double _value);

			Type	GetType() {	return	UNDER;	};
			bool	IsSatisfied();
private:
};

class	LinearMatchCondition : public LinearSingleCondition
{
public:
	LinearMatchCondition(std::string const& _target_id, double _value);

			Type	GetType() {	return	MATCH;	};
			bool	IsSatisfied();
private:
};

class	LinearRangeCondition : public Condition 
{
public:
	LinearRangeCondition(std::string const& _target_id, double _min_value, double _max_value);
	~LinearRangeCondition();

	virtual	JSONNode	GetJSON();
private:
	double	lower_value_;
	double	upper_value_;
};

class	LinearRangeOutCondition : public LinearRangeCondition
{
public:
	LinearRangeOutCondition(std::string const& _target_id, double _min_value, double _max_value);
	~LinearRangeOutCondition();

			Type	GetType() { return RANGE_OUT; };
			bool	IsSatisfied();
private:
};

class	LinearRangeInCondition : public LinearRangeCondition
{
public:
	LinearRangeInCondition(std::string const& _target_id, double _min_value, double _max_value);
	~LinearRangeInCondition();

			Type	GetType() { return RANGE_IN; };
			bool	IsSatisfied();
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
private:
	std::string	target_id_;
};

class	DiscreteSingleAction : public Action
{
public:
	DiscreteSingleAction(std::string const& _target_id, double _value);
	~DiscreteSingleAction();

	virtual	JSONNode GetJSON();
private:
	double		value_;
};

class	DiscreteSetAction: public DiscreteSingleAction
{
public:
	DiscreteSetAction(std::string const& _target_id, double _value);

	virtual	Type	GetType() 	{	return	SET;	};
};

class	Rule : public Object
{
public:
	Rule();
	Rule(JSONNode const& _properties);
	~Rule();

	static	Rule*	Create(JSONNode const& _properties);
	virtual operator JSONNode();

	bool	GetProperty(uint32_t _type, JSONNode& _property);
	static	bool	GetPropertyFieldList(std::list<std::string>& _field_list);
private:
	std::vector<class Condition *>	conditions_;
	std::vector<class Action *>	actions_;
};

#endif
