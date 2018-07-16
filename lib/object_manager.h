#ifndef	OBJECT_MANAGER_H_
#define	OBJECT_MANAGER_H_

#include <string>
#include <map>
#include <sstream>
#include "process_object.h"
#include "gateway.h"
#include "device.h"
#include "data_manager.h"
#include "rule_manager.h"
#include "message.h"
#include "rcs_server.h"
#include "rcs_session.h"
#include "server_linker.h"
#include "server_linker_mosquitto.h"

class	ServerLinker;
class	ObjectManager : public ProcessObject
{
	friend	class	Node;
	friend	class	Gateway;
	friend	class	Device;
	friend	class	Endpoint;
public:
						ObjectManager();
						ObjectManager(std::string const& _id);
						~ObjectManager();

			bool		GetAutoStart();
			bool		SetAutoStart(bool _auto_start);

			uint32_t	GetEndpointReportInterval() const;
			bool		SetEndpointReportInterval(Time const& _time);

			bool		SetProperty(JSONNode const& _property, bool _check = false);

						operator JSONNode() const;

			uint32_t	GetNodeCount();
			uint32_t	GetNodeList(std::list<Node*>& _list);
			uint32_t	GetNodeList(std::string const& _type, std::list<Node*>& _list);
			Node*		GetNode(std::string const& _id);
			bool		StartNode(std::string const& _id);
			bool		StopNode(std::string const& _id);

			bool		Attach(Gateway* _device);
			bool		Detach(Gateway* _device);
			Gateway*	CreateGateway(JSONNode const& _properties, bool from_db = false);
			bool		DestroyGateway(std::string const& _id);
			uint32_t	GetGatewayCount();
			uint32_t	GetGatewayList(std::list<Gateway*>& _device_list);
			uint32_t	GetGatewayList(std::string const& _type, std::list<Gateway*>& _device_list);
			Gateway*	GetGateway(std::string const& _id);
			Gateway*	GetMasterGateway();
			bool		StartGateway(std::string const& _id);
			bool		StopGateway(std::string const& _id);

			bool		Attach(Device* _device);
			bool		Detach(Device* _device);
			Device*		CreateDevice(JSONNode const& _properties, bool from_db = false);
			bool		DestroyDevice(std::string const& _id);
			uint32_t	GetDeviceCount();
			uint32_t	GetDeviceList(std::list<Device*>& _device_list);
			uint32_t	GetDeviceList(std::string const& _type, std::list<Device*>& _device_list);
			Device*		GetDevice(std::string const& _id);
			bool		StartDevice(std::string const& _id);
			bool		StopDevice(std::string const& _id);

			bool		Attach(Endpoint* _endpoint);
			bool		Detach(Endpoint* _endpoint);
			Endpoint*	CreateEndpoint(JSONNode const& _properties, bool from_db = false);
			bool		DestroyEndpoint(std::string const& _id);
			uint32_t	GetEndpointCount();
			uint32_t	GetEndpointList(std::list<Endpoint*>& _endpoint_list);
			Endpoint*	GetEndpoint(std::string const& _id);
			bool		StartEndpoint(std::string const& _id);
			bool		StopEndpoint(std::string const& _id);

			Rule*		CreateRule(JSONNode const& _properties, bool from_db = false);
			bool		DestroyRule(std::string const& _id);
			Rule*		GetRule(std::string const& _id);
			uint32_t	GetRuleList(std::list<Rule*>& _list);

			// Data Manager
			bool		Attach(DataManager* _data_manager);

			// External Service Call Server
			bool		Attach(RCSSession* _rms);
			bool		Detach(RCSSession* _rms);

			// External Service Call Server

			std::string	GetTopicNameGateway(std::string const& _id);
			std::string	GetTopicNameDevice(std::string const& _id);
			std::string	GetTopicNameEndpoint(std::string const& _id);
			//add SY.KANG
			bool    	InfoProcess(Node* _object);
			//
			DataManager&	GetDataManager()	{	return	data_manager_;	};
			RuleManager&	GetRuleManager()	{	return	rule_manager_;	};
			ServerLinkerMosq&	GetServerLinker()	{	return	server_linker_;	};
			RCSServer&		GetRCSServer()		{	return	rcs_server_;	};

			void		SystemOperation(uint8_t _operation_type);
			void		SetSystemOperating(std::string const& _msg_type);
			uint8_t		system_operation_;	
protected:
	
			bool		OnMessage(Message* _message);

			bool		ServiceCall(JSONNode& _request, JSONNode& _response);

			bool		KeepAlive(Node* _object);

			bool		SendMessage(std::string const& _topic, std::string const& _message);

			bool		IDChanged(Node* _node, std::string const& _old_id);
			bool		IDChanged(Gateway* _gateway, std::string const& _old_id);
			bool		IDChanged(Device* _device, std::string const& _old_id);
			bool		IDChanged(Endpoint* _endpoint, std::string const& _old_id);

			bool		UpdateProperties(Object* _object);
			bool		UpdateProperties(Gateway* _gateway);
			bool		UpdateProperties(Device* _device);
			bool		UpdateProperties(Endpoint* _endpoint);

	virtual	void		Preprocess();
	virtual	void		Process();
	virtual	void		Postprocess();

	bool				auto_start_;
	uint32_t			endpoint_report_interval_;
	Timer				endpoint_report_timer_;

	DataManager							data_manager_;
	RuleManager							rule_manager_;
	ServerLinkerMosq					server_linker_;
	RCSServer							rcs_server_;
	std::map<std::string,RCSSession*>	rms_map_;
	std::map<std::string,Gateway*>		gateway_map_;
	std::map<std::string,Device*>		device_map_;
	std::map<std::string,Endpoint*>		endpoint_map_;

};

#endif
