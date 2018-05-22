#ifndef	RCS_MESSAGE_H_
#define	RCS_MESSAGE_H_

#include <list>
#include "defined.h"
#include "active_object.h"
//#include "librdkafka/rdkafkacpp.h"
#include "timer.h"
#include "gateway.h"
#include "device.h"
#include "endpoint.h"


class	RCSMessage
{
public:
	RCSMessage(RCSMessage const& _message);
	RCSMessage(std::string const& _type = MSG_TYPE_RCS_ERROR);
	RCSMessage(JSONNode const& _message);
	~RCSMessage();

	const 	std::string&	GetMsgID()		const	{	return	msg_id_;		}
	const	std::string&	GetMsgType()	const	{	return	msg_type_;		}
			bool			SetMsgType(std::string const& _type);
	const	std::string&	GetSecretCode()	const	{	return	secret_code_;	}
	const	Date&			GetTime()		const	{	return	time_;		}
	const	std::string&	GetReqID()		const	{	return	req_id_;	}
			bool			SetReqID(std::string const& _req_id);
			JSONNode&		GetPayload();

			bool			SetSecretKey(std::string const& _key);
			bool			AddGateway(JSONNode const& _properties);
			bool			AddGatewayID(std::string const& _id);
			bool			AddGatewayNull();
			bool			AddGatewayFields(Fields _fields);
	const	Fields&			GetGatewayFields()	{	return	gateway_fields_;	};
			uint32_t		GetGatewayCount();
			JSONNode		GetGateway(uint32_t index);

			bool			AddDevice(JSONNode const& _properties);
			bool			AddDeviceID(std::string const& _id);
			bool			AddDeviceNull();
			bool			AddDeviceFields(Fields _fields);
	const	Fields&			GetDeviceFields()	{	return	device_fields_;	};
			uint32_t		GetDeviceCount();
			JSONNode		GetDevice(uint32_t index);

			bool			AddEndpoint(JSONNode const& _properties);
			bool			AddEndpointID(std::string const& _id);
			bool			AddEndpointNull();
			bool			AddEndpointFields(Fields _fields);
	const	Fields&			GetEndpointFields()	{	return	endpoint_fields_;	};
			uint32_t		GetEndpointCount();
			JSONNode		GetEndpoint(uint32_t index);

			bool			AddEPData(JSONNode const& _epdata);
			uint32_t		GetEPDataCount();
			JSONNode		GetEPData(uint32_t index);

			bool			AddRule(JSONNode const& _rule);
			uint32_t		GetRuleCount();
			JSONNode		GetRule(uint32_t index);


			bool			Make();

protected:
	std::string		msg_id_;
	std::string		msg_type_;
	std::string		secret_key_;
	std::string		secret_code_;
	std::string		req_id_;
	Date			time_;
	JSONNode		payload_;
	Fields			gateway_fields_;
	Fields			device_fields_;
	Fields			endpoint_fields_;
	bool			gateway_is_null_;
	bool			device_is_null_;
	bool			endpoint_is_null_;

	std::list<JSONNode>	gateway_list_;
	std::list<JSONNode>	device_list_;
	std::list<JSONNode>	endpoint_list_;
	std::list<JSONNode>	epdata_list_;
	std::list<JSONNode>	rule_list_;
};



#endif
