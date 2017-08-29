#ifndef	GATEWAY_H_
#define	GATEWAY_H_

#include <list>
#include <libjson/libjson.h>
#include "node.h"
#include "timer.h"
#include "property.h"
#include "KompexSQLiteStatement.h"
#include "value.h"

class	Device;
class	ObjectManager;

class	Gateway : public Node
{
	friend class	Device;
	friend class	ObjectManager;

public:

	Gateway(ObjectManager& _manager, std::string const& _type);
	~Gateway();

	// Properties operation

	virtual	bool		SetProperty(JSONNode const& _property, bool _check = false);

			uint32_t	GetDeviceCount();
	const 	std::string&	GetDeviceAt(int index);
			bool		GetDeviceList(std::list<std::string>& _list);
			bool		GetDeviceMap(std::map<std::string, Device*>& _map);

	// Utility
	virtual				operator JSONNode();

	static	const char*	Type();
	static	bool		IsIncludeIn(Object *_object);

	static	Gateway*	Create(ObjectManager& _manager, JSONNode const& _properties);
	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);
	//add SY.KANG
	 	void    	InfoProcess();
	//

protected:

			void		Preprocess();
			void		Process();
			void		Postprocess();

	// Device operation
			bool		IsAttached(std::string const& _device_id);
	virtual	bool		Attach(std::string const& _device_id);
			bool		Detach(std::string const& _device_id);
			bool		Detach();
			//add SY.KANG
			uint32_t	gateway_info_interval_;
			Timer		gateway_info_timer_;
			//
	std::list<std::string>	device_id_list_;
};

#include "gateway_gen.h"

#endif
