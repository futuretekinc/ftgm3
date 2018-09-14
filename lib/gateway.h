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
	
	bool    SetCorrectionInterval(std::string const& _interval, bool _check = false);
	bool    SetCorrectionInterval(uint32_t _interval);
 	bool    GetProperty(uint32_t _type, JSONNode& _property);
	// Utility
	virtual				operator JSONNode();

	static	const char*	Type();
	static	bool		IsIncludeIn(Object *_object);

	static	Gateway*	Create(ObjectManager& _manager, JSONNode const& _properties);
	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);
	//add SY.KANG
	 	void    	InfoProcess();
		bool   		GetFTGMVersion(JSONNode& _properties);
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
	std::list<std::string>	device_id_list_;
	uint32_t	correction_interval_;
};

#include "gateway_gen.h"

#endif
