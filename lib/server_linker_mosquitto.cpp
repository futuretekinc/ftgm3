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
#include "server_linker_mosquitto.h"
#include "rcs_message.h"
#include "object_manager.h"
#include "trace.h"
#include "time2.h"
#include "sha1.h"
#include "md5.h"
#include "sha256.h"
#include "utils.h"

ServerLinkerMosq::Produce::Produce(std::string const& _topic, RCSMessage const& _message)
: Message(MSG_TYPE_SL_PRODUCE), topic_(_topic), message_(_message)
{
}

ServerLinkerMosq::Produce::Produce(std::string const& _topic, std::string const& _payload)
: Message(MSG_TYPE_SL_PRODUCE), topic_(_topic)
{
	if (libjson::is_valid(_payload))
	{
		message_ = libjson::parse(_payload);

		message_.Make();
	}
}

ServerLinkerMosq::Consume::Consume(std::string const& _topic, std::string const& _payload)
: Message(MSG_TYPE_SL_CONSUME), topic_(_topic)
{
	if (libjson::is_valid(_payload))
	{
		message_ = libjson::parse(_payload);

		message_.Make();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinkerMosq::Link
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinkerMosq::Link::Link(ServerLinkerMosq& _linker, std::string const& _topic)
: Object(), linker_(_linker), topic_(_topic), connected_(false)
{
	trace.SetClassName(GetClassName());
	name_ 	= "link";
	enable_ = true;
}

bool	ServerLinkerMosq::Link::Touch()
{
	keep_alive_timer_.Set(Date::GetCurrent() + Time(60000000));

	return	true;
}

uint32_t	ServerLinkerMosq::Link::GetLiveTime()
{
	return	keep_alive_timer_.RemainTime().GetSeconds();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinkerMosq::UpLink
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinkerMosq::UpLink::UpLink(ServerLinkerMosq& _linker, std::string const& _topic)
: Link(_linker, _topic), number_of_out_going_messages_(0), number_of_error_messages_(0), qos_(0)
{
}

bool	ServerLinkerMosq::UpLink::Start()
{
#if 0
	if (topic_ == NULL)
	{
		topic_ = linker_.CreateProducerTopic(topic_, error_string_);
		if (topic_ == NULL)
		{
			TRACE_ERROR("Failed to create topic!");
			return	false;
		}
	}
#endif
	Touch();
	
	TRACE_INFO("Uplink[" << topic_ << "] started");
	return	true;
}

bool	ServerLinkerMosq::UpLink::Stop()
{
#if 0
	if (topic_ != NULL)
	{
		try
		{
			delete topic_;
			topic_ = NULL;
		}
		catch(std::exception& e)
		{
			TRACE_ERROR("Failed to destroy topic!");	
			return	false;
		}
	}
#endif
	return	true;
}

bool	ServerLinkerMosq::UpLink::SetQoS(uint32_t _qos)
{
	qos_ = _qos;

	return	true;
}

bool	ServerLinkerMosq::UpLink::Send(std::string const& _message)
{
#if 0
	if (topic_ == NULL)
	{
		if (!Start())
		{
			TRACE_ERROR("Failed to send mesage!");
			return	false;
		}
	}
#endif
//	TRACE_INFO("Send(" << topic_->name() << ", " << partition_ << ", " << _message << ")");
//	linker_.Send(_message);

	return	true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinkerMosq::DownLink
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinkerMosq::DownLink::DownLink(ServerLinkerMosq& _linker, std::string const& _topic)
: Link(_linker, _topic), number_of_incomming_messages_(0), number_of_error_messages_(0)
{
}

bool	ServerLinkerMosq::DownLink::Start()
{
	if (mosquitto_subscribe(linker_.mosquitto_, NULL, topic_.c_str(), 0) > 0)
	{
		TRACE_ERROR("Subscribe error : " << topic_);	
	}
	else
	{
		TRACE_INFO("Subscribe[" << topic_ << "] connected!!!");
		connected_ = true;
	}

	return	true;
}

bool	ServerLinkerMosq::DownLink::Stop()
{
	TRACE_INFO("Subscribe[" << topic_ << "] disconnected!!!");
	connected_ = false;

	return	true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinkerMosq
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinkerMosq::ServerLinkerMosq(ObjectManager* _manager)
: 	ProcessObject(), 
	manager_(_manager), 
	broker_connected_(false),
	auto_connection_(true),
	keep_alive_enable_(true),
	secret_key_(""),
	report_late_arrive_message_(SERVER_LINKER_REPORT_LATE_ARRIVE_MESSAGE),
	request_timeout_(SERVER_LINKER_REQUEST_TIMEOUT_SEC),
	broker_retry_interval_(SERVER_LINKER_CONNECTION_RETRY_INTERVAL_SEC * TIME_SECOND),
	protocol_version_(MQTT_PROTOCOL_V31),
	topic_version_(SERVER_LINKER_TOPIC_VERSION),
	topic_id_(SERVER_LINKER_TOPIC_ID),
	global_up_name_(SERVER_LINKER_TOPIC_GLOBAL_UP_NAME),
	global_down_name_(SERVER_LINKER_TOPIC_GLOBAL_DOWN_NAME)
{
	trace.SetClassName(GetClassName());
	name_ 	= "linker";
	enable_ = true;

	mosquitto_lib_init();

	mosquitto_ = mosquitto_new(manager_->GetID().c_str(), true, (void *)this);

	SetGlobalUpTopic(topic_version_,  global_up_name_, topic_id_);
	SetGlobalDownTopic(topic_version_,global_up_name_, topic_id_);

	mosquitto_max_inflight_messages_set(mosquitto_, 20);
	mosquitto_opts_set(mosquitto_, MOSQ_OPT_PROTOCOL_VERSION, (void *)&protocol_version_);
	mosquitto_log_callback_set(mosquitto_, OnLogCB);
	mosquitto_publish_callback_set(mosquitto_, OnPublishCB);
	mosquitto_subscribe_callback_set(mosquitto_, OnSubscribeCB);
	mosquitto_connect_callback_set(mosquitto_, OnConnectCB);
	mosquitto_disconnect_callback_set(mosquitto_, OnDisconnectCB);
	mosquitto_message_callback_set(mosquitto_, OnMessageCB);

	SetHashAlg(DEFAULT_CONST_SERVER_LINKER_HASH);

}

ServerLinkerMosq::~ServerLinkerMosq()
{
	for(std::map<std::string, UpLink*>::iterator it = up_link_map_.begin() ; it != up_link_map_.end() ; it++)
	{
		delete it->second;	
	}

	for(std::map<std::string, DownLink*>::iterator it = down_link_map_.begin() ; it != down_link_map_.end() ; it++)
	{
		delete it->second;	
	}

	if (mosquitto_)
	{
		mosquitto_destroy(mosquitto_);
	}
}

bool	ServerLinkerMosq::SetProperty(JSONNode const& _config, bool _check)
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

ServerLinkerMosq::operator JSONNode() const
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

bool	ServerLinkerMosq::SetHashAlg(std::string const& _name, bool _check)
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

bool	ServerLinkerMosq::SetTopicVersion(std::string const& _version, bool _check)
{
	if (!_check)
	{
		topic_version_ = _version;
		SetGlobalUpTopic(topic_version_,  global_up_name_, topic_id_);
		SetGlobalDownTopic(topic_version_,  global_down_name_, topic_id_);
	}

	return	true;
}

const std::string&	ServerLinkerMosq::GetTopicVersion()
{
	return	topic_version_;
}

bool	ServerLinkerMosq::SetTopicID(std::string const& _id, bool _check)
{
	if (!_check)
	{
		topic_id_ = _id;
		SetGlobalUpTopic(topic_version_,  global_up_name_, topic_id_);
		SetGlobalDownTopic(topic_version_,  global_down_name_, topic_id_);
	}

	return	true;
}

const std::string&	ServerLinkerMosq::GetTopicID()
{
	return	topic_id_;
}

bool	ServerLinkerMosq::SetGlobalUpName(std::string const& _name, bool _check)
{
	if (!_check)
	{
		global_up_name_ = _name;
		SetGlobalUpTopic(topic_version_,  global_up_name_, topic_id_);
	}

	return	true;
}

const std::string&	ServerLinkerMosq::GetGlobalUpName()
{
	return	global_up_name_;
}

bool	ServerLinkerMosq::SetGlobalDownName(std::string const& _name, bool _check)
{
	if (!_check)
	{
		global_down_name_ = _name;
		SetGlobalDownTopic(topic_version_,  global_down_name_, topic_id_);
	}

	return	true;
}

const std::string&	ServerLinkerMosq::GetGlobalDownName()
{
	return	global_down_name_;
}

bool	ServerLinkerMosq::SetGlobalUpTopic(std::string const& _version, std::string const& _name, std::string const& _id, bool _check)
{
	if (!_check)
	{
		global_up_topic_ = _version + "/" + _name + "/" + _id;
	}

	return	true;
}

const std::string&	ServerLinkerMosq::GetGlobalUpTopic()
{
	return	global_up_topic_;
}

bool	ServerLinkerMosq::SetGlobalDownTopic(std::string const& _version, std::string const& _name, std::string const& _id, bool _check)
{
	if (!_check)
	{
		global_down_topic_ = _version + "/" + _name + "/" + _id;
	}

	return	true;
}

const std::string&	ServerLinkerMosq::GetGlobalDownTopic()
{
	return	global_down_topic_;
}

bool	ServerLinkerMosq::SetSecretKey(std::string const& _secret_key, bool _check)
{
	if (!_check)
	{
		secret_key_=  _secret_key;
	}

	return	true;
}

bool	ServerLinkerMosq::GetSecretKey(std::string & _secret_key)
{
	_secret_key = secret_key_;

	return	true;
}

bool	ServerLinkerMosq::SetBroker(std::string const& _broker, bool _check)
{
	if (!_check)
	{
		broker_ = _broker;
	}

	return	true;
}

const std::string&	ServerLinkerMosq::GetBroker()
{
	return	broker_;
}

bool		ServerLinkerMosq::SetAutoConnection(bool _auto)
{
	auto_connection_ = _auto;

	return	true;
}

bool		ServerLinkerMosq::SetAutoConnection(std::string const& _auto, bool _check)
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


uint32_t	ServerLinkerMosq::GetUpLink(std::vector<UpLink*>& _link_list)
{
	for(std::map<std::string, UpLink*>::iterator it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
	{
		_link_list.push_back(it->second);		
	}

	return	_link_list.size();
}

uint32_t	ServerLinkerMosq::GetUpLink(std::list<std::string>& _topic_list)
{
	for(std::map<std::string, UpLink*>::iterator it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
	{
		_topic_list.push_back(it->first);		
	}

	return	_topic_list.size();
}

uint32_t	ServerLinkerMosq::GetDownLink(std::vector<DownLink*>& _link_list)
{
	for(std::map<std::string, DownLink*>::iterator it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
	{
		_link_list.push_back(it->second);		
	}

	return	_link_list.size();
}

uint32_t	ServerLinkerMosq::GetDownLink(std::list<std::string>& _topic_list)
{
	for(std::map<std::string, DownLink*>::iterator it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
	{
		_topic_list.push_back(it->first);		
	}

	return	_topic_list.size();
}

ServerLinkerMosq::UpLink*	ServerLinkerMosq::AddUpLink(std::string const& _topic)
{
	UpLink*	link = NULL;

	link = GetUpLink(_topic);
	if (link == NULL)
	{
		try 
		{
			link = new UpLink(*this, _topic);	

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

ServerLinkerMosq::UpLink*	ServerLinkerMosq::GetUpLink(std::string const& _topic)
{
	std::map<std::string, UpLink*>::iterator it = up_link_map_.find(_topic);
	if (it == up_link_map_.end())
	{
		return	NULL;
	}

	return	it->second;	
}

bool		ServerLinkerMosq::DelUpLink(std::string const& _topic)
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

ServerLinkerMosq::DownLink*	ServerLinkerMosq::AddDownLink(std::string const& _topic)
{
	DownLink*	link = NULL;

	link = ServerLinkerMosq::GetDownLink(_topic);
	if (link == NULL)
	{
		try
		{
			link = new DownLink(*this, _topic);	

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

#if 0
	if (up_link_map_.size() == 0)
	{
		AddUpLink(topic_);
	}
#endif
	TRACE_INFO("DownLink[" << _topic << "] added.");
	return	link;
}

bool		ServerLinkerMosq::DelDownLink(std::string const& _topic)
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

ServerLinkerMosq::DownLink*	ServerLinkerMosq::GetDownLink(std::string const& _topic)
{
	std::map<std::string, DownLink*>::iterator it = down_link_map_.find(_topic);
	if (it == down_link_map_.end())
	{
		return	NULL;
	}

	return	it->second;	
}

void	ServerLinkerMosq::Preprocess()
{
#if 0
	RdKafka::Conf::ConfResult result = conf_global_->set("metadata.broker.list", broker_, error_string_);
	if (result != RdKafka::Conf::CONF_OK)
	{
		TRACE_ERROR("Failed to set broker to conf_global!");
	}

	conf_global_->set("dr_cb", &delivery_report_cb_, error_string_);
	conf_global_->set("event_cb", &event_cb_, error_string_);
#endif
	AddUpLink(global_up_topic_);
	AddDownLink(global_down_topic_);

	ProcessObject::Preprocess();
	
}

void	ServerLinkerMosq::Process()
{
	if (IsConnected())
	{
		try
		{
//			producer_->poll(0);

			for(std::map<std::string, DownLink*>::iterator it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
			{
//				it->second->Consume(&consume_cb_);
			}
		}
		catch(std::exception& e)
		{
			TRACE_ERROR("Connection error![" << e.what() << "]");	
		}
	}
	else
	{
		if (auto_connection_ && !broker_connected_)
		{
			InternalConnect();
		}
	}

	
	uint64_t	current = uint64_t(Date::GetCurrent().GetMicroSecond());

	if (request_map_.upper_bound(current) != request_map_.begin())
	{
		for(std::map<uint64_t, Produce*>::iterator it = request_map_.begin(); it != request_map_.upper_bound(current) ; it++)
		{
			if (it->second->GetMessage().GetMsgType() != MSG_STR_KEEP_ALIVE)
			{
				TRACE_ERROR("Requst Timeout : " <<  it->second->GetMessage().GetMsgID());
				TRACE_ERROR("Msg Type : " <<  it->second->GetMessage().GetMsgType());
			}
			delete it->second;
			request_map_.erase(it);
		}
	}

	ProcessObject::Process();
}

void	ServerLinkerMosq::Postprocess()
{
	Disconnect();

	ProcessObject::Preprocess();

}

bool	ServerLinkerMosq::Start()
{
	if (broker_.length() == 0)
	{
		return	false;	
	}

	return	ProcessObject::Start();
}

bool	ServerLinkerMosq::IsConnected()
{
	return	broker_connected_;
}

bool	ServerLinkerMosq::Connect(uint32_t _delay_sec)
{
	if (!auto_connection_)
	{
		auto_connection_ = true;

		if (!IsConnected())
		{
			InternalConnect(_delay_sec);
		}
	}

	return	true;
}

bool	ServerLinkerMosq::InternalConnect(uint32_t _delay_sec)
{
	try
	{
		if (_delay_sec != 0)
		{
			Date date = Date::GetCurrent() + Time(_delay_sec * TIME_SECOND);
			broker_retry_timeout_.Set(date);
		}
		else if (broker_retry_timeout_.RemainTime() == 0)
		{
			int	ret = mosquitto_connect(mosquitto_, broker_.c_str(), 1883, keep_alive_interval_);
			if (ret > 0)
			{
				TRACE_ERROR("Failed to connect to broker[" << broker_ << "] : " << ret);
				THROW_CONNECT_TIMEOUT(mosquitto_strerror(ret));
			}

			mosquitto_loop_start(mosquitto_);
			Date date = Date::GetCurrent() + broker_retry_interval_;
			broker_retry_timeout_.Set(date);

			TRACE_INFO("Try Connect to broker.");
		}
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
		Date date = Date::GetCurrent() + broker_retry_interval_;
		broker_retry_timeout_.Set(date);

		InternalDisconnect();
	}

	auto_connection_ = true;
	return	true;
}

bool	ServerLinkerMosq::Disconnect()
{
	if (auto_connection_)
	{
		if (broker_connected_)
		{
			InternalDisconnect();
		}

		auto_connection_ = false;
	}

	return	true;
}

bool	ServerLinkerMosq::InternalDisconnect()
{
	mosquitto_loop_stop(mosquitto_, true);

	broker_connected_ = false;

	for(std::map<std::string, UpLink*>::iterator it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
	{
		it->second->Stop();
	}

	for(std::map<std::string, DownLink*>::iterator it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
	{
		it->second->Stop();
	}
#if 0
	if (producer_ != NULL)
	{
		delete producer_;	
		producer_ = NULL;	
	}

	if (consumer_ != NULL)
	{
		delete consumer_;
		consumer_ = NULL;
	}

#endif
	return	true;
}

bool	ServerLinkerMosq::Send(RCSMessage const& _message)
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

bool	ServerLinkerMosq::KeepAliveEnable(bool _enable)
{
	keep_alive_enable_ = _enable;

	return	true;
}

bool	ServerLinkerMosq::ReportEPData(Endpoint* _ep)
{
	RCSMessage	message(MSG_STR_REPORT);

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

bool	ServerLinkerMosq::RequestInit(std::string const& _type, JSONNode& _payload)
{
	JSONNode	payload;

	payload.push_back(JSONNode(TITLE_NAME_MSG_TYPE, _type));
	payload.push_back(JSONNode(TITLE_NAME_TIME, ToString(time_t(Date::GetCurrent()))));

	_payload = payload;

	return	true;
}

bool	ServerLinkerMosq::ReplyInit(std::string const& _type, std::string const& _req_id, JSONNode& _payload)
{
	JSONNode	payload;

	payload.push_back(JSONNode(TITLE_NAME_MSG_TYPE, _type));
	payload.push_back(JSONNode(TITLE_NAME_TIME, ToString(time_t(Date::GetCurrent()))));
	payload.push_back(JSONNode(TITLE_NAME_REQ_ID, _req_id));

	_payload = payload;

	return	true;
}

bool	ServerLinkerMosq::AddGateway(JSONNode& _payload, Gateway* _gateway, Fields const& _fields)
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

bool	ServerLinkerMosq::AddGateway(JSONNode& _payload, std::string const& _id)
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

bool	ServerLinkerMosq::AddDevice(JSONNode& _payload, Device* _device, Fields const& _fields)
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

bool	ServerLinkerMosq::AddDevice(JSONNode& _payload, std::string const& _id)
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

bool	ServerLinkerMosq::AddEndpoint(JSONNode& _payload, Endpoint* _endpoint, Fields const& _fields)
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

bool	ServerLinkerMosq::AddEndpoint(JSONNode& _payload, std::string const& _id)
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

bool	ServerLinkerMosq::AddEPData(JSONNode& _payload, Endpoint* _ep)
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

bool	ServerLinkerMosq::AddEPData(JSONNode& _payload, Endpoint* _ep, uint32_t _lower_bound, uint32_t _upper_bound)
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

bool	ServerLinkerMosq::Error(std::string const& _req_id ,std::string const& _err_msg)
{
	RCSMessage	message(MSG_STR_ERROR);
	
	message.SetReqID(_req_id);

	return	Send(message);
}

bool	ServerLinkerMosq::ConfirmRequest(RCSMessage* _reply, std::string& _req_type, bool _exception)
{
	for(std::map<uint64_t, Produce*>::iterator it = request_map_.begin() ; it != request_map_.end() ; it++)
	{
		Produce*	produce = it->second;
		RCSMessage&	message = produce->GetMessage();

		TRACE_INFO("Msg ID : " << message.GetMsgID() << " - Req ID : " << _reply->GetReqID())
		if (message.GetMsgID() == _reply->GetReqID())
		{
			_req_type = JSONNodeGetMsgType(message.GetPayload());
		
			delete produce;

			request_map_.erase(it);

			return	true;
		}
	}

	if (_exception)
	{
		THROW_REQUEST_TIMEOUT(_reply);
	}

	return	false;
}


bool	ServerLinkerMosq::OnMessage(Message* _message)
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

void	ServerLinkerMosq::OnConsume(Consume* _consume)
{
	RCSMessage&	message = _consume->GetMessage();	

	TRACE_INFO("Paylod : " << message.GetPayload().write_formatted());

	if (message.GetMsgType() == MSG_STR_ADD)
	{
		RCSMessage	reply;

		manager_->GetRCSServer().Add(message, reply);	

		Send(reply);
	}
	else if (message.GetMsgType() == MSG_STR_DEL)
	{
		RCSMessage	reply;

		manager_->GetRCSServer().Del(message, reply);	

		Send(reply);
	}
	else if (message.GetMsgType() == MSG_STR_GET)
	{
		RCSMessage	reply;

		manager_->GetRCSServer().Get(message, reply);	

		Send(reply);
	}
	else if (message.GetMsgType() == MSG_STR_SET)
	{
		RCSMessage	reply;

		manager_->GetRCSServer().Set(message, reply);	

		Send(reply);
	}
	else if (message.GetMsgType() == MSG_STR_CONFIRM)
	{
		std::string	req_type;

		if (!ConfirmRequest(&message, req_type, false))
		{
			Error(message.GetMsgID(), "");
		}

		manager_->GetRCSServer().Confirm(message, req_type);	
	}
	else if (message.GetMsgType() == MSG_STR_ERROR)
	{
		std::string	req_type;

		if (!ConfirmRequest(&message, req_type, false))
		{
			manager_->GetRCSServer().Error(message);	
		}
		else
		{
			manager_->GetRCSServer().Error(message, req_type);	
		}
	}
}

bool	ServerLinkerMosq::OnProduce(Produce* _produce)
{
	std::string topic	= _produce->GetTopic();
	RCSMessage&	message = _produce->GetMessage();	

	message.Make();

	UpLink* link = GetUpLink(topic);
	if (link != NULL)
	{
		int	msg_id;
		std::string	payload = message.GetPayload().write();

		int ret = mosquitto_publish(mosquitto_, &msg_id, link->GetTopic().c_str(), payload.size(), payload.c_str(), link->GetQoS(), 0);
		if (ret != 0)
		{
			link->IncreaseNumberOfErrorMessages();
			TRACE_ERROR("Failed to produce : " << ret);
			return	true;
		}

		if (message.GetMsgType() != MSG_TYPE_RCS_CONFIRM)
		{
			message_map_.insert(std::pair<int, Produce*>(msg_id, _produce));
		}

		link->IncreaseNumberOfOutGoingMessages();
		link->Touch();

		TRACE_INFO("  Topic : " << topic <<"[" << link->GetQoS() << "]");
		TRACE_INFO("Payload : " << message.GetPayload().write_formatted());
	}

	return	false;
}

void ServerLinkerMosq::OnConnectCB(struct mosquitto *mosq, void *obj, int result)
{
	ServerLinkerMosq*	linker = (ServerLinkerMosq*)obj;

	TRACE_INFO2(linker, "Connected to broker[!" << linker->GetBroker() << "]");
	linker->broker_connected_ = true;
	TRACE_INFO2(linker, "Up Link Count : " << linker->up_link_map_.size());
	for(std::map<std::string, UpLink*>::iterator it = linker->up_link_map_.begin(); it != linker->up_link_map_.end() ; it++)
	{
		it->second->Start();
	}

	TRACE_INFO2(linker, "Down Link Count : " << linker->down_link_map_.size());
	for(std::map<std::string, DownLink*>::iterator it = linker->down_link_map_.begin() ; it != linker->down_link_map_.end() ; it++)
	{
		it->second->Start();
	}
}

void ServerLinkerMosq::OnDisconnectCB(struct mosquitto *_mosq, void *_obj, int _rc)
{
	ServerLinkerMosq*	linker = (ServerLinkerMosq*)_obj;

	TRACE_INFO2(linker, "Disconnected from broker[" << linker->GetBroker() << "]");
	for(std::map<std::string, UpLink*>::iterator it = linker->up_link_map_.begin(); it != linker->up_link_map_.end() ; it++)
	{
		it->second->Stop();
	}

	for(std::map<std::string, DownLink*>::iterator it = linker->down_link_map_.begin() ; it != linker->down_link_map_.end() ; it++)
	{
		it->second->Stop();
	}

	linker->broker_connected_ = false;
}

void ServerLinkerMosq::OnPublishCB(struct mosquitto *_mosq, void *_obj, int _mid)
{
	ServerLinkerMosq*	linker = (ServerLinkerMosq*)_obj;

	TRACE_INFO2(linker, "The Message[" << _mid << "] was delivered");

	std::map<int, Produce*>::iterator it = linker->message_map_.find(_mid);
	if (it != linker->message_map_.end())
	{
		Produce*	produce = it->second;

		uint64_t	expire_time = produce->GetMessage().GetTime().GetMicroSecond() + (linker->request_timeout_ * TIME_SECOND);

		TRACE_INFO2(linker, "The request expiry time[" << expire_time << "] has been set in the request message[" << _mid << "].");
		linker->request_map_[expire_time] = produce;
		linker->message_map_.erase(it);
	}
	else
	{
		TRACE_INFO2(linker, "Tth publish[" << _mid << "] not found!");	
	}
}

void	ServerLinkerMosq::OnLogCB(struct mosquitto *_mosq, void *_obj, int _level, const char *_str)
{
	ServerLinkerMosq*	linker = (ServerLinkerMosq*)_obj;

	TRACE_INFO2(linker, "Log : " << _str);
}

void	ServerLinkerMosq::OnSubscribeCB(struct mosquitto *_mosq, void *_obj, int mid, int qos_count, const int *granted_qos)
{
	ServerLinkerMosq*	linker = (ServerLinkerMosq*)_obj;

	TRACE_INFO2(linker, "Subscribe : " << mid);
}

void	ServerLinkerMosq::OnMessageCB(struct mosquitto *_mosq, void *_obj, const struct mosquitto_message *_message)
{
	ServerLinkerMosq*	linker = (ServerLinkerMosq *)_obj;
	DownLink*	link = linker->GetDownLink(_message->topic);

	TRACE_INFO2(linker, "Message Received : " << _message->topic);
	if (link != NULL)
	{
		link->IncreaseNumberOfIncommingMessages();
		link->Touch();

		TRACE_INFO2(linker, _message->topic << " : " << _message->retain);
		if (_message->payloadlen != 0)
		{
			std::string	payload = (char *)_message->payload;

			size_t	close_brace_pos = payload.rfind('}');

			if (close_brace_pos == std::string::npos)
			{
				link->IncreaseNumberOfErrorMessages();
				TRACE_ERROR2(linker, "Invalid payload : " << payload);
			}
			else
			{
				payload = payload.substr(0, close_brace_pos+1);

				try
				{
					Consume	*consume = new Consume(_message->topic, payload);
					linker->Post(consume);
				}
				catch(std::exception& e)
				{
					link->IncreaseNumberOfErrorMessages();
					TRACE_ERROR2(linker, "Exception : " << e.what());
					TRACE_ERROR2(linker, "Invalid payload : " << payload);
				}
			}
		}
	}
	else
	{
		TRACE_ERROR2(NULL, "Down link unknown!");	
	}
}
