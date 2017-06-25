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


/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinkerMosq::UpLink
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinkerMosq::UpLinkMosq::UpLinkMosq(ServerLinkerMosq* _linker, std::string const& _topic)
: UpLink(_linker, _topic)
{
}

bool	ServerLinkerMosq::UpLinkMosq::Send(std::string const& _message)
{
	ServerLinkerMosq*	mosq_linker = dynamic_cast<ServerLinkerMosq*>(linker_);
	if (mosq_linker == NULL)
	{
		TRACE_ERROR("The server linker is not supported mosquitto!");
		return	false;	
	}

	int	msg_id;

	int ret = mosquitto_publish(mosq_linker->mosquitto_, &msg_id, topic_.c_str(), _message.size(), _message.c_str(), qos_, 0);
	if (ret != 0)
	{
		return	false;
	}

	return	true;
}

bool	ServerLinkerMosq::UpLinkMosq::Send(std::string const& _message, std::string& _msg_id)
{
	ServerLinkerMosq*	mosq_linker = dynamic_cast<ServerLinkerMosq*>(linker_);
	if (mosq_linker == NULL)
	{
		TRACE_ERROR("The server linker is not supported mosquitto!");
		return	false;	
	}

	int	msg_id;

	int ret = mosquitto_publish(mosq_linker->mosquitto_, &msg_id, topic_.c_str(), _message.size(), _message.c_str(), qos_, 0);
	if (ret != 0)
	{
		return	false;
	}

	_msg_id = ToString(msg_id);
	return	true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinkerMosq::DownLink
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinkerMosq::DownLinkMosq::DownLinkMosq(ServerLinkerMosq* _linker, std::string const& _topic)
: DownLink(_linker, _topic)
{
}

bool	ServerLinkerMosq::DownLinkMosq::Start()
{
	ServerLinkerMosq*	mosq_linker = dynamic_cast<ServerLinkerMosq*>(linker_);
	if (mosq_linker == NULL)
	{
		TRACE_ERROR("The server linker is not supported mosquitto!");
		return	false;	
	}

	if (mosquitto_subscribe(mosq_linker->mosquitto_, NULL, topic_.c_str(), 0) > 0)
	{
		TRACE_ERROR("Subscribe error : " << topic_);	
		return	false;
	}

	TRACE_INFO("Subscribe[" << topic_ << "] connected!!!");
	connected_ = true;

	return	true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Class ServerLinkerMosq
/////////////////////////////////////////////////////////////////////////////////////////////
ServerLinkerMosq::ServerLinkerMosq(ObjectManager* _manager)
: 	ServerLinker(_manager),
	protocol_version_(MQTT_PROTOCOL_V31)
{
	mosquitto_lib_init();

	mosquitto_ = mosquitto_new(manager_->GetID().c_str(), true, (void *)this);

	mosquitto_max_inflight_messages_set(mosquitto_, 20);
	mosquitto_opts_set(mosquitto_, MOSQ_OPT_PROTOCOL_VERSION, (void *)&protocol_version_);
	mosquitto_log_callback_set(mosquitto_, OnLogCB);
	mosquitto_publish_callback_set(mosquitto_, OnPublishCB);
	mosquitto_subscribe_callback_set(mosquitto_, OnSubscribeCB);
	mosquitto_connect_callback_set(mosquitto_, OnConnectCB);
	mosquitto_disconnect_callback_set(mosquitto_, OnDisconnectCB);
	mosquitto_message_callback_set(mosquitto_, OnMessageCB);


}

ServerLinkerMosq::~ServerLinkerMosq()
{

	if (mosquitto_)
	{
		mosquitto_destroy(mosquitto_);
	}
}

bool	ServerLinkerMosq::SetProperty(JSONNode const& _config, bool _check)
{
	bool	ret_value = true;

	if (_config.name() == TITLE_NAME_MQTT)
	{
		if (_config.type() != JSON_NODE)
		{
			TRACE_ERROR("The mqtt properties type must have node!");
			return	false;
		}

		for(JSONNode::const_iterator it = _config.begin() ; it != _config.end() ; it++)
		{
			if (it->name() == TITLE_NAME_VERSION)
			{
				if (it->as_string() == "v3.1")
				{
					protocol_version_ = MQTT_PROTOCOL_V31;
				}
				else if (it->as_string() == "v3.1.1")
				{
					protocol_version_ = MQTT_PROTOCOL_V311;
				}
				else
				{
					TRACE_ERROR("The MQTT version " << it->as_string() << " is not supported!");	
					return	false;
				}
			}
			else if (it->name() == TITLE_NAME_LOG)
			{
					
			}
		}
	}
	else
	{
		ret_value = ServerLinker::SetProperty(_config, _check);	
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

	JSONNode	mqtt;
	switch(protocol_version_ )
	{
	case	MQTT_PROTOCOL_V31: 	mqtt.push_back(JSONNode(TITLE_NAME_VERSION, "v3.1")); break;
	case	MQTT_PROTOCOL_V311:	mqtt.push_back(JSONNode(TITLE_NAME_VERSION, "v3.1.1")); break;
	}

	mqtt.set_name(TITLE_NAME_MQTT);

	root.push_back(mqtt);

	JSONNode	trace_config = trace;
	trace_config.set_name(TITLE_NAME_TRACE);

	root.push_back(trace_config);

	return	root;
}


ServerLinker::UpLink*	ServerLinkerMosq::CreateUpLink(std::string const& _topic)
{
	try
	{
		return	new UpLinkMosq(this, _topic);	
	}
	catch(std::exception& e)
	{
		TRACE_ERROR("Failed to creat UpLinkMosq!!");	
	}
}

ServerLinker::DownLink*	ServerLinkerMosq::CreateDownLink(std::string const& _topic)
{
	try
	{
		return	new DownLinkMosq(this, _topic);	
	}
	catch(std::exception& e)
	{
		TRACE_ERROR("Failed to creat DownLinkMosq!!");	
	}
}

void	ServerLinkerMosq::Preprocess()
{
	ServerLinker::Preprocess();
}

void	ServerLinkerMosq::Process()
{
	if (!IsConnected())
	{
		if (auto_connection_)
		{
			InternalConnect();
		}
	}

	
	uint64_t	current = uint64_t(Date::GetCurrent().GetMicroSecond());

	request_map_locker_.Lock();

	if (request_map_.upper_bound(current) != request_map_.begin())
	{
		for(std::map<uint64_t, Produce*>::iterator it = request_map_.begin(); it != request_map_.upper_bound(current) ; it++)
		{
			Produce*	produce = it->second;
			it->second = NULL;

			if (produce->GetMessage().GetMsgType() != MSG_TYPE_RCS_KEEP_ALIVE)
			{
				if (produce->GetTransmissionCount() < retransmission_count_max_)
				{
					TRACE_ERROR("Retransmission : " <<  produce->GetTransmissionCount());
					Post(produce);
					produce->IncTransmissionCount();
				}
				else
				{
					try
					{
						delete produce;
					}
					catch(std::exception& e)
					{
						TRACE_INFO("Woops : Failed to delete produce");
					}
				}
			}
			else
			{
				try
				{
					delete produce;
				}
				catch(std::exception& e)
				{
					TRACE_INFO("Woops : Failed to delete produce");
				}
			}
		}

		TRACE_INFO("Timeout request erase start!");
		request_map_.erase(request_map_.begin(), request_map_.upper_bound(current));
		TRACE_INFO("Timeout request erase finished!");
	}
	
	request_map_locker_.Unlock();

	ProcessObject::Process();
}

void	ServerLinkerMosq::Postprocess()
{
	ServerLinker::Postprocess();
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

	TRACE_ENTRY;
	for(std::map<std::string, DownLink*>::iterator it = down_link_map_.begin(); it != down_link_map_.end() ; it++)
	{
		it->second->Stop();
	}
	TRACE_EXIT;

	return	true;
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

	TRACE_INFO2(linker, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	TRACE_INFO2(linker, "Disconnected from broker[" << linker->GetBroker() << "]");
	TRACE_INFO2(linker, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
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

	std::map<std::string, Produce*>::iterator it = linker->message_map_.find(ToString(_mid));
	if (it != linker->message_map_.end())
	{
		Produce*	produce = it->second;

		uint64_t	expire_time = Date::GetCurrent().GetMicroSecond() + (linker->request_timeout_ * TIME_SECOND);

		TRACE_INFO2(linker, "The request expire time[" << expire_time << "] has been set in the request message[" << produce->GetMessage().GetMsgID() << ":" << _mid << "].");
		linker->request_map_[expire_time] = produce;
		linker->message_map_.erase(it);
	}
	else
	{
//		TRACE_INFO2(linker, "Tth publish[" << _mid << "] not found!");	
	}
}

void	ServerLinkerMosq::OnLogCB(struct mosquitto *_mosq, void *_obj, int _level, const char *_str)
{
	ServerLinkerMosq*	linker = (ServerLinkerMosq*)_obj;

//	TRACE_INFO2(linker, "Log : " << _str);
}

void	ServerLinkerMosq::OnSubscribeCB(struct mosquitto *_mosq, void *_obj, int mid, int qos_count, const int *granted_qos)
{
	ServerLinkerMosq*	linker = (ServerLinkerMosq*)_obj;

//	TRACE_INFO2(linker, "Subscribe : " << mid);
}

void	ServerLinkerMosq::OnMessageCB(struct mosquitto *_mosq, void *_obj, const struct mosquitto_message *_message)
{
	ServerLinkerMosq*	linker = (ServerLinkerMosq *)_obj;
	DownLink*	link = linker->GetDownLink(_message->topic);

	//TRACE_INFO2(linker, "Message Received : " << _message->topic);
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
