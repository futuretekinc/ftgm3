#ifndef	OBJECT_MANAGER_H_
#define	OBJECT_MANAGER_H_

#include <string>
#include <map>
#include <sstream>
#include "active_object.h"
#include "gateway.h"
#include "device.h"
#include "data_manager.h"
#include "message.h"
#include "rcs_server.h"
#include "rcs_session.h"
#include "server_linker.h"

class	ObjectManager : public ActiveObject
{
	friend	class	Node;
	friend	class	Gateway;
	friend	class	Device;
	friend	class	Endpoint;
public:
						ObjectManager();
						ObjectManager(ValueID const& _id);
						~ObjectManager();

			bool		GetAutoStart();
			bool		SetAutoStart(bool _auto_start);

			Time const&	GetEndpointReportInterval() const;
			bool		SetEndpointReportInterval(Time const& _time);

			bool		LoadFromFile(std::string const& _file_name);
			bool		Load(const char *_json);
			bool		Load(JSONNode const& _json);

						operator JSONNode() const;

			uint32_t	GetNodeCount();
			uint32_t	GetNodeList(std::list<Node*>& _list);
			uint32_t	GetNodeList(ValueType const& _type, std::list<Node*>& _list);
			Node*		GetNode(std::string const& _id);

			bool		Attach(Gateway* _device);
			bool		Detach(Gateway* _device);
			Gateway*	CreateGateway(JSONNode const& _json);
			Gateway*	CreateGateway(Properties const& _properties, bool from_db = false);
			bool		DestroyGateway(std::string const& _id);
			uint32_t	GetGatewayCount();
			uint32_t	GetGatewayList(std::list<Gateway*>& _device_list);
			uint32_t	GetGatewayList(ValueType const& _type, std::list<Gateway*>& _device_list);
			Gateway*	GetGateway(std::string const& _id);

			bool		Attach(Device* _device);
			bool		Detach(Device* _device);
			Device*		CreateDevice(JSONNode const& _json);
			Device*		CreateDevice(Properties const& _properties, bool from_db = false);
			bool		DestroyDevice(std::string const& _id);
			uint32_t	GetDeviceCount();
			uint32_t	GetDeviceList(std::list<Device*>& _device_list);
			uint32_t	GetDeviceList(ValueType const& _type, std::list<Device*>& _device_list);
			Device*		GetDevice(std::string const& _id);

			bool		Attach(Endpoint* _endpoint);
			bool		Detach(Endpoint* _endpoint);
			Endpoint*	CreateEndpoint(JSONNode const& _properties, bool from_db = false);
			Endpoint*	CreateEndpoint(Properties const& _properties, bool from_db = false);
			bool		DestroyEndpoint(std::string const& _id);
			uint32_t	GetEndpointCount();
			uint32_t	GetEndpointList(std::list<Endpoint*>& _endpoint_list);
			Endpoint*	GetEndpoint(std::string const& _id);

			// Data Manager
			bool		Attach(DataManager* _data_manager);

			// External Service Call Server
			bool		Attach(RCSSession* _rms);
			bool		Detach(RCSSession* _rms);

			// External Service Call Server

			std::string	GetTopicNameGateway(std::string const& _id);
			std::string	GetTopicNameDevice(std::string const& _id);
			std::string	GetTopicNameEndpoint(std::string const& _id);

			DataManager&	GetDataManager()	{	return	data_manager_;	};
			ServerLinker&	GetServerLinker()	{	return	server_linker_;	};
			RCSServer&		GetRCSServer()		{	return	rcs_server_;	};

#if 0
			bool		RemoteServiceCall(RCSMessage& _request, RCSMessage& _response);

			bool		RCSAdd(RCSMessage& _request, RCSMessage& _response);
			bool		RCSDel(RCSMessage& _request, RCSMessage& _response);
			bool		RCSGet(RCSMessage& _request, RCSMessage& _response);
			bool		RCSSet(RCSMessage& _request, RCSMessage& _response);
			bool		RCSList(RCSMessage& _request, RCSMessage& _response);
			bool		RCSConfirm(RCSMessage& _response, std::string& _req_type);
			bool		RCSError(RCSMessage& _response);
			bool		RCSError(RCSMessage& _response, std::string& _req_type);

			bool		RCSGetGateway(JSONNode& _node, JSONNode& _response);
			bool		RCSSetGateway(JSONNode& _node, JSONNode& _response);
			bool		RCSConfirmGateway(JSONNode& _node, std::string& _req_type);

			bool		RCSGetDevice(JSONNode& _node, JSONNode& _response);
			bool		RCSSetDevice(JSONNode& _node, JSONNode& _response);
			bool		RCSConfirmDevice(JSONNode& _node, std::string& _req_type);

			bool		RCSGetEndpoint(JSONNode& _node, JSONNode& _response);
			bool		RCSSetEndpoint(JSONNode& _node, JSONNode& _response);
			bool		RCSConfirmEndpoint(JSONNode& _node, std::string& _req_type);

			bool		RCSGetData(JSONNode& _node, JSONNode& _response);
			bool		RCSSetData(JSONNode& _node, JSONNode& _response);
			bool		RCSConfirmData(JSONNode& _node, std::string& _req_type);
#endif
protected:
	
			bool		OnMessage(Message* _message);

			bool		ServiceCall(JSONNode& _request, JSONNode& _response);

			bool		KeepAlive(Node* _object);

			bool		SendMessage(std::string const& _topic, std::string const& _message);

			bool		IDChanged(Node* _node, ValueID const& _old_id);
			bool		IDChanged(Gateway* _gateway, ValueID const& _old_id);
			bool		IDChanged(Device* _device, ValueID const& _old_id);
			bool		IDChanged(Endpoint* _endpoint, ValueID const& _old_id);

			bool		UpdateProperties(Object* _object);
			bool		UpdateProperties(Gateway* _gateway);
			bool		UpdateProperties(Device* _device);
			bool		UpdateProperties(Endpoint* _endpoint);

			bool		AddNodeToMessage(JSONNode& _payload, Node* _node, Properties::Fields const& _fields);
			bool		AddGatewayToMessage(JSONNode& _payload, Gateway* 	_gateway, 	Properties::Fields const& _fields);
			bool		AddGatewayToMessage(JSONNode& _payload, std::string const& _id);
			bool		AddDeviceToMessage(JSONNode& _payload, 	Device* 	_device, 	Properties::Fields const& _fields);
			bool		AddDeviceToMessage(JSONNode& _payload, std::string const& _id);
			bool		AddEndpointToMessage(JSONNode& _payload,Endpoint* 	_endpoint, 	Properties::Fields const& _fields);
			bool		AddEndpointToMessage(JSONNode& _payload, std::string const& _id);
			bool		AddEPDataToMessage(JSONNode& _payload, Endpoint* _ep);
			bool		AddEPDataToMessage(JSONNode& _payload, Endpoint* _ep, uint32_t _lower_bound, uint32_t _upper_bound);

	virtual	void		Preprocess();
	virtual	void		Process();
	virtual	void		Postprocess();

	bool				auto_start_;
	Time				endpoint_report_interval_;
	Timer				endpoint_report_timer_;

	DataManager							data_manager_;
	ServerLinker						server_linker_;
	RCSServer							rcs_server_;
	std::map<std::string,RCSSession*>	rms_map_;
	std::map<std::string,Gateway*>		gateway_map_;
	std::map<std::string,Device*>		device_map_;
	std::map<std::string,Endpoint*>		endpoint_map_;

};

#endif
