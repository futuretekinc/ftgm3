#ifndef	RCS_CLIENT_H_
#define	RCS_CLIENT_H_

#include <map>
#include "process_object.h"
#include "tcp_client.h"
#include "rcs_message.h"

struct GatewayInfo;
struct DeviceInfo;
struct EndpointInfo;

class	RCSClient : public ProcessObject
{
public:

	RCSClient();

			bool		SetServerIP(std::string const& _server_ip);
	const std::string&	GetServerIP();
			bool		SetServerPort(uint16_t _server_ip);
			uint16_t	GetServerPort();

			bool		SetDumpPacket(bool _dump_packet);

			bool	Start(uint32_t _timeout);
			bool	Stop(bool _wait = false);

			bool	Connect();
			bool	Disconnect();
			bool	IsConnected();

			bool	AddGateway(JSONNode const& _properties);
			bool	DelGateway(std::string const& _id);
			bool	GetGateway(std::vector<JSONNode>& _vector);
			bool	GetGateway(std::string const& _id , JSONNode& _properties);
			bool	GetGateway(std::string const& _id , Fields const& _fields, JSONNode& _properties);
			bool	GetGateway(std::list<std::string>& _fields, std::vector<JSONNode>& _vector);
			bool	SetGateway(JSONNode const& _properties);
			bool	StartGateway(std::string const& _id);
			bool	StopGateway(std::string const& _id);
			bool	GetGatewayList(std::vector<std::string>& _id_list);
			bool	SetGatewayEnable(std::string const& _id, bool _enable);
			bool	SetGatewayEnable(std::vector<std::string> const& _id_list, bool _enable);

			bool	AddDevice(JSONNode const& _properties);
			bool	DelDevice(std::string const& _id);
			bool	GetDevice(std::vector<JSONNode>& _vector);
			bool	GetDevice(std::string const& _id , JSONNode& _properties);
			bool	GetDevice(std::string const& _id , Fields const& _fields, JSONNode& _properties);
			bool	GetDevice(std::list<std::string>& _fields, std::vector<JSONNode>& _vector);
			bool	SetDevice(JSONNode const& _properties);
			bool	StartDevice(std::string const& _id);
			bool	StopDevice(std::string const& _id);
			bool	GetDeviceList(std::vector<std::string>& _id_list);
			bool	SetDeviceEnable(std::string const& _id, bool _enable);
			bool	SetDeviceEnable(std::vector<std::string> const& _id_list, bool _enable);
		
			bool	AddEndpoint(JSONNode const& _properties);
			bool	DelEndpoint(std::string const& _id);
			bool	GetEndpoint(std::vector<JSONNode>& _vector);
			bool	GetEndpoint(std::string const& _id , JSONNode& _properties);
			bool	GetEndpoint(std::string const& _id , Fields const& _fields, JSONNode& _properties);
			bool	GetEndpoint(std::list<std::string>& _fields, std::vector<JSONNode>& _vector);
			bool	SetEndpoint(JSONNode const& _properties);
			bool	StartEndpoint(std::string const& _id);
			bool	StopEndpoint(std::string const& _id);
			bool	GetEndpointList(std::vector<std::string>& _id_list);
			bool	SetEndpointEnable(std::string const& _id, bool _enable);
			bool	SetEndpointEnable(std::vector<std::string> const& _id_list, bool _enable);

			bool	DelEPData(std::string const& _id, uint32_t _count);
			bool	DelEPData(std::string const& _id, time_t _start, time_t _end);
			bool	GetEPData(std::string const& _id, uint32_t _count, std::multimap<time_t, std::string>& _value_map);
			bool	GetEPData(std::string const& _id, time_t _start, time_t _end, std::multimap<time_t, std::string>& _value_map);
			bool	SetEPData(std::string const& _id, time_t _time_of_expire, std::string& _value );

			bool	RemoteCall(JSONNode& _request, JSONNode& _result);
			bool	RemoteCall(RCSMessage& _request, RCSMessage& _result);

	virtual	bool	OnMessage(Message* _message);

protected:
	virtual	void	Preprocess();
	virtual	void	Process();
	virtual	void	Postprocess();


	std::string		secret_code_;
	TCPClient		tcp_client_;
	std::string		server_ip_;
	uint16_t		server_port_;

	bool			dump_packet_;
};

#endif
