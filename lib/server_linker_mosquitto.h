#ifndef	SERVER_LINKER_MOSQUITTO_H_
#define	SERVER_LINKER_MOSQUITTO_H_

#include "server_linker.h"
#include "mosquitto.h"

class	ServerLinkerMosq : public ServerLinker
{
public:


	class UpLinkMosq: public UpLink
	{
	public:
					UpLinkMosq(ServerLinkerMosq* _linker, std::string const& _topic);

			bool	Send(std::string const& _message);
			bool	Send(std::string const& _message, std::string& _msg_id);

	};

	class DownLinkMosq: public DownLink
	{
	public:
					DownLinkMosq(ServerLinkerMosq* _linker, std::string const& _topic);

			bool		Connect();
	};

						ServerLinkerMosq(ObjectManager* _manager= NULL);
						~ServerLinkerMosq();

	virtual	bool		SetProperty(JSONNode const& _property, bool _check = false); 
	virtual				operator JSONNode() const;

			UpLink*		CreateUpLink(std::string const& _topic);
			
			DownLink*	CreateDownLink(std::string const& _topic);

			bool		Connect(uint32_t _delay_sec = 0);
			bool		Disconnect();

	static	void 		OnConnectCB(struct mosquitto *mosq, void *obj, int result);
	static	void 		OnDisconnectCB(struct mosquitto *mosq, void *obj, int rc);
	static	void 		OnPublishCB(struct mosquitto *mosq, void *obj, int mid);
	static	void		OnSubscribeCB(struct mosquitto *_mosq, void *_obj, int mid, int qos_count, const int *granted_qos);
	static	void		OnMessageCB(struct mosquitto *_mosq, void *_obj, const struct mosquitto_message *_message);
	static	void		OnLogCB(struct mosquitto *mosq, void *obj, int level, const char *str);

protected:
			void		Preprocess();
			void		Process();
			void		Postprocess();
			bool		InternalConnect(uint32_t _delay_sec = 0);
			bool		InternalDisconnect();


	uint32_t			protocol_version_;


	struct mosquitto*	mosquitto_;
};

#endif
