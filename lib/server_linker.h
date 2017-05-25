#ifndef	SERVER_LINKER_H_
#define	SERVER_LINKER_H_

#include <list>
#include "active_object.h"
#include "librdkafka/rdkafkacpp.h"
#include "timer.h"

class	ServerLinker : public ActiveObject
{
public:
	class EventCB : public RdKafka::EventCb, Object
	{
	public:
					EventCB(ServerLinker& _parent);

			void 	event_cb (RdKafka::Event &_event) ;

	protected:
		ServerLinker&	parent_;
	};

	class DeliveryReportCB : public RdKafka::DeliveryReportCb, Object
	{
	public:
					DeliveryReportCB(ServerLinker& _parent);

			void 	dr_cb (RdKafka::Message &_message) ;

	protected:
		ServerLinker&	parent_;
	};

	class Link : public Object
	{
	public:
		Link(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition = RdKafka::Topic::PARTITION_UA);

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
					UpLink(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition = RdKafka::Topic::PARTITION_UA);

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

					DownLink(ServerLinker& _linker, std::string const& _topic_name, int32_t _partition = RdKafka::Topic::PARTITION_UA);

			bool	Start();
			bool	Stop();
	protected:

		ConsumeCB		message_cb_;
	};

						ServerLinker();
						~ServerLinker();

			bool		Load(JSONNode const& _json); 

			virtual		operator JSONNode() const;

			bool		AddBroker(std::string const& _broker);
			bool		DeleteBroker(std::string const& _broker);
			uint32_t	GetBrokerList(std::list<std::string>& _broker_list);

			UpLink*		AddUpLink(std::string const& _topic_name, int32_t _partition = RdKafka::Topic::PARTITION_UA);
			UpLink*		GetUpLink(std::string const& _topic_name);
			bool		DeleteUpLink(std::string const& _topic_name);
			uint32_t	GetUpLinkNameList(std::list<std::string>& _topic_name_list);

			DownLink*	AddDownLink(std::string const& _topic_name, int32_t _partition = RdKafka::Topic::PARTITION_UA);
			DownLink*	GetDownLink(std::string const& _topic_name);
			bool		DeleteDownLink(std::string const& _topic_name);
			uint32_t	GetDownLinkNameList(std::list<std::string>& _topic_name_list);


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

	std::list<std::string>	broker_list_;
	RdKafka::Conf*		conf_global_;
	RdKafka::Conf*		conf_topic_;
	RdKafka::Producer*	producer_;
	RdKafka::Consumer*	consumer_;
	std::string			error_string_;

	std::list<std::string>				up_link_list_;
	std::list<std::string>				down_link_list_;
	std::map<std::string, UpLink*>		up_link_map_;
	std::map<std::string, DownLink*>	down_link_map_;

	EventCB				event_cb_;
	DeliveryReportCB	delivery_report_cb_;
};

#endif
