#ifndef	UTILS_H_
#define	UTILS_H_

#include <list>
#include <libjson/libjson.h>
#include "time2.h"
#include "value.h"

bool	GetValue(JSONNode const& _node, bool& _value);
bool	GetValue(JSONNode const& _node, std::string& _value);

bool	GetValue(JSONNode const& _node, std::string const& _name, bool& _value, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, uint32_t& _value, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, float& _value, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, ValueFloat& _value, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, std::string& _value, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, time_t& _value, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, Date& _value, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, std::list<JSONNode>& _list, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, JSONNode& _value, bool _empty_allow = 0);


void	ThrowFunctionFailed(std::string const& _argument);

class	Asserted : public std::exception
{
public:
	explicit Asserted(std::string const& _argument);

	virtual const char* 	what() const throw();

protected:
	std::string	message_;
};

class	InvalidArgument : public std::exception
{
public:
	explicit InvalidArgument(std::string const& _argument);
	explicit InvalidArgument(std::string const& _name, Value const* _value);

	virtual const char* 	what() const throw();

protected:
	std::string	message_;
};

class	ObjectNotFound : public std::exception
{
public:
	explicit ObjectNotFound(std::string const& _object_id);

	const	std::string&	GetObjectID()		{	return	object_id_;		}
	virtual const char* 	what() const throw();

protected:
	std::string	object_id_;
	std::string	message_;
};

class	NotInitialized : public std::exception
{
public:
	explicit	NotInitialized(std::string const& _message);

protected:
	std::string	message_;
};

#endif
