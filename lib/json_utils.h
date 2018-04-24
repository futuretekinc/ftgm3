#ifndef	JSON_UTILS_H_
#define	JSON_UTILS_H_

#include <stdint.h>

uint32_t		JSONNodeGet(JSONNode const& _node, std::string const& field, uint32_t _default);
int32_t		JSONNodeGet(JSONNode const& _node, std::string const& field, int32_t _default);

std::string	JSONNodeGetID(JSONNode const& _node);
std::string	JSONNodeGetName(JSONNode const& _node);
std::string	JSONNodeGetName(JSONNode const& _node, std::string const& _default);
time_t		JSONNodeGetTime(JSONNode const& _node);
time_t		JSONNodeGetTime(JSONNode const& _node, time_t _default);
std::string	JSONNodeGetType(JSONNode const& _node);
std::string	JSONNodeGetModel(JSONNode const& _node);
time_t		JSONNodeGetStartTime(JSONNode const& _node, time_t _default);
time_t		JSONNodeGetEndTime(JSONNode const& _node, time_t _default);
time_t		JSONNodeGetLastTime(JSONNode const& _node);
time_t		JSONNodeGetTimeOfExpire(JSONNode const& _node);
uint32_t	JSONNodeGetCount(JSONNode const& _node, uint32_t _default = 0);
std::string	JSONNodeGetValue(JSONNode const& _node);
std::string	JSONNodeGetMsgID(JSONNode const& _node);
std::string	JSONNodeGetMsgID(JSONNode const& _node, std::string _default);
std::string	JSONNodeGetMsgType(JSONNode const& _node);
std::string	JSONNodeGetResult(JSONNode const& _node);
std::string     JSONNodeGetDefault(JSONNode const& _node, std::string _default);

bool		JSONNodeDelField(JSONNode& _node, std::string const& _name);
bool		JSONNodeDelType(JSONNode& _node);

JSONNode	JSONNodeGetNode(JSONNode const& _node, std::string const& _name);
JSONNode	JSONNodeGetTraceNode(JSONNode const& _node);
JSONNode	JSONNodeGetValueNode(JSONNode const& _node);

bool		JSONNodeIsExistField(JSONNode const& _node, std::string const& _name);
bool		JSONNodeIsExistValue(JSONNode const& _node);

bool		JSONNodeUpdate(JSONNode& _node, std::string const& _name, std::string const& _value);
bool		JSONNodeUpdate(JSONNode& _node, std::string const& _name, uint32_t _value);

JSONNode	JSONNodeLoadFromFile(std::string const& _file_name);

#endif
