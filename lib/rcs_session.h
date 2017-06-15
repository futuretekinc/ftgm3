#ifndef	RCS_SESSION_H_
#define	RCS_SESSION_H_

#include <string>
#include <map>
#include <sstream>
#include "active_object.h"
#include "device.h"
#include "data_manager.h"
#include "message.h"
#include "rcs_server.h"
#include "tcp_session.h"

class	RCSServer;
class	RCSSession : public TCPSession
{
public:
	RCSSession(RCSServer *_server, int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout);
	~RCSSession();

			bool		OnMessage(Message* _base_message);

protected:
	virtual	void		Process();
};

#endif
