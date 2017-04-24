#ifndef	TCP_SERVER_H_
#define	TCP_SERVER_H_

#include <sys/socket.h> 
#include <arpa/inet.h>
#include <map>
#include <list>
#include <libjson/libjson.h>
#include "active_object.h"
#include "message.h"
#include "locker.h"
#include "tcp_session.h"

class	TCPSession;

class	TCPServer : public ActiveObject
{
public:
	struct MessageSessionDisconnected : Message
	{
		uint16_t	port;
		MessageSessionDisconnected(uint16_t _port) : Message(SESSION_DISCONNECTED), port(_port) {};
	};

	TCPServer();
	~TCPServer();

	void		Attach(ObjectManager* _object_managerl);

	bool		SetProperty(Property const& _property, bool create = false);

	bool		SessionDisconnected(uint16_t _port);
	bool		PacketReceived(uint16_t _port, void* _data, uint32_t _length);

	uint32_t	GetSessionCount();
	bool		GetSessionInformationList(std::list<TCPSession::Information>& _information_list);

	void		OnMessage(Message *_message);
protected:

	void		Preprocess();
	void		Process();
	void		Postprocess();

	uint16_t	port_;
	uint32_t	max_session_count_;
	uint32_t	timeout_;

	ObjectManager*	object_manager_;
	int			socket_;	
	
	bool		(*onMessageCallback_)(void*, Message*);
	Locker		session_map_locker_;
	std::map<uint16_t, TCPSession*>	session_map_;
};

#endif
