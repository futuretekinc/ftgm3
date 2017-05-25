#ifndef	OBJECT_MANAGER_H_
#define	OBJECT_MANAGER_H_

#include <string>
#include <map>
#include <sstream>
#include "active_object.h"
#include "device.h"
#include "data_manager.h"
#include "message.h"
#include "remote_message_server.h"
#include "server_linker.h"

class	ObjectManager : public ActiveObject
{
	friend	class	Device;
	friend	class	Endpoint;
public:
	ObjectManager();
	~ObjectManager();

			bool		Load(std::string const& _file_name);
			bool		Load(const char *_json);
			bool		Load(JSONNode const& _json);

			bool		Attach(Device* _device);
			bool		Detach(Device* _device);
			Device*		CreateDevice(Properties const& _properties, bool from_db = false);
			bool		DestroyDevice(std::string const& _id);
			uint32_t	GetDeviceCount();
			uint32_t	GetDeviceList(std::list<Device*>& _device_list);
			uint32_t	GetDeviceList(ValueType const& _type, std::list<Device*>& _device_list);
			Device*		GetDevice(std::string const& _id);

			bool		Attach(Endpoint* _endpoint);
			bool		Detach(Endpoint* _endpoint);
			Endpoint*	CreateEndpoint(Properties const& _properties, bool from_db = false);
			bool		DestroyEndpoint(std::string const& _id);
			uint32_t	GetEndpointCount();
			uint32_t	GetEndpointList(std::list<Endpoint*>& _endpoint_list);
			Endpoint*	GetEndpoint(std::string const& _id);

			bool		AddData(std::string const& _endpoint_id, Value const* _value);

			bool		Attach(DataManager* _data_manager);

			bool		Attach(RemoteMessageServer* _rms);
			bool		Detach(RemoteMessageServer* _rms);

			bool		UpdateProperties(std::string const& _id);

			void		OnMessage(Message* _message);

			bool		SendKeepAlive(ValueID const& _id);
			bool		SendEndpointReport(ValueID const& _id, std::list<Value*> const& _value_list);
			bool		SendMessage(std::string const& _topic, std::string const& _message);
protected:
	
			bool		IDChanged(Device* _device, ValueID const& _old_id);
			bool		IDChanged(Endpoint* _endpoint, ValueID const& _old_id);

			bool		UpdateProperties(Object* _object);
			bool		UpdateProperties(Device* _device);
			bool		UpdateProperties(Endpoint* _endpoint);

	virtual	void		Preprocess();
	virtual	void		Process();
	virtual	void		Postprocess();

	bool				auto_start_;
	Time				endpoint_report_interval_;
	Timer				endpoint_report_timer_;

	DataManager									data_manager_;
	ServerLinker								server_linker_;
	std::map<std::string,RemoteMessageServer*>	rms_map_;
	std::map<std::string,Device*>				device_map_;
	std::map<std::string,Endpoint*>				endpoint_map_;

};

#endif
