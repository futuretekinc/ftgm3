#include <exception>
#include "defined.h"
#include "server_linker.h"
#include "trace.h"
#include "time2.h"

#define	MESSAGE_SERVER_LINKER			(0x00020000)
#define	MESSAGE_SERVER_LINKER_PRODUCE	(MESSAGE_SERVER_LINKER + 1)

struct	MessageProduce : Message
{
	MessageProduce(std::string const& _sender, std::string const& _topic, int32_t _partition, std::string const& _message)
	: Message(MESSAGE_SERVER_LINKER_PRODUCE, _sender)
	{
		topic = _topic;	
		message = _message;	
		partition = _partition;
	}

	std::string		topic;
	std::string		message;
	int32_t			partition;
};

ServerLinker::EventCB::EventCB(ServerLinker& _parent)
: RdKafka::EventCb(), parent_(_parent)
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
				parent_.Stop();
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

ServerLinker::DeliveryReportCB::DeliveryReportCB(ServerLinker& _parent)
: RdKafka::DeliveryReportCb(), parent_(_parent)
{
	trace.SetClassName(GetClassName());
	name_ 	= "delivery_report";
	enable_ = true;
}

void ServerLinker::DeliveryReportCB::dr_cb (RdKafka::Message &message) 
{
	TRACE_INFO("Message delivery for (" << message.len() << " bytes): " << message.errstr());
	if (message.key())
	{
		TRACE_INFO("Key: " << *(message.key()));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker::Link
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinker::Link::Link(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition)
: linker_(_linker), partition_(_partition), topic_name_(_topic_name), topic_(NULL)
{
	trace.SetClassName(GetClassName());
	name_ 	= "link";
	enable_ = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker::UpLink
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinker::UpLink::UpLink(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition)
: Link(_linker, _topic_name, _partition)
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

	TRACE_INFO("Send(" << topic_->name() << ", " << partition_ << ", " << _message << ")");
	linker_.Produce(topic_->name(), partition_, _message);

	return	true;
}

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
: Link(_linker, _topic_name, _partition), message_cb_(*this), offset_(RdKafka::Topic::OFFSET_END)
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

			TRACE_INFO2(&linker_, _msg.topic_name() << " : " << _msg.offset());
			if (_msg.len() != 0)
			{
				if (libjson::is_valid((char *)_msg.payload()))
				{
					JSONNode	payload = libjson::parse((char *)_msg.payload());
					TRACE_INFO2(&linker_, "Payload : " << payload.write_formatted());

					try
					{
						Message *message = new MessageConsume(linker_.GetID(), _msg.topic_name(), payload);


						Message::Send(linker_.GetID(), message);
					}
					catch(std::exception& e)
					{
						TRACE_ERROR2(&linker_, "Failed to create message[" << e.what() << "]");	
					}
				}
				else
				{
					TRACE_ERROR2(&linker_, "Invalid payload : " << (char *)_msg.payload());
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinker
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinker::ServerLinker()
: ActiveObject(), event_cb_(*this), delivery_report_cb_(*this), consumer_(NULL), producer_(NULL), connection_retry_interval_(10000000), consume_cb_(*this)
{
	trace.SetClassName(GetClassName());
	name_ 	= "link";
	enable_ = true;
}

ServerLinker::~ServerLinker()
{
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

	if ((_json.name() == TITLE_NAME_SERVER_LINKER) || (_json.name().size() == 0))
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
		if (_json.type() == JSON_ARRAY)
		{
			for(auto broker = _json.begin() ; broker != _json.end() ; broker++)
			{
				AddBroker(broker->as_string());
			}
		}
		else if (_json.type() == JSON_STRING)
		{
			AddBroker(_json.as_string());
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

	JSONNode	broker_list(JSON_ARRAY);

	for(auto it = broker_list_.begin(); it != broker_list_.end() ; it++)
	{
		JSONNode	broker(JSON_STRING);

		broker = (*it);

		broker_list.push_back(broker);
	}
	broker_list.set_name(TITLE_NAME_BROKER);
	root.push_back(broker_list);

	JSONNode	trace_config = trace;
	trace_config.set_name(TITLE_NAME_TRACE);

	root.push_back(trace_config);

	return	root;
}

bool		ServerLinker::AddBroker(std::string const& _broker)
{
	for(auto it = broker_list_.begin(); it != broker_list_.end() ; it++)
	{
		if (_broker == (*it))
		{
			TRACE_ERROR("Failed to add broker because already exist broker.");
			return	false;	
		}
	}

	broker_list_.push_back(_broker);

	return	true;
}

bool		ServerLinker::DeleteBroker(std::string const& _broker)
{
	for(auto it = broker_list_.begin(); it != broker_list_.end() ; it++)
	{
		if (_broker == (*it))
		{
			broker_list_.erase(it);
			return	true;	
		}
	}

	TRACE_WARN("Failed to delete broker because broker is not exist.");

	return	false;
}

uint32_t	ServerLinker::GetBrokerList(std::list<std::string>& _broker_list)
{
	for(auto it = broker_list_.begin(); it != broker_list_.end() ; it++)
	{
		_broker_list.push_back(*it);		
	}

	return	_broker_list.size();
}

uint32_t	ServerLinker::GetUpLinkNameList(std::list<std::string>& _topic_name_list)
{
	for(auto it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
	{
		_topic_name_list.push_back(it->first);		
	}

	return	_topic_name_list.size();
}

uint32_t	ServerLinker::GetDownLinkNameList(std::list<std::string>& _topic_name_list)
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
	}

	if (topic == NULL)
	{
		TRACE_ERROR("Failed to create topic!");
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
	}

	if (topic == NULL)
	{
		TRACE_ERROR("Failed to create topic!");
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
	if (it != up_link_map_.end())
	{
		return	it->second;	
	}

	return	NULL;
}

bool		ServerLinker::DeleteUpLink(std::string const& _topic_name)
{
	auto it = up_link_map_.find(_topic_name);
	if (it != up_link_map_.end())
	{
		UpLink*	link = it->second;	

		up_link_map_.erase(it);

		delete link;

		return	true;	
	}

	return	false;
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
		}
		catch(std::exception& e)
		{
			TRACE_ERROR("Failed to create down link!");
			return	NULL;
		}
	}

	TRACE_INFO("DownLink[" << _topic_name << "] added.");
	return	link;
}

bool		ServerLinker::DeleteDownLink(std::string const& _topic_name)
{
	auto it = down_link_map_.find(_topic_name);
	if (it != down_link_map_.end())
	{
		DownLink*	link = it->second;	

		down_link_map_.erase(it);

		delete link;

		return	true;	
	}

	return	false;
}

ServerLinker::DownLink*	ServerLinker::GetDownLink(std::string const& _topic_name)
{
	auto it = down_link_map_.find(_topic_name);
	if (it != down_link_map_.end())
	{
		return	it->second;	
	}

	return	NULL;
}

void	ServerLinker::Preprocess()
{
	conf_global_= RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
	conf_topic_ = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

	std::ostringstream oss;
	bool	first = true;

	for(auto it = broker_list_.begin(); it != broker_list_.end() ; it++)
	{
		if (!first)
		{
			oss << "," << (*it);
		}
		else
		{
			oss << (*it);
			first = false;	
		}
	}

	RdKafka::Conf::ConfResult result = conf_global_->set("metadata.broker.list", oss.str(), error_string_);
	if (result != RdKafka::Conf::CONF_OK)
	{
		TRACE_ERROR("Failed to set broker to conf_global!");
	}

	conf_global_->set("dr_cb", &delivery_report_cb_, error_string_);
	conf_global_->set("event_cb", &event_cb_, error_string_);

	Date date = Date::GetCurrentDate();
	producer_retry_timeout_.Set(date);
	consumer_retry_timeout_.Set(date);

	ActiveObject::Preprocess();
	
}

void	ServerLinker::Process()
{
	if (producer_ != NULL)
	{
		producer_->poll(0);
	}
	else
	{
		if (producer_retry_timeout_.RemainTime() == 0)
		{
			TRACE_INFO("Create producer")
				producer_ 	= RdKafka::Producer::create(conf_global_, error_string_);
			if (producer_ == NULL)
			{
				TRACE_ERROR("Failed to create producer!");
				Date date = Date::GetCurrentDate() + connection_retry_interval_;
				producer_retry_timeout_.Set(date);
			}
			else
			{
				TRACE_INFO("Up Link Count : " << up_link_map_.size());
				for(auto it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
				{
					TRACE_INFO("Up Link : " << it->second);
					it->second->Start();
				}
			}
		}
	}

	if(consumer_ != NULL)
	{
#if 1
		for(auto it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
		{
			if (consumer_->consume_callback(it->second->GetTopic(), it->second->GetPartition(), 1, it->second->GetConsumCB(), it->second) < 0)
			{
				TRACE_ERROR("Failed to set down link callback!");
			}
		}
#endif
	}
	else
	{
		if (consumer_retry_timeout_.RemainTime() == 0)
		{
			TRACE_INFO("Create consumer")
				consumer_	= RdKafka::Consumer::create(conf_global_, error_string_);
			if (consumer_ == NULL)
			{
				TRACE_ERROR("Failed to create consumer!");
				Date date = Date::GetCurrentDate() + connection_retry_interval_;
				consumer_retry_timeout_.Set(date);
			}
			else
			{
				TRACE_INFO("Down Link Count : " << down_link_map_.size());
				for(auto it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
				{
					TRACE_INFO("Down Link : " << it->second);
					it->second->Start();
				}
			}
		}
	}


	ActiveObject::Process();
}

void	ServerLinker::Postprocess()
{
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

	ActiveObject::Preprocess();

}

void	ServerLinker::OnMessage(Message* _message)
{
	switch(_message->type)
	{
	case	MESSAGE_SERVER_LINKER_PRODUCE:
		{
			if (producer_ != NULL)
			{
				MessageProduce*	message_produce = dynamic_cast<MessageProduce*>(_message);

				if (message_produce != 0)
				{
					RdKafka::ErrorCode error_code = producer_->produce(const_cast<char *>(message_produce->topic.c_str()), message_produce->partition, 
							RdKafka::Producer::RK_MSG_COPY, const_cast<char *>(message_produce->message.c_str()), message_produce->message.size(), NULL, 0, 0, NULL);
					if (error_code != RdKafka::ERR_NO_ERROR)
					{
						TRACE_ERROR("Failed to produce : " << RdKafka::err2str(error_code));
					}
					else
					{
						TRACE_INFO("Message : " << message_produce->topic << ":" << message_produce->partition );
						TRACE_INFO_JSON(message_produce->message);
					}
				}
				else
				{
					TRACE_ERROR("Invalid message!");
				}
			}
			else
			{
				TRACE_ERROR("Failed to produce because producer not exist!");		
			}
		}
		break;

	default:
		{
			TRACE_ERROR("Unknown message[" << _message->type << "]");	
		}

	}
}

bool	ServerLinker::Start()
{
	if (broker_list_.size() == 0)
	{
		return	false;	
	}

	return	ActiveObject::Start();
}

bool	ServerLinker::Produce(std::string const& _topic_name, int32_t _partition, std::string const& _message)
{
	try
	{
		Post(new MessageProduce(id_, _topic_name, _partition, _message));
	}
	catch(std::exception& e)
	{
		TRACE_ERROR("Exception : " << e.what());	
	}
}

