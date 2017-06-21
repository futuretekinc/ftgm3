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
bool	GetValue(JSONNode const& _node, std::string const& _name, std::string& _value, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, time_t& _value, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, Date& _value, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, std::list<JSONNode>& _list, bool _empty_allow = 0);
bool	GetValue(JSONNode const& _node, std::string const& _name, JSONNode& _value, bool _empty_allow = 0);

void	ThrowFunctionFailed(std::string const& _argument);

bool	IsValidIP(const std::string& _ip);
bool	IsTrue(const std::string& _value);

std::string	ToString(double _value, int _precision);
std::string ToString(bool _value);
std::string ToString(time_t	_value);
std::string ToString(int _value);
std::string ToString(uint16_t _value);
std::string ToString(uint32_t _value);
std::string ToString(uint64_t  _value);

#endif
