#ifndef	TCP_SERVER_H_
#define	TCP_SERVER_H_

#include <sys/socket.h> 
#include <arpa/inet.h>
#include <map>
#include <list>
#include <libjson/libjson.h>
#include "process_object.h"
#include "message.h"
#include "locker.h"
#include "tcp_session.h"

class	TCPSession;
class	ObjectManager;

class	TCPServer : public ProcessObject
{
public:
	struct MessageSessionDisconnected : Message
	{
		uint16_t	port;
		MessageSessionDisconnected(uint16_t _port) : Message(MSG_TYPE_SESSION_DISCONNECTED), port(_port) {};
	};

						TCPServer(ObjectManager* _manager);
						~TCPServer();

			bool		LoadField(JSONNode const& _json, bool _check = false);

						operator JSONNode() const;

			uint16_t	GetPort();
			bool		SetPort(uint16_t _port, bool _check = false);
			bool		SetPort(std::string const& _port, bool _check = false);

			uint32_t	GetMaxSessionCount();
			bool		SetMaxSessionCount(uint32_t _count, bool _check = false);
			bool		SetMaxSessionCount(std::string const& _count, bool _check = false);

			uint32_t	GetTimeout();
			bool		SetTimeout(uint32_t	_timeout, bool _check = false);
			bool		SetTimeout(std::string const& _timeout, bool _check = false);

			ObjectManager*	GetManager()	{	return	manager_;	};

			bool		SetProperty(JSONNode const& _property, bool _check =- false);

	virtual	TCPSession*	CreateSession(int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout);
			bool		SessionDisconnected(uint16_t _port);
			bool		PacketReceived(uint16_t _port, void* _data, uint32_t _length);

			uint32_t	GetSessionCount();
			bool		GetSessionInformationList(std::list<TCPSession::Information>& _information_list);

			bool		OnMessage(Message *_message);
protected:

			void		Preprocess();
			void		Process();
			void		Postprocess();

			uint16_t	port_;
			uint32_t	max_session_count_;
			uint32_t	timeout_;

			ObjectManager*	manager_;
			int			socket_;	

			bool		(*onMessageCallback_)(void*, Message*);
			Locker		session_map_locker_;
			std::map<uint16_t, TCPSession*>	session_map_;
};

#endif
