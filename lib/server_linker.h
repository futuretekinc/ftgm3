#ifndef	SERVER_LINKER_H_
#define	SERVER_LINKER_H_

#include <list>
#include "defined.h"
#include "process_object.h"
#include "librdkafka/rdkafkacpp.h"
#include "timer.h"
#include "rcs_message.h"

#define	MSG_TYPE_SL						0x00040000
#define	MSG_TYPE_SL_CONSUME				(MSG_TYPE_SL + 1)
#define	MSG_TYPE_SL_PRODUCE				(MSG_TYPE_SL + 2)

class	ObjectManager;
class	Device;
class	Endpoint;

class	ServerLinker : public ProcessObject
{
public:

	class	Produce : public Message
	{
		public:
			Produce(std::string const& _topic, RCSMessage const& _message);
			Produce(std::string const& _topic, std::string const& _payload);

			const 	std::string&	GetTopic()		{	return	topic_;	}
					RCSMessage&		GetMessage()	{	return	message_;}

		protected:
			std::string	topic_;
			RCSMessage	message_;
	};

	class	Consume : public Message
	{
		public:
			Consume(std::string const& _topic, std::string const& _payload);

			const 	std::string&	GetTopic()		{	return	topic_;	}
					RCSMessage&		GetMessage()	{	return	message_;}
		protected:
			std::string	topic_;
			RCSMessage	message_;
	};

	class	SLMError : public std::exception
	{
	public:
		SLMError();

		virtual const char* 	what() const throw();

	protected:
		std::string	message_;
	};

	class	SLMInvalidArgument : public SLMError
	{
	public:
		explicit SLMInvalidArgument(std::string const& _argument);

	protected:
	};

	class	SLMObjectNotFound : public SLMError
	{
	public:
		explicit SLMObjectNotFound(std::string const& _object_id);

		const	std::string&	GetObjectID()		{	return	object_id_;		}

	protected:
		std::string	object_id_;
	};

	class	SLMRequestTimeout : public SLMError
	{
	public:
		explicit	SLMRequestTimeout(RCSMessage* _message);
	};

	class	SLMNotInitialized : public SLMError
	{
	public:
		explicit	SLMNotInitialized(std::string const& _message);
	};

	class EventCB : public RdKafka::EventCb, Object
	{
	public:
					EventCB(ServerLinker& _linker);

			void 	event_cb (RdKafka::Event &_event) ;

	protected:
		ServerLinker&	linker_;
	};

	class DeliveryReportCB : public RdKafka::DeliveryReportCb, Object
	{
	public:
					DeliveryReportCB(ServerLinker& _linker);

			void 	dr_cb (RdKafka::Message &_message) ;

	protected:
		ServerLinker&	linker_;
	};

	class Link : public Object
	{
	public:
		Link(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition = DEFAULT_CONST_MSG_PARTITION);

		
	//	RdKafka::Topic*	GetTopic()	{ return	topic_;	};
const	std::string&	GetTopic()	{ return	topic_name_;	};
		int32_t			GetPartition()	{ return	partition_;	};

		bool			Touch();
		uint32_t		GetLiveTime();

	protected:
		ServerLinker&	linker_;
		std::string		topic_name_;
		RdKafka::Topic*	topic_;
		int32_t			partition_;
		std::string		error_string_;
		Timer			keep_alive_timer_;
	};

	class UpLink: public Link
	{
	public:
					UpLink(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition = DEFAULT_CONST_MSG_PARTITION);

			bool	Start();
			bool	Stop();
			bool	Send(std::string const& _message);

			uint32_t	IncreaseNumberOfOutGoingMessages()	{	return	++number_of_out_going_messages_;	};
			uint32_t	IncreaseNumberOfErrorMessages()		{	return	++number_of_error_messages_;	};

			uint32_t	NumberOfOutGoingMessages()	{	return	number_of_out_going_messages_;	};
			uint32_t	NumberOfErrorMessages()		{	return	number_of_error_messages_;	};

	protected:
		uint32_t		number_of_out_going_messages_;
		uint32_t		number_of_error_messages_;
	};

	class DownLink: public Link
	{
	public:
		class	ConsumeCB : public RdKafka::ConsumeCb
		{
		public:
			ConsumeCB(ServerLinker::DownLink& _link) : ConsumeCb(), link_(_link) {};

		protected:
				void	consume_cb(RdKafka::Message& _msg, void *opaque);
			DownLink&	link_;
		};

					DownLink(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition = DEFAULT_CONST_MSG_PARTITION);

			bool		Start();
			bool		Stop();

			bool		Consume(RdKafka::ConsumeCb* _consum_cb);

			ConsumeCB*	GetConsumCB()	{	return	&message_cb_;	};

			int64_t		GetOffset()		{	return	offset_;	};
			int64_t		SetOffset(int64_t _offset)		{	return	offset_ = _offset;	};

			uint32_t	IncreaseNumberOfIncommingMessages()	{	return	++number_of_incomming_messages_;	};
			uint32_t	IncreaseNumberOfErrorMessages()		{	return	++number_of_error_messages_;	};

			uint32_t	NumberOfIncommingMessages()	{	return	number_of_incomming_messages_;	};
			uint32_t	NumberOfErrorMessages()		{	return	number_of_error_messages_;	};
			
	protected:

		ConsumeCB		message_cb_;
		int64_t			offset_;

		uint32_t		number_of_incomming_messages_;
		uint32_t		number_of_error_messages_;
	};

	class	ConsumeCB : public RdKafka::ConsumeCb
	{
	public:
		friend	class	ServerLinker;
		ConsumeCB(ServerLinker& _linker) : ConsumeCb(), linker_(_linker) {};

	protected:
			void	consume_cb(RdKafka::Message& _msg, void *opaque);
		ServerLinker&	linker_;
	};

						ServerLinker(ObjectManager* _manager= NULL);
						~ServerLinker();

			bool		SetProperty(JSONNode const& _property, bool _check = false); 
			virtual		operator JSONNode() const;

	const std::string&	GetGlobalUpTopic();		
			bool		SetGlobalUpTopic(std::string const& _topic, bool _check = false);
	const std::string&	GetGlobalDownTopic();		
			bool		SetGlobalDownTopic(std::string const& _topic, bool _check = false);

			bool		SetHashAlg(std::string const& _name, bool _check = false);

			bool		GetSecretKey(std::string & _secret_key);
			bool		SetSecretKey(std::string const& _secret_key, bool _check = false);

	const std::string&	GetBroker();
			bool		SetBroker(std::string const& _broker, bool _check = false);

			bool		SetAutoConnection(bool _auto);
			bool		SetAutoConnection(std::string const& _auto, bool _check = false);

			UpLink*		AddUpLink(std::string const& _topic_name, int32_t _partition = DEFAULT_CONST_MSG_PARTITION);
			bool		DelUpLink(std::string const& _topic_name);
			UpLink*		GetUpLink(std::string const& _topic_name);
			uint32_t	GetUpLink(std::vector<UpLink*>& _link_array);
			uint32_t	GetUpLink(std::list<std::string>& _topic_name_list);

			DownLink*	AddDownLink(std::string const& _topic_name, int32_t _partition = DEFAULT_CONST_MSG_PARTITION);
			bool		DelDownLink(std::string const& _topic_name);
			DownLink*	GetDownLink(std::string const& _topic_name);
			uint32_t	GetDownLink(std::vector<DownLink*>& _link_array);
			uint32_t	GetDownLink(std::list<std::string>& _topic_name_list);

			bool		Start();
			bool		Connect(uint32_t _delay_sec = 0);
			bool		Disconnect();
			bool		IsConnected();

			bool		Send(RCSMessage const& _message);
			bool		Send(std::string const& _message);
			bool		Send(std::string const& _msg_type, JSONNode& _payload);
			bool		Send(std::string const& _msg_type, std::string const& _req_id,  JSONNode& _payload);

			bool		KeepAlive(Node* _object);
			bool		ResetKeepAliveTimer();
			bool		KeepAliveEnable(bool _enable);


			bool		AddNode(std::string const& _id);
			bool		AddNode(Node* _node);
			bool		GetNode(std::string const& _id);
			bool		ConfirmNode(std::string const& _req_id, Node* _node);

			bool		RequestInit(std::string const& _type, JSONNode& _payload);
			bool		ReplyInit(std::string const& _type, std::string const& _req_id, JSONNode& _payload);
			bool		AddGateway(JSONNode& _payload, Gateway* _gateway, Fields const& _fields);
			bool		AddGateway(JSONNode& _payload, std::string const& _id);
			bool		AddDevice(JSONNode& _payload, Device* _device, Fields const& _fields);
			bool		AddDevice(JSONNode& _payload, std::string const& _id);
			bool		AddEndpoint(JSONNode& _payload, Endpoint* _endpoint, Fields const& _fields);
			bool		AddEndpoint(JSONNode& _payload, std::string const& _id);

			bool		OnMessage(Message* _message);
			bool		GetEPDataInfo(Endpoint* _ep);
			bool		ConfirmEPDataInfo(Endpoint* _ep);
			bool		ReportEPData(Endpoint* _ep);
			bool		AddEPData(JSONNode& _payload, Endpoint* _ep);
			bool		AddEPData(JSONNode& _payload, Endpoint* _ep, uint32_t _lower_bound, uint32_t _upper_bound);

			bool		Error(std::string const& _req_id, std::string const& _err_msg);


protected:
	RdKafka::Producer*	GetProducer()	{	return	producer_;	};
	RdKafka::Consumer*	GetConsumer()	{	return	consumer_;	};
	RdKafka::Topic*		CreateProducerTopic(std::string const& _topic_name, std::string& _error_string);
	RdKafka::Topic*		CreateConsumerTopic(std::string const& _topic_name, std::string& _error_string);

			void		Preprocess();
			void		Process();
			void		Postprocess();
			bool		InternalConnect(uint32_t _delay_sec = 0);
			bool		InternalDisconnect();

			bool		OnProduce(Produce* _produce);
			void		OnConsume(Consume* _consume);
			void		OnConsumeAdd(Message* _message);
			void		OnConsumeDel(Message* _message);
			void		OnConsumeGet(Message* _message);
			void		OnConsumeSet(Message* _message);
			void		OnConsumeConfirm(Message* _message);
			void		OnConsumeError(Message* _message);

			void		OnAddGateway(JSONNode const& _node, JSONNode& _reply);
			void		OnDelGateway(JSONNode const& _node, JSONNode& _reply);
			void		OnGetGateway(JSONNode const& _node, JSONNode& _reply);
			void		OnSetGateway(JSONNode const& _node, JSONNode& _reply);
			void		OnConfirmGateway(JSONNode const& _node, std::string const& _req_type);

			void		OnAddDevice(JSONNode const& _node, JSONNode& _reply);
			void		OnDelDevice(JSONNode const& _node, JSONNode& _reply);
			void		OnGetDevice(JSONNode const& _node, JSONNode& _reply);
			void		OnSetDevice(JSONNode const& _node, JSONNode& _reply);
			void		OnConfirmDevice(JSONNode const& _node, std::string const& _req_type);

			void		OnAddEndpoint(JSONNode const& _node, JSONNode& _reply);
			void		OnDelEndpoint(JSONNode const& _node, JSONNode& _reply);
			void		OnGetEndpoint(JSONNode const& _node, JSONNode& _reply);
			void		OnSetEndpoint(JSONNode const& _node, JSONNode& _reply);
			void		OnConfirmEndpoint(JSONNode const& _node, std::string const& _req_type);

			void		OnDelData(JSONNode const& _node, JSONNode& _reply);
			void		OnGetData(JSONNode const& _node, JSONNode& _reply);
			void		OnSetData(JSONNode const& _node, JSONNode& _reply);
			void		OnConfirmEPData(JSONNode const& _node, std::string const& _req_type);
			
			std::string (*secret_code_hash_)(const std::string &string);

			bool		ConfirmRequest(RCSMessage* _reply, std::string& _req_type, bool _exception = true);

	ObjectManager*		manager_;
	std::string			secret_key_;
	std::string			broker_;
	std::string			global_up_topic_;
	std::string			global_down_topic_;

	RdKafka::Conf*		conf_global_;
	RdKafka::Conf*		conf_topic_;

	bool				keep_alive_enable_;
	bool				auto_connection_;
	bool				broker_connected_;
	Time				broker_retry_interval_;
	Timer				broker_retry_timeout_;

	uint64_t			request_timeout_;
	bool				report_late_arrive_message_;

	std::map<std::string, Produce*>		message_map_;
	std::map<uint64_t, Produce*>		request_map_;

	RdKafka::Producer*	producer_;
	std::map<std::string, UpLink*>		up_link_map_;

	RdKafka::Consumer*	consumer_;
	std::map<std::string, DownLink*>	down_link_map_;

	std::string			hash_alg_name_;

	std::string			error_string_;

	EventCB				event_cb_;
	DeliveryReportCB	delivery_report_cb_;
	ConsumeCB			consume_cb_;
};

#endif
