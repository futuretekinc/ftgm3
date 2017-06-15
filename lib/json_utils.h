#ifndef	JSON_UTILS_H_
#define	JSON_UTILS_H_

std::string	JSONNodeGetID(JSONNode const& _node);
time_t		JSONNodeGetStartTime(JSONNode const& _node, time_t _default);
time_t		JSONNodeGetEndTime(JSONNode const& _node, time_t _default);
time_t		JSONNodeGetTimeOfExpire(JSONNode const& _node);
std::string	JSONNodeGetValue(JSONNode const& _node);
std::string	JSONNodeGetMsgID(JSONNode const& _node);
std::string	JSONNodeGetMsgID(JSONNode const& _node, std::string _default);

#endif
