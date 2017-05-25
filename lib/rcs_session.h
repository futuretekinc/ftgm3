#ifndef	RCS_SESSION_H_
#define	RCS_SESSION_H_

#include <string>
#include <map>
#include <sstream>
#include "active_object.h"
#include "device.h"
#include "data_manager.h"
#include "message.h"
#include "tcp_session.h"

class	RCSSession : public TCPSession
{
	class	DeviceMessageService : public Object
	{
	public:
		bool	Service(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Add(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayAdd(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Del(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayDel(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	List(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Get(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayGet(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Set(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Start(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayStart(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Stop(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayStop(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Enable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayEnable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Disable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayDisable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
	};

	class	EndpointMessageService : public Object
	{
	public:
		bool	Service(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Add(RCSSession& _rms, 	JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayAdd(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Del(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayDel(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	List(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Get(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayGet(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Set(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Start(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayStart(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Stop(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayStop(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Enable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayEnable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	Disable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
		bool	ArrayDisable(RCSSession& _rms, JSONNode& _request, JSONNode& _response, std::ostringstream& _error_message);
	};
public:
	RCSSession(TCPServer *_server, int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout);
	~RCSSession();

			void		OnMessage(Message* _base_message);
			void		SetObjectManager(ObjectManager*	_object_manager);

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
