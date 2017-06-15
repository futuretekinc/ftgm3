#include <exception>
#include <list>
#include <string>
#include <iomanip>
#include "node.h"
#include "defined.h"
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
: Message(MSG_TYPE_SL_PRODUCE), topic_(_topic), message_(_message)
{
}

ServerLinker::Produce::Produce(std::string const& _topic, std::string const& _payload)
: Message(MSG_TYPE_SL_PRODUCE), topic_(_topic)
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

ServerLinker::SLMError::SLMError()
{
}

const char* ServerLinker::SLMError::what() const throw()
{
	return	message_.c_str();
}

ServerLinker::SLMInvalidArgument::SLMInvalidArgument(std::string const& _argument)
: SLMError()
{
	std::ostringstream	oss;

	oss << "Invalid argument[" << _argument << "]";

	message_ = oss.str();
}

ServerLinker::SLMObjectNotFound::SLMObjectNotFound(std::string const& _object_id)
: SLMError(), object_id_(_object_id)
{
	std::ostringstream	oss;

	oss << "Object[" << _object_id << "] not found!";

	message_ = oss.str();
}

ServerLinker::SLMRequestTimeout::SLMRequestTimeout(RCSMessage* _message)
: SLMError()
{ 
	std::ostringstream	oss;
	oss << "Message[" << _message->GetMsgID() <<"] arrives late or is invalid.";

	message_ = oss.str();
}

ServerLinker::SLMNotInitialized::SLMNotInitialized(std::string const& _message)
: SLMError()
{
	message_ = _message;
}

ServerLinker::EventCB::EventCB(ServerLinker& _linker)
: Object(), RdKafka::EventCb(), linker_(_linker)
{
	trace.SetClassName(GetClassName());
	name_ 	= "event";
	enable_ = true;
}

void	ServerLinker::EventCB::event_cb(RdKafka::Event &event) 
{
	switch (event.type())
	{   
	case RdKafka::Event::EVENT_ERROR:
		{
			TRACE_INFO("ERROR (" << RdKafka::err2str(event.err()) << "): " << event.str());
			if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
			{
				linker_.InternalDisconnect();
				linker_.InternalConnect();
			}
		}
		break;

	case RdKafka::Event::EVENT_STATS:
		{
			TRACE_INFO("STATS : " << event.str());
		}
		break;

	case RdKafka::Event::EVENT_LOG:
		{
			TRACE_INFO("LOG-" << event.severity() << "-" << event.fac().c_str() << " : " << event.str().c_str());
		}
		break;

	default:
		{
			TRACE_INFO("EVENT : " << event.type() << "(" << RdKafka::err2str(event.err()) << ") : " << event.str());
		}
		break;
	}   
}

ServerLinker::DeliveryReportCB::DeliveryReportCB(ServerLinker& _linker)
: Object(), RdKafka::DeliveryReportCb(), linker_(_linker)
{
	trace.SetClassName(GetClassName());
	name_ 	= "delivery_report";
	enable_ = true;
}

void ServerLinker::DeliveryReportCB::dr_cb (RdKafka::Message &data) 
{

	Produce *produce = (Produce*) data.msg_opaque();
	if (produce!= NULL)
	{
		std::string	topic = produce->GetTopic();
		RCSMessage&	message = produce->GetMessage();

		auto it = linker_.message_map_.find(message.GetMsgID());
		if (it != linker_.message_map_.end())
		{
			TRACE_INFO("The Message[" << message.GetMsgID() << "] was delivered");

			uint64_t	expire_time = message.GetTime().GetMicroSecond() + (linker_.request_timeout_ * TIME_SECOND);

			TRACE_INFO("The request expiry time[" << expire_time << "] has been set in the request message[" << message.GetMsgID() << "].");
			linker_.request_map_[expire_time] = produce;
			linker_.message_map_.erase(it);
		}

	}

	if (data.key())
	{
		TRACE_INFO("Key: " << *(data.key()));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker::Link
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinker::Link::Link(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition)
: Object(), linker_(_linker), partition_(_partition), topic_name_(_topic_name), topic_(NULL)
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
ServerLinker::UpLink::UpLink(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition)
: Link(_linker, _topic_name, _partition), number_of_out_going_messages_(0), number_of_error_messages_(0)
{
}

bool	ServerLinker::UpLink::Start()
{
	if (topic_ == NULL)
	{
		topic_ = linker_.CreateProducerTopic(topic_name_, error_string_);
		if (topic_ == NULL)
		{
			TRACE_ERROR("Failed to create topic!");
			return	false;
		}
	}

	Touch();
	
	TRACE_INFO("Uplink[" << topic_name_ << "] started");
	return	true;
}

bool	ServerLinker::UpLink::Stop()
{
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

	return	true;
}

bool	ServerLinker::UpLink::Send(std::string const& _message)
{
	if (topic_ == NULL)
	{
		if (!Start())
		{
			TRACE_ERROR("Failed to send mesage!");
			return	false;
		}
	}

//	TRACE_INFO("Send(" << topic_->name() << ", " << partition_ << ", " << _message << ")");
//	linker_.Send(_message);

	return	true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker::DownLink::ConsumCB
/////////////////////////////////////////////////////////////////////////////////////////////
void	ServerLinker::DownLink::ConsumeCB::consume_cb(RdKafka::Message& _msg, void *opaque)
{
	if (_msg.err() == RdKafka::ERR_NO_ERROR)
	{
		link_.SetOffset(_msg.offset());

		TRACE_INFO2(&link_, _msg.topic_name() << " : " << _msg.offset());
		if (_msg.len() != 0)
		{
			TRACE_INFO2(&link_, "Payload : " << (char *)_msg.payload());
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker::DownLink
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinker::DownLink::DownLink(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition)
: Link(_linker, _topic_name, _partition), message_cb_(*this), offset_(RdKafka::Topic::OFFSET_END), number_of_incomming_messages_(0), number_of_error_messages_(0)
{
}

bool	ServerLinker::DownLink::Start()
{
	if (topic_ == NULL)
	{
		topic_ = linker_.CreateConsumerTopic(topic_name_, error_string_);
		if (topic_ == NULL)
		{
			TRACE_ERROR("Failed to create topic!");
			return	false;
		}
	}

	RdKafka::ErrorCode error_code = linker_.GetConsumer()->start(topic_, partition_, offset_);
	if (error_code != RdKafka::ErrorCode::ERR_NO_ERROR)
	{
		TRACE_ERROR("Failed to start down link[" << error_code << "]!");
		return	false;	
	}

	Touch();

	return	true;
}

bool	ServerLinker::DownLink::Stop()
{
	if (topic_ != NULL)
	{
		RdKafka::ErrorCode	error_code = linker_.GetConsumer()->stop(topic_, partition_);
		if (error_code != RdKafka::ErrorCode::ERR_NO_ERROR)
		{
			TRACE_ERROR("Failed to stop down link!");
			return	false;
		}

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

	return	true;
}

bool	ServerLinker::DownLink::Consume(RdKafka::ConsumeCb* _consum_cb)
{
	if ((linker_.GetConsumer() != NULL) && (topic_ != NULL))
	{
		if (linker_.GetConsumer()->consume_callback(topic_, partition_, 1, _consum_cb, this) > 0)
		{
			return	true;	
		}
	}

	return	false;	
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker::ConsumCB
/////////////////////////////////////////////////////////////////////////////////////////////
void	ServerLinker::ConsumeCB::consume_cb(RdKafka::Message& _msg, void *opaque)
{
	DownLink*	link = (DownLink *)opaque;
	
	if (link != NULL)
	{
		if (_msg.err() == RdKafka::ERR_NO_ERROR)
		{
			link->SetOffset(_msg.offset());
			link->IncreaseNumberOfIncommingMessages();
			link->Touch();

			TRACE_INFO2(&linker_, _msg.topic_name() << " : " << _msg.offset());
			if (_msg.len() != 0)
			{
				std::string	payload = (char *)_msg.payload();

				size_t	close_brace_pos = payload.rfind('}');

				if (close_brace_pos == std::string::npos)
				{
					link->IncreaseNumberOfErrorMessages();
					TRACE_ERROR2(&linker_, "Invalid payload : " << payload);
				}
				else
				{
					payload = payload.substr(0, close_brace_pos+1);

					try
					{
						Consume	*consume = new Consume(_msg.topic_name(), payload);
						linker_.Post(consume);
					}
					catch(std::exception& e)
					{
						link->IncreaseNumberOfErrorMessages();
						TRACE_ERROR2(&linker_, "Exception : " << e.what());
						TRACE_ERROR2(&linker_, "Invalid payload : " << payload);
					}
				}
			}
		}
		else
		{
			if (_msg.err() != RdKafka::ERR__PARTITION_EOF)
			{
				TRACE_ERROR2(NULL, "Topic : " << _msg.topic_name());
				TRACE_ERROR2(NULL, "Error : " << RdKafka::err2str(_msg.err()));
			}
		}
	}
	else
	{
		TRACE_ERROR2(NULL, "Down link unknown!");	
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinker::ServerLinker(ObjectManager* _manager)
: 	ActiveObject(), 
	manager_(_manager), 
	event_cb_(*this), 
	delivery_report_cb_(*this), 
	consume_cb_(*this),
	consumer_(NULL), 
	producer_(NULL), 
	broker_connected_(false),
	auto_connection_(true),
	keep_alive_enable_(true),
	report_late_arrive_message_(SERVER_LNKER_REPORT_LATE_ARRIVE_MESSAGE),
	request_timeout_(SERVER_LINKER_REQUEST_TIMEOUT_SEC),
	broker_retry_interval_(SERVER_LINKER_CONNECTION_RETRY_INTERVAL_SEC * TIME_SECOND)
{
	trace.SetClassName(GetClassName());
	name_ 	= "linker";
	enable_ = true;
	global_up_topic_  = "v1_server_1";
	global_down_topic_  = "v1_client_1";
	secret_code_hash_ = sha1;

	conf_global_= RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
	conf_topic_ = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
}

ServerLinker::~ServerLinker()
{
	delete conf_global_;
	delete conf_topic_;

	for(auto it = up_link_map_.begin() ; it != up_link_map_.end() ; it++)
	{
		delete it->second;	
	}

	for(auto it = down_link_map_.begin() ; it != down_link_map_.end() ; it++)
	{
		delete it->second;	
	}
}

bool	ServerLinker::Load(JSONNode const& _json)
{
	bool	ret_value = true;

	if (_json.name() == TITLE_NAME_SECRET)
	{
		if (_json.type() == JSON_STRING)
		{
			secret_code_ = _json.as_string();	
		}
		else
		{
			TRACE_ERROR("Invalid secure code!");
		}
	}
	else if (_json.name() == TITLE_NAME_HASH)
	{
		if (_json.type() == JSON_STRING)
		{
			if (_json.as_string() == "md5")
			{
				secret_code_hash_ = md5;	
			}
			else if (_json.as_string() == "sha1")
			{
				secret_code_hash_ = sha1;	
			}
			else if (_json.as_string() == "sha256")
			{
				secret_code_hash_ = sha256;	
			}
			else
			{
				TRACE_ERROR("Not supported hash algorithm[" << _json.as_string() << "]");
			}
		}
		else
		{
			TRACE_ERROR("Invalid secure code!");
		}
	
	}
	else if ((_json.name() == TITLE_NAME_SERVER_LINKER) || (_json.name().size() == 0))
	{
		if (_json.type() == JSON_NODE)
		{
			for(auto it = _json.begin(); it != _json.end() ; it++)
			{
				ret_value = Load(*it);
				if (!ret_value)
				{
					std::cout << "Invalid format" << std::endl;
				}
			}
		}
	}
	else if (_json.name() == TITLE_NAME_BROKER)
	{
		if (_json.type() == JSON_STRING)
		{
			SetBroker(_json.as_string());
		}
		else
		{
			TRACE_ERROR("Invalid json format");
			ret_value = false;	
		}
	}
	else
	{
		ret_value = ActiveObject::Load(_json);	
	}

	return	ret_value;
}

ServerLinker::operator JSONNode() const
{
	JSONNode	root;

	root.push_back(JSONNode(TITLE_NAME_BROKER, broker_));

	JSONNode	trace_config = trace;
	trace_config.set_name(TITLE_NAME_TRACE);

	root.push_back(trace_config);

	return	root;
}

bool	ServerLinker::SetSecureCode(std::string const& _secret_code)
{
	secret_code_=  _secret_code;

	return	true;
}

bool	ServerLinker::GetSecureCode(std::string & _secret_code)
{
	_secret_code = secret_code_;

	return	true;
}

bool	ServerLinker::SetBroker(std::string const& _broker)
{
	broker_ = _broker;

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

uint32_t	ServerLinker::GetUpLink(std::vector<UpLink*>& _link_list)
{
	for(auto it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
	{
		_link_list.push_back(it->second);		
	}

	return	_link_list.size();
}

uint32_t	ServerLinker::GetUpLink(std::list<std::string>& _topic_name_list)
{
	for(auto it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
	{
		_topic_name_list.push_back(it->first);		
	}

	return	_topic_name_list.size();
}

uint32_t	ServerLinker::GetDownLink(std::vector<DownLink*>& _link_list)
{
	for(auto it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
	{
		_link_list.push_back(it->second);		
	}

	return	_link_list.size();
}

uint32_t	ServerLinker::GetDownLink(std::list<std::string>& _topic_name_list)
{
	for(auto it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
	{
		_topic_name_list.push_back(it->first);		
	}

	return	_topic_name_list.size();
}

RdKafka::Topic*	ServerLinker::CreateProducerTopic(std::string const& _topic_name, std::string& _error_string)
{
	RdKafka::Topic*	topic  = NULL;

	if (producer_ != NULL)
	{
		topic  = RdKafka::Topic::create(producer_, _topic_name, conf_topic_, _error_string);
		if (topic != NULL)
		{
			TRACE_INFO("The producer topic[" << _topic_name << "] created");
		}
		else
		{
			TRACE_ERROR("Failed to create topic!");
		}
	}

	return	topic;
}

RdKafka::Topic*	ServerLinker::CreateConsumerTopic(std::string const& _topic_name, std::string& _error_string)
{
	RdKafka::Topic* topic  = NULL;
	
	if (consumer_ != NULL)
	{
		topic = RdKafka::Topic::create(consumer_, _topic_name, conf_topic_, _error_string);
		if (topic != NULL)
		{
			TRACE_INFO("The consumer topic[" << _topic_name << "] created");
		}
		else
		{
			TRACE_ERROR("Failed to create topic!");
		}
	}

	return	topic;
}

ServerLinker::UpLink*	ServerLinker::AddUpLink(std::string const& _topic_name, int32_t _partition)
{
	UpLink*	link = NULL;

	link = GetUpLink(_topic_name);
	if (link == NULL)
	{
		try 
		{
			link = new UpLink(*this, _topic_name, _partition);	

			up_link_map_[_topic_name] = link;
			TRACE_INFO("Uplink added : " << _topic_name << "[" << link << "]");

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

	TRACE_INFO("UpLink[" << _topic_name << "] added.");
	return	link;
}

ServerLinker::UpLink*	ServerLinker::GetUpLink(std::string const& _topic_name)
{
	auto it = up_link_map_.find(_topic_name);
	if (it == up_link_map_.end())
	{
		return	NULL;
	}

	return	it->second;	
}

bool		ServerLinker::DelUpLink(std::string const& _topic_name)
{
	auto it = up_link_map_.find(_topic_name);
	if (it == up_link_map_.end())
	{
		return	false;
	}

	UpLink*	link = it->second;	

	up_link_map_.erase(it);

	delete link;

	return	true;	
}

ServerLinker::DownLink*	ServerLinker::AddDownLink(std::string const& _topic_name, int32_t _partition)
{
	DownLink*	link = NULL;

	link = ServerLinker::GetDownLink(_topic_name);
	if (link == NULL)
	{
		try
		{
			link = new DownLink(*this, _topic_name, _partition);	

			down_link_map_[_topic_name] = link;
			TRACE_INFO("Downlink added : " << _topic_name << "[" << link << "]");

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
	TRACE_INFO("DownLink[" << _topic_name << "] added.");
	return	link;
}

bool		ServerLinker::DelDownLink(std::string const& _topic_name)
{
	auto it = down_link_map_.find(_topic_name);
	if (it == down_link_map_.end())
	{
		return	false;
	}
	DownLink*	link = it->second;	

	down_link_map_.erase(it);

	delete link;

	return	true;	
}

ServerLinker::DownLink*	ServerLinker::GetDownLink(std::string const& _topic_name)
{
	auto it = down_link_map_.find(_topic_name);
	if (it == down_link_map_.end())
	{
		return	NULL;
	}

	return	it->second;	
}

void	ServerLinker::Preprocess()
{
	RdKafka::Conf::ConfResult result = conf_global_->set("metadata.broker.list", broker_, error_string_);
	if (result != RdKafka::Conf::CONF_OK)
	{
		TRACE_ERROR("Failed to set broker to conf_global!");
	}

	conf_global_->set("dr_cb", &delivery_report_cb_, error_string_);
	conf_global_->set("event_cb", &event_cb_, error_string_);

	AddUpLink(global_up_topic_);
	AddDownLink(global_down_topic_);

	ActiveObject::Preprocess();
	
}

void	ServerLinker::Process()
{
	if (IsConnected())
	{
		try
		{
			producer_->poll(0);

			for(auto it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
			{
				it->second->Consume(&consume_cb_);
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
		for(auto it = request_map_.begin(); it != request_map_.upper_bound(current) ; it++)
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

	ActiveObject::Process();
}

void	ServerLinker::Postprocess()
{
	Disconnect();

	ActiveObject::Preprocess();

}

bool	ServerLinker::Start()
{
	if (broker_.length() == 0)
	{
		return	false;	
	}

	return	ActiveObject::Start();
}

bool	ServerLinker::IsConnected()
{
	return	broker_connected_;
}

bool	ServerLinker::Connect(uint32_t _delay_sec)
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

bool	ServerLinker::InternalConnect(uint32_t _delay_sec)
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
			TRACE_INFO("Create producer")
			producer_ 	= RdKafka::Producer::create(conf_global_, error_string_);
			if (producer_ == NULL)
			{
				TRACE_ERROR("Failed to create producer!");
				throw std::logic_error("Connection timeout to broker");
			}

			TRACE_INFO("Create consumer")
			consumer_	= RdKafka::Consumer::create(conf_global_, error_string_);
			if (consumer_ == NULL)
			{
				TRACE_ERROR("Failed to create consumer!");
				throw std::logic_error("Connection timeout to broker");
			}
		
			TRACE_INFO("Up Link Count : " << up_link_map_.size());
			for(auto it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
			{
				it->second->Start();
			}

			TRACE_INFO("Down Link Count : " << down_link_map_.size());
			for(auto it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
			{
				it->second->Start();
			}

			TRACE_INFO("Connected to broker.")
			broker_connected_ = true;
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

bool	ServerLinker::Disconnect()
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

bool	ServerLinker::InternalDisconnect()
{
	broker_connected_ = false;

	for(auto it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
	{
		it->second->Stop();
	}

	for(auto it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
	{
		it->second->Stop();
	}

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

#if 0
bool	ServerLinker::Produce(std::string const& _payload)
{
	try
	{
		std::string msg_id = std::to_string(Date::GetCurrent().GetMicroSecond());
		std::string secret_code = secret_code_hash_(secret_code_ + msg_id);

		RCSProduce *message = new RCSProduce(this->GetID(), msg_id, secret_code, global_up_topic_, libjson::parse(_payload));

		Post(message);
		return	true;
	}
	catch(std::exception& e)
	{
		TRACE_ERROR("Failed to produce![" << e.what() << "]");
		return	false;	
	}
}

bool	ServerLinker::Produce(std::string const& _type, JSONNode& _payload)
{
	JSONNode	body;

	RequestInit(_type, body);

	for(auto it = _payload.begin() ; it != _payload.end() ; it++)
	{
		body.push_back(*it);
	}
	return	Produce(body);
}
#endif

bool	ServerLinker::KeepAliveEnable(bool _enable)
{
	keep_alive_enable_ = _enable;

	return	true;
}

bool	ServerLinker::ReportEPData(Endpoint* _ep)
{
	RCSMessage	message(MSG_STR_REPORT);

	Endpoint::ValueList	value_list;
	_ep->GetUnreportedValueList(value_list);

	JSONNode	node;

	node.push_back(JSONNode(TITLE_NAME_ID, _ep->GetID()));
	node.push_back(JSONNode(TITLE_NAME_COUNT, value_list.size()));

	JSONNode	array(JSON_ARRAY);

	for(auto it = value_list.begin(); it != value_list.end() ; it++)
	{
		JSONNode	item;

		item.push_back(JSONNode(TITLE_NAME_TIME, std::to_string(time_t((*it)->GetDate()))));
		item.push_back(JSONNode(TITLE_NAME_VALUE, std::string(*(*it))));

		array.push_back(item);
	}
	array.set_name(TITLE_NAME_VALUE);
	node.push_back(array);

	message.AddEPData(node);

	return	Send(message);
}

bool	ServerLinker::RequestInit(std::string const& _type, JSONNode& _payload)
{
	JSONNode	payload;

	payload.push_back(JSONNode(TITLE_NAME_MSG_TYPE, _type));
	payload.push_back(JSONNode(TITLE_NAME_TIME, std::to_string(time_t(Date::GetCurrent()))));

	_payload = payload;

	return	true;
}

bool	ServerLinker::ReplyInit(std::string const& _type, std::string const& _req_id, JSONNode& _payload)
{
	JSONNode	payload;

	payload.push_back(JSONNode(TITLE_NAME_MSG_TYPE, _type));
	payload.push_back(JSONNode(TITLE_NAME_TIME, std::to_string(time_t(Date::GetCurrent()))));
	payload.push_back(JSONNode(TITLE_NAME_REQ_ID, _req_id));

	_payload = payload;

	return	true;
}

bool	ServerLinker::AddGateway(JSONNode& _payload, Gateway* _gateway, Properties::Fields const& _fields)
{
	auto it = _payload.find(TITLE_NAME_GATEWAY);
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
	auto it = _payload.find(TITLE_NAME_GATEWAY);
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

bool	ServerLinker::AddDevice(JSONNode& _payload, Device* _device, Properties::Fields const& _fields)
{
	auto it = _payload.find(TITLE_NAME_DEVICE);
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
	auto it = _payload.find(TITLE_NAME_DEVICE);
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

bool	ServerLinker::AddEndpoint(JSONNode& _payload, Endpoint* _endpoint, Properties::Fields const& _fields)
{
	auto it = _payload.find(TITLE_NAME_ENDPOINT);
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
	auto it = _payload.find(TITLE_NAME_ENDPOINT);
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

	Endpoint::ValueList value_list;

	TRACE_INFO("Add EP Data!");
	if (!_ep->GetUnreportedValueList(value_list))
	{
		TRACE_WARN("Failed to get unreported value list.");
		return	false;
	}

	node.push_back(JSONNode(TITLE_NAME_ID, _ep->GetID()));
	node.push_back(JSONNode(TITLE_NAME_COUNT, value_list.size()));

	JSONNode	array(JSON_ARRAY);
	for(auto it = value_list.begin(); it != value_list.end() ; it++)
	{
		JSONNode	item;
		
		item.push_back(JSONNode(TITLE_NAME_TIME, std::to_string(time_t((*it)->GetDate()))));
		item.push_back(JSONNode(TITLE_NAME_VALUE, std::string(*(*it))));

		array.push_back(item);
	}

	array.set_name(TITLE_NAME_VALUE);
	node.push_back(array);

	auto it = _payload.find(TITLE_NAME_DATA);
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
	Endpoint::ValueList value_list;

	if (!_ep->GetUnreportedValueList(value_list))
	{
		TRACE_WARN("Failed to get unreported value list.");
		return	false;
	}

	node.push_back(JSONNode(TITLE_NAME_ID, _ep->GetID()));
	node.push_back(JSONNode(TITLE_NAME_COUNT, value_list.size()));

	JSONNode	array(JSON_ARRAY);
	for(auto it = value_list.begin(); it != value_list.end() ; it++)
	{
		JSONNode	item;
		
		item.push_back(JSONNode(TITLE_NAME_TIME, std::to_string(time_t((*it)->GetDate()))));
		item.push_back(JSONNode(TITLE_NAME_VALUE, std::string(*(*it))));

		array.push_back(item);
	}
	array.set_name(TITLE_NAME_VALUE);

	node.push_back(array);

	auto it = _payload.find(TITLE_NAME_DATA);
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

bool	ServerLinker::GetEPDataInfo(Endpoint* _ep)
{
#if 0
	JSONNode	payload;

	Endpoint::ValueList value_list;

	payload.push_back(JSONNode(TITLE_NAME_MSG_TYPE, MSG_STR_GET_EP_DATA_INFO));
	payload.push_back(JSONNode(TITLE_NAME_TIME, time_t(Date::GetCurrent())));
	payload.push_back(JSONNode(TITLE_NAME_ENDPOINT_ID, _ep->GetID()));

	return	Produce(payload, true);
#endif
	return	false;
}

bool	ServerLinker::ConfirmEPDataInfo(Endpoint* _ep)
{
#if 0
	JSONNode	payload;

	Endpoint::ValueList value_list;

	payload.push_back(JSONNode(TITLE_NAME_MSG_TYPE, MSG_STR_CONFIRM_EP_DATA_INFO));
	payload.push_back(JSONNode(TITLE_NAME_TIME, time_t(Date::GetCurrent())));
	payload.push_back(JSONNode(TITLE_NAME_ENDPOINT_ID, _ep->GetID()));
	payload.push_back(JSONNode(TITLE_NAME_COUNT, _ep->GetDataCount()));
	if (_ep->GetDataCount() != 0)
	{
		payload.push_back(JSONNode(TITLE_NAME_LAST_TIME, time_t(_ep->GetDateOfLastData())));
	}

	return	Produce(payload);
#endif
	return	false;
}

bool	ServerLinker::Error(std::string const& _req_id ,std::string const& _err_msg)
{
	RCSMessage	message(MSG_STR_ERROR);
	
	message.SetReqID(_req_id);

	return	Send(message);
}

bool	ServerLinker::ConfirmRequest(RCSMessage* _reply, std::string& _req_type, bool _exception)
{
	for(auto it = request_map_.begin() ; it != request_map_.end() ; it++)
	{
		Produce*	produce = it->second;
		RCSMessage&	message = produce->GetMessage();

		if (message.GetMsgID() == _reply->GetReqID())
		{
			GetValue(message.GetPayload(), TITLE_NAME_MSG_TYPE, _req_type);
		
			delete produce;

			request_map_.erase(it);

			return	true;
		}
	}

	if (_exception)
	{
		throw SLMRequestTimeout(_reply);
	}

	return	false;
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
	catch(SLMObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		Error(msg_id, e.what());
	}
	catch(SLMRequestTimeout& e)
	{
		TRACE_ERROR(e.what());
		Error(msg_id, e.what());
	}
	catch(SLMInvalidArgument& e)
	{
		TRACE_ERROR(e.what());
		Error(msg_id, e.what());
	}
	catch(SLMNotInitialized& e)
	{
		TRACE_ERROR(e.what());
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
	}

	return	true;
}

void	ServerLinker::OnConsume(Consume* _consume)
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

		if (!ConfirmRequest(&message, req_type))
		{
			throw SLMRequestTimeout(&message);
		}

		manager_->GetRCSServer().Confirm(message, req_type);	
	}
	else if (message.GetMsgType() == MSG_STR_ERROR)
	{
		std::string	req_type;

		if (!ConfirmRequest(&message, req_type))
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
	RCSMessage&	message = _produce->GetMessage();	

	message.Make();

	UpLink* link = GetUpLink(topic);
	if (link != NULL)
	{
		std::string	payload = message.GetPayload().write();

		RdKafka::ErrorCode error_code = producer_->produce(const_cast<char *>(topic.c_str()), 0, 
				RdKafka::Producer::RK_MSG_COPY, const_cast<char *>(payload.c_str()), payload.size(), NULL, 0, 0, _produce);
		if (error_code != RdKafka::ERR_NO_ERROR)
		{
			link->IncreaseNumberOfErrorMessages();
			TRACE_ERROR("Failed to produce : " << RdKafka::err2str(error_code));
			return	true;
		}

		message_map_.insert(std::pair<std::string, Produce*>(message.GetMsgID(), _produce));

		link->IncreaseNumberOfOutGoingMessages();
		link->Touch();

		TRACE_INFO("  Topic : " << topic);
		TRACE_INFO("Payload : " << message.GetPayload().write_formatted());
	}

	return	false;
}
