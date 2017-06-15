#ifndef	GATEWAY_H_
#define	GATEWAY_H_

#include <list>
#include <mutex>
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

	Gateway(ObjectManager& _manager, ValueType const& _type);
	~Gateway();

	// Properties operation

	virtual	bool		SetProperty(Property const& _property, Properties::Fields const& _fields = PROPERTY_ALL);

//	virtual	bool		GetProperties(Properties& _properties, Properties::Fields const& _fields = PROPERTY_ALL);
//	virtual	bool		GetProperties(JSONNode& _properties, Properties::Fields const& _fields = PROPERTY_ALL);

			uint32_t	GetDeviceCount();
	const 	ValueID&	GetDeviceAt(int index);
			bool		GetDeviceList(std::list<ValueID>& _list);
			bool		GetDeviceMap(std::map<std::string, Device*>& _map);

	// Utility
	virtual				operator JSONNode();

	static	const	ValueType&	Type();
	static	bool		IsValidType(std::string const& _type);
	static	Gateway*	Create(ObjectManager& _manager, Properties const& _properties);
	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);

protected:

			void		Preprocess();
			void		Process();
			void		Postprocess();

	// Device operation
			bool		IsAttached(ValueID const& _device_id);
	virtual	bool		Attach(ValueID const& _device_id);
			bool		Detach(ValueID const& _device_id);
			bool		Detach();

	std::list<ValueID>	device_id_list_;
};

#include "gateway_gen.h"

#endif
