#ifndef	RCS_SERVER_H_
#define	RCS_SERVER_H_

#include <sys/socket.h> 
#include <arpa/inet.h>
#include <map>
#include <list>
#include <libjson/libjson.h>
#include "active_object.h"
#include "message.h"
#include "locker.h"
#include "rcs_session.h"
#include "tcp_server.h"

class	RCSSession;

class	RCSServer : public TCPServer
{
public:

	RCSServer(ObjectManager* _manager);

	bool		Load(JSONNode const& _json);

				operator JSONNode() const;

protected:
	TCPSession*	CreateSession(TCPServer *_server, int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout);
	bool		SetPropertyInternal(Property const& _property, bool create = false);
};

#endif
