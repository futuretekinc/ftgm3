#ifndef	SERVER_LINKER_H_
#define	SERVER_LINKER_H_

#include <list>
#include "defined.h"
#include "active_object.h"
#include "librdkafka/rdkafkacpp.h"
#include "timer.h"

#define	MSG_TYPE_SERVER_LINKER			(0x00030000)
#define	MSG_TYPE_SERVER_LINKER_PRODUCE	(MSG_TYPE_SERVER_LINKER + 1)
#define	MSG_TYPE_SERVER_LINKER_CONSUME	(MSG_TYPE_SERVER_LINKER + 2)
#define	MSG_TYPE_SERVER_LINKER_PRODUCE2	(MSG_TYPE_SERVER_LINKER + 3)

class	ObjectManager;

struct	MessageProduce : Message
{
	MessageProduce(std::string const& _sender, std::string const& _topic, int32_t _partition, std::string const& _payload)
	: Message(MSG_TYPE_SERVER_LINKER_PRODUCE, _sender)
	{
		topic = _topic;	
		payload = _payload;	
		partition = _partition;
	}

	std::string		topic;
	std::string		payload;
	int32_t			partition;
};

struct	MessageProduce2 : Message
{
	MessageProduce2(std::string const& _sender, std::string const& _topic, JSONNode const& _payload)
	: Message(MSG_TYPE_SERVER_LINKER_PRODUCE2, _sender), topic(_topic), payload(_payload) 
	{
	}

	std::string		topic;
	JSONNode		payload;
};

struct	MessageConsume : Message
{
	MessageConsume(std::string const& _sender, std::string const& _topic, JSONNode& _payload);

	std::string	topic;
	JSONNode	payload;
};

class	ServerLinker : public ActiveObject
{
public:
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

		
		RdKafka::Topic*	GetTopic()	{ return	topic_;	};
		int32_t			GetPartition()	{ return	partition_;	};
	protected:
		ServerLinker&	linker_;
		std::string		topic_name_;
		RdKafka::Topic*	topic_;
		int32_t			partition_;
		std::string		error_string_;
	};

	class UpLink: public Link
	{
	public:
					UpLink(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition = DEFAULT_CONST_MSG_PARTITION);

			bool	Start();
			bool	Stop();
			bool	Send(std::string const& _message);
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
	protected:

		ConsumeCB		message_cb_;
		int64_t			offset_;
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

			bool		Load(JSONNode const& _json); 

			virtual		operator JSONNode() const;

			bool		AddBroker(std::string const& _broker);
			bool		DeleteBroker(std::string const& _broker);
			uint32_t	GetBrokerList(std::list<std::string>& _broker_list);

			UpLink*		AddUpLink(std::string const& _topic_name, int32_t _partition = DEFAULT_CONST_MSG_PARTITION);
			UpLink*		GetUpLink(std::string const& _topic_name);
			bool		DeleteUpLink(std::string const& _topic_name);
			uint32_t	GetUpLinkNameList(std::list<std::string>& _topic_name_list);

			DownLink*	AddDownLink(std::string const& _topic_name, int32_t _partition = DEFAULT_CONST_MSG_PARTITION);
			DownLink*	GetDownLink(std::string const& _topic_name);
			bool		DeleteDownLink(std::string const& _topic_name);
			uint32_t	GetDownLinkNameList(std::list<std::string>& _topic_name_list);

			bool		Start();
			bool		Produce(JSONNode const& _message);
			bool		Produce(std::string const& _topic_name, int32_t _partition, std::string const& _message);

protected:
	RdKafka::Producer*	GetProducer()	{	return	producer_;	};
	RdKafka::Consumer*	GetConsumer()	{	return	consumer_;	};
	RdKafka::Topic*		CreateProducerTopic(std::string const& _topic_name, std::string& _error_string);
	RdKafka::Topic*		CreateConsumerTopic(std::string const& _topic_name, std::string& _error_string);

			void		Preprocess();
			void		Process();
			void		Postprocess();

			void		OnMessage(Message* _message);

	ObjectManager*		manager_;
	std::list<std::string>	broker_list_;
	
	Time				connection_retry_interval_;

	RdKafka::Conf*		conf_global_;
	RdKafka::Conf*		conf_topic_;


	RdKafka::Producer*	producer_;
	Timer				producer_retry_timeout_;
	std::map<std::string, UpLink*>		up_link_map_;

	RdKafka::Consumer*	consumer_;
	Timer				consumer_retry_timeout_;
	std::map<std::string, DownLink*>	down_link_map_;

	std::string			error_string_;

	EventCB				event_cb_;
	DeliveryReportCB	delivery_report_cb_;
	ConsumeCB			consume_cb_;
};

#endif
