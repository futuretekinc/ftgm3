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

class	ObjectManager : public ActiveObject
{
public:
	ObjectManager();
	~ObjectManager();

			bool		Attach(DataManager* _data_manager);

			bool		Load(std::string const& _file_name);
			bool		Load(const char *_json);
			bool		Load(JSONNode const& _json);

			bool		Attach(Device* _device);
			bool		Detach(Device* _device);
			Device*		CreateDevice(Properties const& _properties, bool from_db = false);
			bool		DestroyDevice(std::string const& _id);
			uint32_t	GetDeviceCount();
			uint32_t	GetDeviceList(std::list<Device*>& _device_list);
			uint32_t	GetDeviceList(Device::Type _type, std::list<Device*>& _device_list);
			Device*		GetDevice(std::string const& _id);

			bool		Attach(Endpoint* _endpoint);
			bool		Detach(Endpoint* _endpoint);
			Endpoint*	CreateEndpoint(Properties const& _properties, bool from_db = false);
			Endpoint*	CreateEndpoint(std::string const& _device_id, Properties const& _properties, bool from_db = false);
			bool		DestroyEndpoint(std::string const& _id);
			uint32_t	GetEndpointCount();
			uint32_t	GetEndpointList(std::list<Endpoint*>& _endpoint_list);
			Endpoint*	GetEndpoint(std::string const& _id);

			bool		AddData(std::string const& _endpoint_id, Date const& _date, Value const* _value);

			bool		Attach(RemoteMessageServer* _rms);
			bool		Detach(RemoteMessageServer* _rms);

			bool		UpdateProperties(std::string const& _id);

			void		OnMessage(Message* _message);
protected:
	
	virtual	void		Preprocess();
	virtual	void		Process();
	virtual	void		Postprocess();

	DataManager*								data_manager_;
	std::map<std::string,RemoteMessageServer*>	rms_map_;
	std::map<std::string,Device*>				device_map_;
	std::map<std::string,Endpoint*>				endpoint_map_;
};

#endif
