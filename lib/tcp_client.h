#ifndef	TCP_CLIENT_H_
#define	TCP_CLIENT_H_

#include <sys/socket.h> 
#include <arpa/inet.h>
#include <list>
#include "active_object.h"
#include "locker.h"

class	TCPSession;

class	TCPClient : public ActiveObject
{
public:
	struct	Frame
	{
		uint32_t	len;
		uint8_t*	data;

		Frame(uint8_t *_data, uint32_t _len);
		~Frame();
	};	

	TCPClient();
	TCPClient(std::string const& _ip, uint16_t _port);
	~TCPClient();

	bool		Connect();
	bool		Connect(const std::string& _server_ip, uint16_t _server_port = 8888);
	bool		Disconnect();
	bool		IsConnected();

	bool		Send(std::string const& _message);
	bool		Send(const void*	_frame, uint32_t	_frame_len);
	bool		Receive(uint32_t&	_buffer_len);
	bool		Receive(void*	_frame, uint32_t&	_frame_len);
	bool		RequestAndReply(std::string const& _request, std::string& _reply, uint32_t _timeout = 5 * TIME_SECOND);
	uint32_t	GetReceivedPacketCount();

	void		SetMessageProcessID(ValueID const& _process_id);

protected:

	void		Preprocess();
	void		Process();
	void		Postprocess();

	ValueID					message_process_id_;
	Locker					locker_;
	int						socket_;	
	std::string				server_ip_;
	uint16_t				server_port_;
	std::list<Frame*>		receive_packet_list_;
};

#endif
