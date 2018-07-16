#ifndef	SERVER_LINKER_H_
#define	SERVER_LINKER_H_

#include <list>
#include "defined.h"
#include "process_object.h"
#include "timer.h"
#include "rcs_message.h"
#include "locker.h"

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
			~Produce();

			const 	std::string&	GetTopic()		{	return	topic_;	}
					RCSMessage&		GetMessage()	{	return	message_;}

					uint32_t		GetTransmissionCount()		{	return	transmission_count_;	};
					uint32_t		IncTransmissionCount()		{	return	++transmission_count_;	};
		protected:
			std::string	topic_;
			RCSMessage	message_;
			uint32_t	transmission_count_;
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

	class Link : public Object
	{
	public:
		Link(ServerLinker* _linker, std::string const& _topic);

		
const	std::string&	GetTopic()	{ return	topic_;	};

		bool			Touch();
		uint32_t		GetLiveTime();

	protected:
		ServerLinker*	linker_;
		std::string		topic_;
		bool			connected_;
		std::string		error_string_;
		Timer			keep_alive_timer_;
	};

	class UpLink: public Link
	{
	public:
					UpLink(ServerLinker* _linker, std::string const& _topic);

		virtual	bool	Connect();
		virtual	bool	Disconnect();
		virtual	bool	Send(std::string const& _message);
		virtual	bool	Send(std::string const& _message, std::string& _msg_id);

			uint32_t	GetQoS()	{	return	qos_;	}
			bool		SetQoS(uint32_t _qos);

			uint32_t	IncreaseNumberOfOutGoingMessages()	{	return	++number_of_out_going_messages_;	};
			uint32_t	IncreaseNumberOfErrorMessages()		{	return	++number_of_error_messages_;	};

			uint32_t	NumberOfOutGoingMessages()	{	return	number_of_out_going_messages_;	};
			uint32_t	NumberOfErrorMessages()		{	return	number_of_error_messages_;	};

	protected:
		uint32_t		qos_;

		uint32_t		number_of_out_going_messages_;
		uint32_t		number_of_error_messages_;
	};

	class DownLink: public Link
	{
	public:
					DownLink(ServerLinker* _linker, std::string const& _topic);

		virtual	bool	Connect();
		virtual	bool	Disconnect();

			uint32_t	IncreaseNumberOfIncommingMessages()	{	return	++number_of_incomming_messages_;	};
			uint32_t	IncreaseNumberOfErrorMessages()		{	return	++number_of_error_messages_;	};

			uint32_t	NumberOfIncommingMessages()	{	return	number_of_incomming_messages_;	};
			uint32_t	NumberOfErrorMessages()		{	return	number_of_error_messages_;	};
			
	protected:

		uint32_t		number_of_incomming_messages_;
		uint32_t		number_of_error_messages_;
	};

						ServerLinker(ObjectManager* _manager= NULL);
	virtual				~ServerLinker();

	virtual	bool		SetProperty(JSONNode const& _property, bool _check = false); 
	virtual				operator JSONNode() const;

	const std::string&	GetTopicVersion();		
			bool		SetTopicVersion(std::string const& _version, bool _check = false);

	const std::string&	GetTopicID();		
			bool		SetTopicID(std::string const& _id, bool _check = false);

	const std::string&	GetGlobalUpName();		
			bool		SetGlobalUpName(std::string const& _name, bool _check = false);
	const std::string&	GetGlobalDownName();		
			bool		SetGlobalDownName(std::string const& _name, bool _check = false);

	const std::string&	GetGlobalUpTopic();		
	const std::string&	GetGlobalDownTopic();		

			bool		SetHashAlg(std::string const& _name, bool _check = false);

			bool		GetSecretKey(std::string & _secret_key);
			bool		SetSecretKey(std::string const& _secret_key, bool _check = false);

	const std::string&	GetBroker();
			bool		SetBroker(std::string const& _broker, bool _check = false);
			
			//Add SY.KANG
			bool		SetUsername(std::string const& _username, bool _check = false);
			bool		SetPassword(std::string const& _password, bool _check = false);
			bool		SetPort(std::string const& _port, bool _check = false);
			bool		SetKeepAliveInterval(std::string const& _keep_alive_interval, bool _check = false);
			bool		SetCafile(std::string	const& _cafile, bool _check = false);
			//

			bool		SetAutoConnection(bool _auto);
			bool		SetAutoConnection(std::string const& _auto, bool _check = false);

	virtual	UpLink*		CreateUpLink(std::string const& _topic);
			UpLink*		AddUpLink(std::string const& _topic);
			bool		DelUpLink(std::string const& _topic);
			UpLink*		GetUpLink(std::string const& _topic);
			uint32_t	GetUpLink(std::vector<UpLink*>& _link_array);
			uint32_t	GetUpLink(std::list<std::string>& _topic_list);

	virtual	DownLink*	CreateDownLink(std::string const& _topic);
			DownLink*	AddDownLink(std::string const& _topic);
			bool		DelDownLink(std::string const& _topic);
			DownLink*	GetDownLink(std::string const& _topic);
			uint32_t	GetDownLink(std::vector<DownLink*>& _link_array);
			uint32_t	GetDownLink(std::list<std::string>& _topic_list);

	virtual	bool		Start(uint32_t _timeout = 1000);

	virtual	bool		Connect(uint32_t _delay_sec = 0);
	virtual	bool		Disconnect();
			bool		IsConnected();

			bool		Send(RCSMessage const& _message);

			bool		KeepAlive(Node* _object);
			bool		ResetKeepAliveTimer();
			bool		KeepAliveEnable(bool _enable);

			bool		RequestInit(std::string const& _type, JSONNode& _payload);
			bool		ReplyInit(std::string const& _type, std::string const& _req_id, JSONNode& _payload);
			bool		AddGateway(JSONNode& _payload, Gateway* _gateway, Fields const& _fields);
			bool		AddGateway(JSONNode& _payload, std::string const& _id);
			bool		AddDevice(JSONNode& _payload, Device* _device, Fields const& _fields);
			bool		AddDevice(JSONNode& _payload, std::string const& _id);
			bool		AddEndpoint(JSONNode& _payload, Endpoint* _endpoint, Fields const& _fields);
			bool		AddEndpoint(JSONNode& _payload, std::string const& _id);
			bool		AddEPData(JSONNode& _payload, Endpoint* _ep);
			bool		AddEPData(JSONNode& _payload, Endpoint* _ep, uint32_t _lower_bound, uint32_t _upper_bound);

	virtual	bool		OnMessage(Message* _message);
			bool		ReportEPData(Endpoint* _ep);

			bool		Error(std::string const& _req_id, std::string const& _err_msg);
			bool		MessageDumpEnable(bool _enable);

protected:
			bool		SetGlobalUpTopic(std::string const& _version, std::string const& _name, std::string const& _id, bool _check = false);
			bool		SetGlobalDownTopic(std::string const& _version, std::string const& _name, std::string const& _id, bool _check = false);

	virtual	void		Preprocess();
	virtual	void		Process();
	virtual	void		Postprocess();

	virtual	bool		OnProduce(Produce* _produce);
	virtual	void		OnConsume(Consume* _consume);

	virtual	bool		ConfirmRequest(RCSMessage const & _reply, std::string& _req_type);

			std::string (*secret_code_hash_)(const std::string &string);

	ObjectManager*		manager_;
	std::string			secret_key_;

	bool				auto_connection_;

	std::string			broker_;
	bool				broker_connected_;
	Time				broker_retry_interval_;
	Timer				broker_retry_timeout_;
	
	//add SY.KANG
	std::string			username_;
	std::string			password_;
	std::string			port_;
	std::string			cafile_;
	//

	uint32_t			retransmission_count_max_;

	std::string			topic_version_;
	std::string			global_up_name_;
	std::string			global_down_name_;
	std::string			topic_id_;
	std::string			global_up_topic_;
	std::string			global_down_topic_;

	bool				keep_alive_enable_;
	uint32_t			keep_alive_interval_;

	bool				enable_confirm_;
	uint64_t			request_timeout_;
	Locker				request_map_locker_;
	std::map<uint64_t, Produce*>		request_map_;

	bool				report_late_arrive_message_;

	std::map<std::string, Produce*>		message_map_;

	std::map<std::string, UpLink*>		up_link_map_;

	std::map<std::string, DownLink*>	down_link_map_;

	std::string			hash_alg_name_;

	bool				message_dump_;
	std::string			error_string_;
};

#endif
