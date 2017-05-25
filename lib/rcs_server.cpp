#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "defined.h"
#include "rcs_server.h"
#include "rcs_session.h"
#include "trace.h"
#include "message.h"
#include "object_manager.h"


/////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////
RCSServer::RCSServer(ObjectManager* _manager)
: TCPServer(_manager)
{
	trace.SetClassName(GetClassName());
	enable_	= true;
	name_ 	= "rcs_server";
}

bool	RCSServer::Load(JSONNode const& _json)
{
	bool	ret_value = true;

	if ((_json.name() == TITLE_NAME_RCS_SERVER) || (_json.name().size() == 0))
	{
		if (_json.type() == JSON_NODE)
		{
			for(auto it = _json.begin(); it != _json.end() ; it++)
			{
				ret_value = Load(*it);
				if (!ret_value)
				{
					std::cout << "Invalid format" << std::endl;
				}
			}
		}
	}
	else
	{
		ret_value = TCPServer::Load(_json);
	}

	return	ret_value;
}

RCSServer::operator JSONNode() const
{
	JSONNode	root;
	
	root.push_back(JSONNode(TITLE_NAME_PORT, port_));
	root.push_back(JSONNode(TITLE_NAME_TIMEOUT, timeout_));
	root.push_back(JSONNode(TITLE_NAME_ENABLE, ((enable_)?"yes":"no")));

	JSONNode	trace_config = trace;

	if (trace_config.size() != 0)
	{
		trace_config.set_name(TITLE_NAME_TRACE);
		root.push_back(trace_config);
	}

	return	root;
}

bool	RCSServer::SetPropertyInternal(Property const& _property, bool create)
{
	return	TCPServer::SetPropertyInternal(_property, create);
}

TCPSession*	RCSServer::CreateSession(TCPServer *_server, int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout)
{
	RCSSession* rcs = new RCSSession(_server, _socket, _addr_info, _timeout);

	manager_->Attach(rcs);
	rcs->SetTrace(true);
	rcs->Start();

	return	rcs;
}


