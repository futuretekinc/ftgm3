#ifndef	JSON_UTILS_H_
#define	JSON_UTILS_H_

std::string	JSONNodeGetID(JSONNode const& _node);
std::string	JSONNodeGetName(JSONNode const& _node);
std::string	JSONNodeGetName(JSONNode const& _node, std::string const& _default);
time_t		JSONNodeGetTime(JSONNode const& _node);
time_t		JSONNodeGetTime(JSONNode const& _node, time_t _default);
std::string	JSONNodeGetType(JSONNode const& _node);
time_t		JSONNodeGetStartTime(JSONNode const& _node, time_t _default);
time_t		JSONNodeGetEndTime(JSONNode const& _node, time_t _default);
time_t		JSONNodeGetTimeOfExpire(JSONNode const& _node);
uint32_t	JSONNodeGetCount(JSONNode const& _node, uint32_t _default = 0);
std::string	JSONNodeGetValue(JSONNode const& _node);
std::string	JSONNodeGetMsgID(JSONNode const& _node);
std::string	JSONNodeGetMsgID(JSONNode const& _node, std::string _default);

JSONNode	JSONNodeGetTraceNode(JSONNode const& _node);
JSONNode	JSONNodeGetValueNode(JSONNode const& _node);

bool		JSONNodeIsExistValue(JSONNode const& _node);
#endif
