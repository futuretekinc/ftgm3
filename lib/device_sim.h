#ifndef	DEVICE_SIM_H_
#define	DEVICE_SIM_H_

#include "device_snmp.h"
#include <vector>
#include <map>
#include "endpoint.h"

class	DeviceSIM : public DeviceSNMP
{
public:
	DeviceSIM(ObjectManager& _manager);
	DeviceSIM(ObjectManager& _manager, JSONNode const& _properties);
	DeviceSIM(ObjectManager& _manager, std::string const& _ip);

			bool		IsIncludedIn(std::string const& _type);

	virtual	Endpoint*	CreateEndpoint(JSONNode const& _properties);

			bool		ReadValue(std::string const& _epid, time_t& time, std::string& _value);
			bool		WriteValue(std::string const& _epid, std::string const& _value);

	static	const std::string&	Type();
protected:
	virtual	bool		Attach(std::string const& _endpoint_id);
	virtual	bool		Detach(std::string const& _endpoint_id);


	std::map<std::string, float>		endpoint_sensor_value_table_;
	std::map<std::string, std::string>	endpoint_actuator_value_table_;

};

#endif
