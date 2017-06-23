#include <exception>
#include <list>
#include <string>
#include <iomanip>
#include "node.h"
#include "defined.h"
#include "exception.h"
#include "json.h"
#include "gateway.h"
#include "property.h"
#include "endpoint.h"
#include "endpoint_actuator.h"
#include "server_linker.h"
#include "rcs_message.h"
#include "object_manager.h"
#include "trace.h"
#include "time2.h"
#include "sha1.h"
#include "md5.h"
#include "sha256.h"
#include "utils.h"

ServerLinker::Produce::Produce(std::string const& _topic, RCSMessage const& _message)
: Message(MSG_TYPE_SL_PRODUCE), topic_(_topic), message_(_message), transmission_count_(0)
{
}

ServerLinker::Produce::Produce(std::string const& _topic, std::string const& _payload)
: Message(MSG_TYPE_SL_PRODUCE), topic_(_topic), transmission_count_(0)
{
	if (libjson::is_valid(_payload))
	{
		message_ = libjson::parse(_payload);

		message_.Make();
	}
}

ServerLinker::Consume::Consume(std::string const& _topic, std::string const& _payload)
: Message(MSG_TYPE_SL_CONSUME), topic_(_topic)
{
	if (libjson::is_valid(_payload))
	{
		message_ = libjson::parse(_payload);

		message_.Make();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker::Link
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinker::Link::Link(ServerLinker* _linker, std::string const& _topic)
: Object(), linker_(_linker), topic_(_topic), connected_(false)
{
	trace.SetClassName(GetClassName());
	name_ 	= "link";
	enable_ = true;
}

bool	ServerLinker::Link::Touch()
{
	keep_alive_timer_.Set(Date::GetCurrent() + Time(60000000));

	return	true;
}

uint32_t	ServerLinker::Link::GetLiveTime()
{
	return	keep_alive_timer_.RemainTime().GetSeconds();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker::UpLink
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinker::UpLink::UpLink(ServerLinker* _linker, std::string const& _topic)
: Link(_linker, _topic), number_of_out_going_messages_(0), number_of_error_messages_(0), qos_(0)
{
}

bool	ServerLinker::UpLink::Start()
{
	Touch();
	
	TRACE_INFO("Uplink[" << topic_ << "] started");
	return	true;
}

bool	ServerLinker::UpLink::Stop()
{
	return	true;
}

bool	ServerLinker::UpLink::Send(std::string const& _message)
{
	return	true;
}

bool	ServerLinker::UpLink::Send(std::string const& _message, std::string& _msg_id)
{
	return	true;
}

bool	ServerLinkerMosq::UpLink::SetQoS(uint32_t _qos)
{
	qos_ = _qos;

	return	true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker::DownLink
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinker::DownLink::DownLink(ServerLinker* _linker, std::string const& _topic)
: Link(_linker, _topic), number_of_incomming_messages_(0), number_of_error_messages_(0)
{
}

bool	ServerLinker::DownLink::Start()
{
	TRACE_INFO("Subscribe[" << topic_ << "] connected!!!");
	connected_ = true;

	return	true;
}

bool	ServerLinker::DownLink::Stop()
{
	TRACE_INFO("Subscribe[" << topic_ << "] disconnected!!!");
	connected_ = false;

	return	true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinker::ServerLinker(ObjectManager* _manager)
: 	ProcessObject(), 
	manager_(_manager), 
	secret_key_(""),
	broker_connected_(false),
	broker_retry_interval_(SERVER_LINKER_CONNECTION_RETRY_INTERVAL_SEC * TIME_SECOND),
	auto_connection_(true),
	keep_alive_enable_(true),
	report_late_arrive_message_(SERVER_LINKER_REPORT_LATE_ARRIVE_MESSAGE),
	request_timeout_(SERVER_LINKER_REQUEST_TIMEOUT_SEC),
	topic_version_(SERVER_LINKER_TOPIC_VERSION),
	topic_id_(SERVER_LINKER_TOPIC_ID),
	global_up_name_(SERVER_LINKER_TOPIC_GLOBAL_UP_NAME),
	global_down_name_(SERVER_LINKER_TOPIC_GLOBAL_DOWN_NAME),
	retransmission_count_max_(SERVER_LINKER_RETRANSMISSION_COUNT_MAX),
	request_map_locker_()
{
	trace.SetClassName(GetClassName());
	name_ 	= "linker";
	enable_ = true;

	SetGlobalUpTopic(topic_version_,  global_up_name_, topic_id_);
	SetGlobalDownTopic(topic_version_,global_up_name_, topic_id_);

	SetHashAlg(DEFAULT_CONST_SERVER_LINKER_HASH);

}

ServerLinker::~ServerLinker()
{
	for(std::map<std::string, UpLink*>::iterator it = up_link_map_.begin() ; it != up_link_map_.end() ; it++)
	{
		delete it->second;	
	}

	for(std::map<std::string, DownLink*>::iterator it = down_link_map_.begin() ; it != down_link_map_.end() ; it++)
	{
		delete it->second;	
	}
}

bool	ServerLinker::SetProperty(JSONNode const& _config, bool _check)
{
	bool	ret_value = true;

	if (_config.name() == TITLE_NAME_SECRET_KEY)
	{
		ret_value = SetSecretKey(_config.as_string(), _check);	
	}
	else if (_config.name() == TITLE_NAME_HASH)
	{
		ret_value = SetHashAlg(_config.as_string(), _check);
	}
	else if (_config.name() == TITLE_NAME_BROKER)
	{
		ret_value = SetBroker(_config.as_string(), _check);
	}
	else if (_config.name() == TITLE_NAME_TOPIC)
	{
		if (_config.type() == JSON_NODE)
		{
			for(JSONNode::const_iterator it = _config.begin(); it != _config.end() ; it++)
			{
				if (it->name() == TITLE_NAME_VERSION)
				{
					ret_value = SetTopicVersion(it->as_string(), _check);
				}
				else if (it->name() == TITLE_NAME_GLOBAL_UP_NAME)
				{
					ret_value = SetGlobalUpName(it->as_string(), _check);
				}
				else if (it->name() == TITLE_NAME_GLOBAL_DOWN_NAME)
				{
					ret_value = SetGlobalDownName(it->as_string(), _check);
				}
				if (it->name() == TITLE_NAME_ID)
				{
					ret_value = SetTopicID(it->as_string(), _check);
				}
			}
		}
		else
		{
			return	false;	
		}
	}
	else
	{
		ret_value = ProcessObject::SetProperty(_config, _check);	
	}

	return	ret_value;
}

ServerLinker::operator JSONNode() const
{
	JSONNode	root;

	root.push_back(JSONNode(TITLE_NAME_HASH_ALG, hash_alg_name_));
	root.push_back(JSONNode(TITLE_NAME_SECRET_KEY, secret_key_));
	root.push_back(JSONNode(TITLE_NAME_BROKER, broker_));

	JSONNode	topic;

	topic.push_back(JSONNode(TITLE_NAME_VERSION, topic_version_));
	topic.push_back(JSONNode(TITLE_NAME_GLOBAL_UP_NAME, global_up_name_));
	topic.push_back(JSONNode(TITLE_NAME_GLOBAL_DOWN_NAME, global_down_name_));
	topic.push_back(JSONNode(TITLE_NAME_ID, topic_id_));
	topic.set_name(TITLE_NAME_TOPIC);

	root.push_back(topic);

	JSONNode	trace_config = trace;
	trace_config.set_name(TITLE_NAME_TRACE);

	root.push_back(trace_config);

	return	root;
}

bool	ServerLinker::SetHashAlg(std::string const& _name, bool _check)
{
	if (_name == "sha1")
	{
		if (!_check)
		{
			hash_alg_name_ = _name;
			secret_code_hash_ = sha1;
		}
	}
	else if (_name == "md5")
	{
		if (!_check)
		{
			hash_alg_name_ = _name;
			secret_code_hash_ = md5;
		}
	}
	else if (_name == "sha256")
	{
		if (!_check)
		{
			hash_alg_name_ = _name;
			secret_code_hash_ = sha256;	
		}
	}
	else
	{
		TRACE_ERROR("Not supported hash algorithm[" << _name << "]");
		if (!_check)
		{
			hash_alg_name_ = "sha1";
			secret_code_hash_ = sha1;
		}
		return	false;
	}

	return	true;
}

bool	ServerLinker::SetTopicVersion(std::string const& _version, bool _check)
{
	if (!_check)
	{
		topic_version_ = _version;
		SetGlobalUpTopic(topic_version_,  global_up_name_, topic_id_);
		SetGlobalDownTopic(topic_version_,  global_down_name_, topic_id_);
	}

	return	true;
}

const std::string&	ServerLinker::GetTopicVersion()
{
	return	topic_version_;
}

bool	ServerLinker::SetTopicID(std::string const& _id, bool _check)
{
	if (!_check)
	{
		topic_id_ = _id;
		SetGlobalUpTopic(topic_version_,  global_up_name_, topic_id_);
		SetGlobalDownTopic(topic_version_,  global_down_name_, topic_id_);
	}

	return	true;
}

const std::string&	ServerLinker::GetTopicID()
{
	return	topic_id_;
}

bool	ServerLinker::SetGlobalUpName(std::string const& _name, bool _check)
{
	if (!_check)
	{
		global_up_name_ = _name;
		SetGlobalUpTopic(topic_version_,  global_up_name_, topic_id_);
	}

	return	true;
}

const std::string&	ServerLinker::GetGlobalUpName()
{
	return	global_up_name_;
}

bool	ServerLinker::SetGlobalDownName(std::string const& _name, bool _check)
{
	if (!_check)
	{
		global_down_name_ = _name;
		SetGlobalDownTopic(topic_version_,  global_down_name_, topic_id_);
	}

	return	true;
}

const std::string&	ServerLinker::GetGlobalDownName()
{
	return	global_down_name_;
}

bool	ServerLinker::SetGlobalUpTopic(std::string const& _version, std::string const& _name, std::string const& _id, bool _check)
{
	if (!_check)
	{
		global_up_topic_ = _version + "/" + _name + "/" + _id;
	}

	return	true;
}

const std::string&	ServerLinker::GetGlobalUpTopic()
{
	return	global_up_topic_;
}

bool	ServerLinker::SetGlobalDownTopic(std::string const& _version, std::string const& _name, std::string const& _id, bool _check)
{
	if (!_check)
	{
		global_down_topic_ = _version + "/" + _name + "/" + _id;
	}

	return	true;
}

const std::string&	ServerLinker::GetGlobalDownTopic()
{
	return	global_down_topic_;
}

bool	ServerLinker::SetSecretKey(std::string const& _secret_key, bool _check)
{
	if (!_check)
	{
		secret_key_=  _secret_key;
	}

	return	true;
}

bool	ServerLinker::GetSecretKey(std::string & _secret_key)
{
	_secret_key = secret_key_;

	return	true;
}

bool	ServerLinker::SetBroker(std::string const& _broker, bool _check)
{
	if (!_check)
	{
		broker_ = _broker;
	}

	return	true;
}

const std::string&	ServerLinker::GetBroker()
{
	return	broker_;
}

bool		ServerLinker::SetAutoConnection(bool _auto)
{
	auto_connection_ = _auto;

	return	true;
}

bool		ServerLinker::SetAutoConnection(std::string const& _auto, bool _check)
{
	bool	value;

	try
	{
		value = IsTrue(_auto);
	}
	catch(InvalidArgument& e)
	{
		TRACE_ERROR(e.what());
		return	false;
	}

	if (!_check)
	{
		auto_connection_ = value;
	}

	return	true;
}


uint32_t	ServerLinker::GetUpLink(std::vector<UpLink*>& _link_list)
{
	for(std::map<std::string, UpLink*>::iterator it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
	{
		_link_list.push_back(it->second);		
	}

	return	_link_list.size();
}

uint32_t	ServerLinker::GetUpLink(std::list<std::string>& _topic_list)
{
	for(std::map<std::string, UpLink*>::iterator it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
	{
		_topic_list.push_back(it->first);		
	}

	return	_topic_list.size();
}

uint32_t	ServerLinker::GetDownLink(std::vector<DownLink*>& _link_list)
{
	for(std::map<std::string, DownLink*>::iterator it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
	{
		_link_list.push_back(it->second);		
	}

	return	_link_list.size();
}

uint32_t	ServerLinker::GetDownLink(std::list<std::string>& _topic_list)
{
	for(std::map<std::string, DownLink*>::iterator it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
	{
		_topic_list.push_back(it->first);		
	}

	return	_topic_list.size();
}

ServerLinker::UpLink*	ServerLinker::CreateUpLink(std::string const& _topic)
{
	return	new UpLink(this, _topic);	
}

ServerLinker::UpLink*	ServerLinker::AddUpLink(std::string const& _topic)
{
	UpLink*	link = NULL;

	link = GetUpLink(_topic);
	if (link == NULL)
	{
		try 
		{
			link = CreateUpLink(_topic);	

			up_link_map_[_topic] = link;
			TRACE_INFO("Uplink added : " << _topic << "[" << link << "]");

			if (IsConnected())
			{
				link->Start();
			}
		}
		catch(std::exception& e)
		{
			TRACE_ERROR("Failed to create up link!");
			return	NULL;
		}
	}

	TRACE_INFO("UpLink[" << _topic << "] added.");
	return	link;
}

ServerLinker::UpLink*	ServerLinker::GetUpLink(std::string const& _topic)
{
	std::map<std::string, UpLink*>::iterator it = up_link_map_.find(_topic);
	if (it == up_link_map_.end())
	{
		return	NULL;
	}

	return	it->second;	
}

bool		ServerLinker::DelUpLink(std::string const& _topic)
{
	std::map<std::string, UpLink*>::iterator it = up_link_map_.find(_topic);
	if (it == up_link_map_.end())
	{
		return	false;
	}

	UpLink*	link = it->second;	

	up_link_map_.erase(it);

	delete link;

	return	true;	
}

ServerLinker::DownLink*	ServerLinker::CreateDownLink(std::string const& _topic)
{
	return	new DownLink(this, _topic);	
}

ServerLinker::DownLink*	ServerLinker::AddDownLink(std::string const& _topic)
{
	DownLink*	link = NULL;

	link = ServerLinker::GetDownLink(_topic);
	if (link == NULL)
	{
		try
		{
			link = CreateDownLink(_topic);	

			down_link_map_[_topic] = link;
			TRACE_INFO("Downlink added : " << _topic << "[" << link << "]");
			TRACE_INFO("Downlink Count : " << down_link_map_.size());

			if (IsConnected())
			{
				link->Start();
			}
		}
		catch(std::exception& e)
		{
			TRACE_ERROR("Failed to create down link!");
			return	NULL;
		}
	}

	TRACE_INFO("DownLink[" << _topic << "] added.");
	return	link;
}

bool		ServerLinker::DelDownLink(std::string const& _topic)
{
	std::map<std::string, DownLink*>::iterator it = down_link_map_.find(_topic);
	if (it == down_link_map_.end())
	{
		return	false;
	}
	DownLink*	link = it->second;	

	down_link_map_.erase(it);

	delete link;

	return	true;	
}

ServerLinker::DownLink*	ServerLinker::GetDownLink(std::string const& _topic)
{
	std::map<std::string, DownLink*>::iterator it = down_link_map_.find(_topic);
	if (it == down_link_map_.end())
	{
		return	NULL;
	}

	return	it->second;	
}

void	ServerLinker::Preprocess()
{
	AddUpLink(global_up_topic_);
	AddDownLink(global_down_topic_);

	ProcessObject::Preprocess();
	
}

void	ServerLinker::Process()
{
	ProcessObject::Process();
}

void	ServerLinker::Postprocess()
{
	Disconnect();

	ProcessObject::Preprocess();

}

bool	ServerLinker::Start()
{
	if (broker_.length() == 0)
	{
		return	false;	
	}

	return	ProcessObject::Start();
}

bool	ServerLinker::IsConnected()
{
	return	broker_connected_;
}

bool	ServerLinker::Connect(uint32_t _delay_sec)
{
	return	true;
}

bool	ServerLinker::Disconnect()
{
	return	true;
}

bool	ServerLinker::Send(RCSMessage const& _message)
{
	try
	{
		Post(new Produce(global_up_topic_, _message));
	}
	catch(std::exception& e)
	{
		TRACE_ERROR("Failed to produce![" << e.what() << "]");
		return	false;	
	}

	return	true;
}

bool	ServerLinker::KeepAliveEnable(bool _enable)
{
	keep_alive_enable_ = _enable;

	return	true;
}

bool	ServerLinker::ReportEPData(Endpoint* _ep)
{
	RCSMessage	message(MSG_TYPE_RCS_REPORT);

	time_t	last_time = 0;

	Endpoint::ValueMap value_map;
	_ep->GetUnreportedValueMap(value_map);

	if (value_map.size() == 0)
	{
		return	true;
	}

	JSONNode	node;

	node.push_back(JSONNode(TITLE_NAME_ID, _ep->GetID()));
	node.push_back(JSONNode(TITLE_NAME_COUNT, value_map.size()));

	JSONNode	array(JSON_ARRAY);

	for(Endpoint::ValueMap::iterator it = value_map.begin(); it != value_map.end() ; it++)
	{
		JSONNode	item;
		time_t		time = time_t(it->first);

		item.push_back(JSONNode(TITLE_NAME_TIME, ToString(time)));
		item.push_back(JSONNode(TITLE_NAME_VALUE, it->second));

		array.push_back(item);

		if (last_time < time)
		{
			last_time = time;	
		}
	}
	array.set_name(TITLE_NAME_VALUE);
	node.push_back(array);

	message.AddEPData(node);

	if (!Send(message))
	{
		return	false;
	}


	_ep->SetLastReportTime(last_time);

	return	true;
}

bool	ServerLinker::RequestInit(std::string const& _type, JSONNode& _payload)
{
	JSONNode	payload;

	payload.push_back(JSONNode(TITLE_NAME_MSG_TYPE, _type));
	payload.push_back(JSONNode(TITLE_NAME_TIME, ToString(time_t(Date::GetCurrent()))));

	_payload = payload;

	return	true;
}

bool	ServerLinker::ReplyInit(std::string const& _type, std::string const& _req_id, JSONNode& _payload)
{
	JSONNode	payload;

	payload.push_back(JSONNode(TITLE_NAME_MSG_TYPE, _type));
	payload.push_back(JSONNode(TITLE_NAME_TIME, ToString(time_t(Date::GetCurrent()))));
	payload.push_back(JSONNode(TITLE_NAME_REQ_ID, _req_id));

	_payload = payload;

	return	true;
}

bool	ServerLinker::AddGateway(JSONNode& _payload, Gateway* _gateway, Fields const& _fields)
{
	JSONNode::iterator it = _payload.find(TITLE_NAME_GATEWAY);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	gateway_array(JSON_ARRAY);		

			gateway_array.set_name(TITLE_NAME_GATEWAY);
			gateway_array.push_back(*it);

			JSONNode	properties;
			_gateway->GetProperties(properties, _fields);

			gateway_array.push_back(properties);

			_payload.erase(it);

			_payload.push_back(gateway_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode	properties;

			_gateway->GetProperties(properties, _fields);
			it->push_back(properties);
		}
	}
	else
	{
		JSONNode	properties;
		_gateway->GetProperties(properties, _fields);

		properties.set_name(TITLE_NAME_GATEWAY);
		_payload.push_back(properties);
	}

	return	true;
}

bool	ServerLinker::AddGateway(JSONNode& _payload, std::string const& _id)
{
	JSONNode::iterator it = _payload.find(TITLE_NAME_GATEWAY);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	gateway_array(JSON_ARRAY);		

			gateway_array.set_name(TITLE_NAME_GATEWAY);
			gateway_array.push_back(*it);

			JSONNode	node;
			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			gateway_array.push_back(node);

			_payload.erase(it);

			_payload.push_back(gateway_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode	node;

			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			it->push_back(node);
		}
	}
	else
	{
		JSONNode	node;
		
		node.push_back(JSONNode(TITLE_NAME_ID, _id));	
		node.set_name(TITLE_NAME_GATEWAY);
		_payload.push_back(JSONNode(node));
	}

	return	true;
}

bool	ServerLinker::AddDevice(JSONNode& _payload, Device* _device, Fields const& _fields)
{
	JSONNode::iterator it = _payload.find(TITLE_NAME_DEVICE);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	device_array(JSON_ARRAY);		

			device_array.set_name(TITLE_NAME_DEVICE);
			device_array.push_back(*it);

			JSONNode	properties;
			_device->GetProperties(properties, _fields);

			device_array.push_back(properties);

			_payload.erase(it);

			_payload.push_back(device_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode	properties;
			_device->GetProperties(properties, _fields);

			it->push_back(properties);
		}
	}
	else
	{
		JSONNode	properties;
		_device->GetProperties(properties, _fields);
		properties.set_name(TITLE_NAME_DEVICE);
		_payload.push_back(properties);
	}

	return	true;
}

bool	ServerLinker::AddDevice(JSONNode& _payload, std::string const& _id)
{
	JSONNode::iterator it = _payload.find(TITLE_NAME_DEVICE);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	device_array(JSON_ARRAY);		

			device_array.set_name(TITLE_NAME_DEVICE);
			device_array.push_back(*it);

			JSONNode	node;
			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			device_array.push_back(node);

			_payload.erase(it);

			_payload.push_back(device_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode	node;
			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			it->push_back(node);
		}
	}
	else
	{
		JSONNode	node;
		
		node.push_back(JSONNode(TITLE_NAME_ID, _id));	
		node.set_name(TITLE_NAME_DEVICE);
		_payload.push_back(node);
	}

	return	true;
}

bool	ServerLinker::AddEndpoint(JSONNode& _payload, Endpoint* _endpoint, Fields const& _fields)
{
	JSONNode::iterator it = _payload.find(TITLE_NAME_ENDPOINT);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	endpoint_array(JSON_ARRAY);		
			endpoint_array.set_name(TITLE_NAME_ENDPOINT);
			endpoint_array.push_back(*it);

			JSONNode	properties;
			_endpoint->GetProperties(properties, _fields);
			endpoint_array.push_back(properties);

			_payload.erase(it);

			_payload.push_back(endpoint_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode	properties;
			_endpoint->GetProperties(properties, _fields);

			it->push_back(properties);
		}
	}
	else
	{
		JSONNode	properties;
		_endpoint->GetProperties(properties, _fields);

		properties.set_name(TITLE_NAME_ENDPOINT);
		_payload.push_back(properties);
	}

	return	true;
}

bool	ServerLinker::AddEndpoint(JSONNode& _payload, std::string const& _id)
{
	JSONNode::iterator it = _payload.find(TITLE_NAME_ENDPOINT);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	endpoint_array(JSON_ARRAY);		

			endpoint_array.set_name(TITLE_NAME_ENDPOINT);
			endpoint_array.push_back(*it);

			JSONNode	node;

			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			endpoint_array.push_back(node);

			_payload.erase(it);

			_payload.push_back(endpoint_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			JSONNode node;
			
			node.push_back(JSONNode(TITLE_NAME_ID, _id));
			it->push_back(node);
		}
	}
	else
	{
		JSONNode	node;
		
		node.push_back(JSONNode(TITLE_NAME_ID, _id));	
		node.set_name(TITLE_NAME_ENDPOINT);
		_payload.push_back(node);
	}

	return	true;
}

bool	ServerLinker::AddEPData(JSONNode& _payload, Endpoint* _ep)
{
	JSONNode	node;

	Endpoint::ValueMap value_map;

	TRACE_INFO("Add EP Data!");
	if (!_ep->GetUnreportedValueMap(value_map))
	{
		TRACE_WARN("Failed to get unreported value list.");
		return	false;
	}

	node.push_back(JSONNode(TITLE_NAME_ID, _ep->GetID()));
	node.push_back(JSONNode(TITLE_NAME_COUNT, value_map.size()));

	JSONNode	array(JSON_ARRAY);
	for(Endpoint::ValueMap::iterator it = value_map.begin(); it != value_map.end() ; it++)
	{
		JSONNode	item;
		
		item.push_back(JSONNode(TITLE_NAME_TIME, ToString(time_t(it->first))));
		item.push_back(JSONNode(TITLE_NAME_VALUE, it->second));

		array.push_back(item);
	}

	array.set_name(TITLE_NAME_VALUE);
	node.push_back(array);

	JSONNode::iterator it = _payload.find(TITLE_NAME_DATA);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	endpoint_array(JSON_ARRAY);		

			endpoint_array.set_name(TITLE_NAME_DATA);
			endpoint_array.push_back(*it);
			endpoint_array.push_back(node);

			_payload.erase(it);

			_payload.push_back(endpoint_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			it->push_back(node);
		}
	}
	else
	{
		node.set_name(TITLE_NAME_DATA);
		_payload.push_back(node);
	}

	return	true;	
}

bool	ServerLinker::AddEPData(JSONNode& _payload, Endpoint* _ep, uint32_t _lower_bound, uint32_t _upper_bound)
{
	JSONNode	node;
	Endpoint::ValueMap	value_map;

	if (!_ep->GetUnreportedValueMap(value_map))
	{
		TRACE_WARN("Failed to get unreported value list.");
		return	false;
	}

	node.push_back(JSONNode(TITLE_NAME_ID, _ep->GetID()));
	node.push_back(JSONNode(TITLE_NAME_COUNT, value_map.size()));

	JSONNode	array(JSON_ARRAY);
	for(Endpoint::ValueMap::iterator it = value_map.begin(); it != value_map.end() ; it++)
	{
		JSONNode	item;
		
		item.push_back(JSONNode(TITLE_NAME_TIME, ToString(time_t(it->first))));
		item.push_back(JSONNode(TITLE_NAME_VALUE, it->second));

		array.push_back(item);
	}
	array.set_name(TITLE_NAME_VALUE);

	node.push_back(array);

	JSONNode::iterator it = _payload.find(TITLE_NAME_DATA);
	if (it != _payload.end())
	{
		if (it->type() == JSON_NODE)
		{
			JSONNode	endpoint_array(JSON_ARRAY);		

			endpoint_array.set_name(TITLE_NAME_DATA);
			endpoint_array.push_back(*it);
			endpoint_array.push_back(node);

			_payload.erase(it);

			_payload.push_back(endpoint_array);
		}
		else if (it->type() == JSON_ARRAY)
		{
			it->push_back(node);
		}
	}
	else
	{
		node.set_name(TITLE_NAME_DATA);
		_payload.push_back(node);
	}

	return	true;

}

bool	ServerLinker::Error(std::string const& _req_id ,std::string const& _err_msg)
{
	RCSMessage	message(MSG_TYPE_RCS_ERROR);
	
	message.SetReqID(_req_id);

	return	Send(message);
}

bool	ServerLinker::ConfirmRequest(RCSMessage const& _reply, std::string& _req_type, bool _exception)
{
	bool	ret_value = false;

	request_map_locker_.Lock();

	TRACE_INFO("Server Linker [" << this << "] request_map_ count : " << request_map_.size());

	for(std::map<uint64_t, Produce*>::iterator it = request_map_.begin() ; it != request_map_.end() ; it++)
	{
		Produce*	produce = it->second;
		RCSMessage&	message = produce->GetMessage();
		TRACE_INFO("REQ ID : " << message.GetMsgID() <<  ", Reply ID : " << _reply.GetReqID());

		if (message.GetMsgID() == _reply.GetReqID())
		{
			_req_type = JSONNodeGetMsgType(message.GetPayload());
		
			delete produce;

			request_map_.erase(it);

			ret_value = true;
		}
	}

	request_map_locker_.Unlock();

	if (!ret_value && _exception)
	{
		THROW_REQUEST_TIMEOUT(&_reply);
	}

	return	ret_value;
}


bool	ServerLinker::OnMessage(Message* _message)
{
	std::string	msg_id;
	try
	{
		switch(_message->GetType())
		{
		case	MSG_TYPE_SL_PRODUCE:
			{
				Produce*	produce = dynamic_cast<Produce*>(_message);

				return	OnProduce(produce);
			}
			break;
		
		case	MSG_TYPE_SL_CONSUME:
			{
				Consume*	consume = dynamic_cast<Consume*>(_message);

				OnConsume(consume);
			}
			break;
		}
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
		Error(msg_id, e.what());
	}

	return	true;
}

void	ServerLinker::OnConsume(Consume* _consume)
{
	RCSMessage&	message = _consume->GetMessage();	

	TRACE_INFO("Paylod : " << message.GetPayload().write_formatted());

	if (message.GetMsgType() == MSG_TYPE_RCS_ADD)
	{
		RCSMessage	reply;

		manager_->GetRCSServer().Add(message, reply);	

		Send(reply);
	}
	else if (message.GetMsgType() == MSG_TYPE_RCS_DEL)
	{
		RCSMessage	reply;

		manager_->GetRCSServer().Del(message, reply);	

		Send(reply);
	}
	else if (message.GetMsgType() == MSG_TYPE_RCS_GET)
	{
		RCSMessage	reply;

		manager_->GetRCSServer().Get(message, reply);	

		Send(reply);
	}
	else if (message.GetMsgType() == MSG_TYPE_RCS_SET)
	{
		RCSMessage	reply;

		manager_->GetRCSServer().Set(message, reply);	

		Send(reply);
	}
	else if (message.GetMsgType() == MSG_TYPE_RCS_CONFIRM)
	{
		std::string	req_type;

		if (!ConfirmRequest(message, req_type, false))
		{
			TRACE_ERROR("The confirm message[" << message.GetMsgID() << "] was timed out.");
		//	Error(message.GetMsgID(), "The confirm message was timed out.");
		}
		else
		{
			manager_->GetRCSServer().Confirm(message, req_type);	
		}
	}
	else if (message.GetMsgType() == MSG_TYPE_RCS_ERROR)
	{
		std::string	req_type;

		if (!ConfirmRequest(message, req_type, false))
		{
			manager_->GetRCSServer().Error(message);	
		}
		else
		{
			manager_->GetRCSServer().Error(message, req_type);	
		}
	}
}

bool	ServerLinker::OnProduce(Produce* _produce)
{
	std::string topic	= _produce->GetTopic();
	RCSMessage 	message = _produce->GetMessage();	

	message.Make();

	UpLink* link = GetUpLink(topic);
	if (link != NULL)
	{
		std::string	payload = message.GetPayload().write();
		std::string	msg_id;

		if (!link->Send(payload, msg_id))
		{
			link->IncreaseNumberOfErrorMessages();
			TRACE_ERROR("Failed to produce!");
			return	true;
		}

		TRACE_INFO("  Topic : " << topic);
		TRACE_INFO("Payload : " << message.GetPayload().write_formatted());

		if (message.GetMsgType() != MSG_TYPE_RCS_CONFIRM)
		{
			uint64_t	expire_time = Date::GetCurrent().GetMicroSecond() + (request_timeout_ * TIME_SECOND);

			request_map_locker_.Lock();
			request_map_[expire_time] = _produce;
			request_map_locker_.Unlock();
		//	TRACE_INFO("Produce insert to Message Map[" << msg_id << "]");
		//	message_map_.insert(std::pair<std::string, Produce*>(msg_id, _produce));
		//	TRACE_INFO("Message Map Size : " << message_msp_.size());
		}
		else
		{
			TRACE_INFO("The MsgID of ReqID [" << message.GetReqID() << "] is " << msg_id);
		}

		link->IncreaseNumberOfOutGoingMessages();
		link->Touch();

	}

	return	false;
}
