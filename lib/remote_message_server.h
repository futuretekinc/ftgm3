#ifndef	REMOTE_MESSAGE_SERVER_H_
#define	REMOTE_MESSAGE_SERVER_H_

#include <string>
#include <map>
#include <sstream>
#include "active_object.h"
#include "device.h"
#include "data_manager.h"
#include "message.h"
#include "tcp_session.h"

class	RemoteMessageServer : public TCPSession
{
	class	DeviceMessageService : public Object
	{
	public:
		bool	Service(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Add(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayAdd(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Del(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayDel(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	List(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Get(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayGet(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Set(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
	};

	class	EndpointMessageService : public Object
	{
	public:
		bool	Service(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Add(RemoteMessageServer& _rms, 	JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayAdd(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Del(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayDel(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	List(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Get(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayGet(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Set(RemoteMessageServer& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
	};
public:
	RemoteMessageServer(TCPServer *_server, int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout);
	~RemoteMessageServer();

			void		OnMessage(Message* _base_message);

protected:
			bool		Call(JSONNode& _request, JSONNode& _response);
			JSONNode	RequestParse(const char* _message);

	virtual	void		Preprocess();
	virtual	void		Process();
	virtual	void		Postprocess();

	DeviceMessageService	dms_;
	EndpointMessageService	ems_;
	ObjectManager*			object_manager_;
};

#endif
