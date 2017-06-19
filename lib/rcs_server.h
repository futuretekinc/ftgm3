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
#include "tcp_server.h"
#include "rcs_session.h"
#include "rcs_message.h"

class	RCSSession;

class	RCSServer : public TCPServer
{
public:

	RCSServer(ObjectManager* _manager);

	bool		SetProperty(JSONNode const& _property, bool _check = false);

				operator JSONNode() const;

	bool		ServiceCall(RCSMessage& _request, RCSMessage& _response);

	bool		Add(RCSMessage& _request, RCSMessage& _response);
	bool		Del(RCSMessage& _request, RCSMessage& _response);
	bool		Get(RCSMessage& _request, RCSMessage& _response);
	bool		Set(RCSMessage& _request, RCSMessage& _response);
	bool		List(RCSMessage& _request, RCSMessage& _response);

	bool		Confirm(RCSMessage& _reply, std::string& _req_type);
	bool		Error(RCSMessage& _reply);
	bool		Error(RCSMessage& _reply, std::string& _req_type);


protected:
	TCPSession*	CreateSession(int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout);

	bool		SetGateway(JSONNode& _node, JSONNode& _result);
	bool		GetGateway(JSONNode& _node, JSONNode& _result);
	bool		ConfirmGateway(JSONNode& _node, std::string& _req_type);

	bool		SetDevice(JSONNode& _node, JSONNode& _result);
	bool		GetDevice(JSONNode& _node, JSONNode& _result);
	bool		ConfirmDevice(JSONNode& _node, std::string& _req_type);

	bool		SetEndpoint(JSONNode& _node, JSONNode& _result);
	bool		GetEndpoint(JSONNode& _node, JSONNode& _result);
	bool		ConfirmEndpoint(JSONNode& _node, std::string& _req_type);
	bool		ConfirmData(JSONNode& _node, std::string& _req_type);

	uint32_t	max_data_get_count_;	
};

#endif
