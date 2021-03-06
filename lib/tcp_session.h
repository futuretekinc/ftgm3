#ifndef	TCP_SESSION_H_
#define	TCP_SESSION_H_

#include <sys/socket.h> 
#include <arpa/inet.h>
#include <list>
#include "process_object.h"
#include "timer.h"

class	TCPServer;

class	TCPSession : public ProcessObject
{
public:
	struct	Information
	{
		struct sockaddr_in addr_info;
		Time		start_time;
		uint32_t	timeout;
	};

	TCPSession(TCPServer *_server, int	_socket, struct sockaddr_in *_addr_info, uint32_t _timeout);
	~TCPSession();

			bool	Disconnect();
			bool	Send(std::string const& _message);
			bool	Send(void *data, uint32_t len);

	const 	Information&	GetInformation();
			bool	OnMessage(Message* _message);

protected:
	virtual	bool	OnReceive(void *_data, uint32_t _len);

			void	Preprocess();
			void	Process();
			void	Postprocess();

	TCPServer	*server_;
	int			socket_;
	Information	information_;
	std::string		process_id_;

	uint8_t	*	receive_buffer_;
	uint32_t	receive_buffer_len_;
	int32_t		receive_len_;
	uint8_t *	send_buffer_;	
	uint32_t	send_buffer_len_;
	int32_t		send_len_;
};

#endif
