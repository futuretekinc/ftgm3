#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "defined.h"
#include "json.h"
#include "utils.h"
#include "trace.h"
#include "object_manager.h"
#include "device_snmp.h"
#include "endpoint.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"
#include "snmp_master.h"

using namespace std;

static Locker snmp_locker;
static uint32_t object_count = 0;

#define	SNMP_TIMEOUT	5

DeviceSNMP::DeviceSNMP(ObjectManager& _manager, std::string const& _type)
: DeviceIP(_manager, _type), module_(""), community_("public"), timeout_(SNMP_TIMEOUT), session_()
{
	trace.SetClassName(GetClassName());
}

DeviceSNMP::DeviceSNMP(ObjectManager& _manager, std::string const& _type, JSONNode const& _properties)
: DeviceIP(_manager, _type), module_(""), community_("public"), timeout_(SNMP_TIMEOUT), session_()
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

DeviceSNMP::DeviceSNMP(ObjectManager& _manager, JSONNode const& _properties)
: DeviceIP(_manager, DeviceSNMP::Type()), module_(""), community_("public"), timeout_(SNMP_TIMEOUT), session_()
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

DeviceSNMP::DeviceSNMP(ObjectManager& _manager, std::string const& _type, std::string const& _module)
: DeviceIP(_manager, _type), module_(_module), community_("public"), timeout_(SNMP_TIMEOUT), session_()
{
	trace.SetClassName(GetClassName());

	SNMP::OID	unknown_oid;
	oid_map_[""] = unknown_oid;

	object_count++;
}

DeviceSNMP::DeviceSNMP(ObjectManager& _manager, std::string const& _type, std::string const& _module, std::string const& _ip)
: DeviceIP(_manager, _type, _ip), module_(_module), community_("public"), timeout_(SNMP_TIMEOUT), session_()
{
	trace.SetClassName(GetClassName());

	SNMP::OID	unknown_oid;
	oid_map_[""] = unknown_oid;

	object_count++;
}

DeviceSNMP::~DeviceSNMP()
{
	Close();

	object_count--;
}

bool		DeviceSNMP::IsIncludedIn(std::string const& _type)
{
	if (_type == DeviceSNMP::Type())
	{
		return	true;	
	}

	return	DeviceIP::IsIncludedIn(_type);
}

bool	DeviceSNMP::Open()
{
	bool	ret_value;
	
	ret_value = session_.Open(ip_, community_);		
	if (ret_value == true)
	{
		session_.SetTimeout(timeout_);	
	}

	return	ret_value;
}

bool	DeviceSNMP::Close()
{
	return	session_.Close();
}

bool		DeviceSNMP::IsOpened()
{
	return	session_.IsOpened();
}

const	std::string&	DeviceSNMP::GetModule()
{
	return	module_;
}

bool	DeviceSNMP::SetModule(std::string const& _module, bool _check)
{
	if (!_check)
	{
		module_ = _module;

		JSONNode	options;

		GetOptions(options);

		JSONNodeUpdate(updated_properties_, TITLE_NAME_OPTIONS, options.write());

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

const	std::string&	DeviceSNMP::GetCommunity()
{
	return	community_;
}

bool	DeviceSNMP::SetCommunity(std::string const& _community, bool _check)
{
	if(!_check)
	{
		community_ = _community;

		JSONNode	options;

		GetOptions(options);

		JSONNodeUpdate(updated_properties_, TITLE_NAME_OPTIONS, options.write());

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

uint32_t	DeviceSNMP::GetTimeout()
{
	return	timeout_;
}

bool	DeviceSNMP::SetTimeout(uint32_t _timeout)
{
	timeout_ = _timeout;
	

	JSONNode	options;

	GetOptions(options);

	JSONNodeUpdate(updated_properties_, TITLE_NAME_OPTIONS, options.write());

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	DeviceSNMP::SetTimeout(std::string const& _timeout, bool _check)
{
	time_t	timeout = strtoul(_timeout.c_str(), 0, 10);

	if (!_check)
	{
		timeout_ = timeout;

		JSONNode	options;

		GetOptions(options);

		JSONNodeUpdate(updated_properties_, TITLE_NAME_OPTIONS, options.write());

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

bool	DeviceSNMP::GetProperty(uint32_t _type, JSONNode & _property)
{
	return	DeviceIP::GetProperty(_type, _property);
}

bool	DeviceSNMP::SetProperty(JSONNode const& _property, bool _check)
{
	return	DeviceIP::SetProperty(_property, _check);
}

bool	DeviceSNMP::GetOptions(JSONNode& _options)
{
	JSONNode	options;

	if (!DeviceIP::GetOptions(options))
	{
		TRACE_ERROR("Failed to get device options");
		return	false;	
	}

	options.push_back(JSONNode(TITLE_NAME_MODULE, module_));
	options.push_back(JSONNode(TITLE_NAME_COMMUNITY, community_));
	options.push_back(JSONNode(TITLE_NAME_TIMEOUT, timeout_));

	_options = options;

	return	true;
}

bool	DeviceSNMP::SetOption(JSONNode& _option, bool _check)
{
	bool	ret_value;

	if (_option.name() == TITLE_NAME_MODULE)
	{
		ret_value = SetModule(_option.as_string(), _check);
	}
	else if (_option.name() == TITLE_NAME_COMMUNITY)
	{
		ret_value = SetCommunity(_option.as_string(), _check);
	}
	else if (_option.name() == TITLE_NAME_TIMEOUT)
	{
		ret_value = SetTimeout(_option.as_string(), _check);
	}
	else
	{
		ret_value = DeviceIP::SetProperty(_option, _check);
	}
	
	return	ret_value;
}

Endpoint*	DeviceSNMP::CreateEndpoint(JSONNode const& _properties)
{
	Endpoint* endpoint = manager_.CreateEndpoint(_properties);
	if (endpoint != NULL)
	{
		Attach(endpoint->GetID());	
	}

	return	endpoint;
}

SNMP::OID DeviceSNMP::GetOID(std::string const& _id)
{
	std::map<std::string, SNMP::OID>::iterator it = oid_map_.find(_id);

	if (it != oid_map_.end())
	{
		return	it->second;
	}

	return	oid_map_[""];
}

SNMP::OID	DeviceSNMP::GetOID(std::string const& _name, uint32_t index)
{
	std::string name;

	if ((module_.length() != 0) && (_name.compare(0, module_.length(), module_) != 0))
	{
		name = module_ + "::" + _name;
	}
	else
	{
		name = _name;
	}

	if (index != 0)
	{
		name += "." + ToString(index);
	}

	SNMP::OID	oid = DeviceSNMP::GetOID(name);
	if (oid.length == 0)
	{
		read_objid(name.c_str(), oid.id, &oid.length);
		if (oid.length != 0)
		{
			read_objid(name.c_str(), oid.id, &oid.length);
		}
		if (index != 0)
		{
			oid.id[oid.length++] = index;	
		}

		oid_map_[name] = oid;
	}

	return	oid;
}

bool	DeviceSNMP::ReadValue(std::string const& _id, time_t& _time, std::string& _value)
{
	bool	ret_value = false;

	if (!DeviceSNMP::IsOpened())
	{
		if (!Open())
		{
			TRACE_ERROR("Session is not opened.");
			return	false;
		}
	}

	SNMP::OID oid = GetOID(_id);
	if (oid.length != 0)
	{
		ret_value = session_.AsyncReadValue(oid, _time, _value);
	}
	else
	{
		TRACE_ERROR("OID not found.");
	}

	return	ret_value;
}

bool	DeviceSNMP::ReadValue(std::string const& _id, time_t& _time, bool& _value)
{
	try
	{
		std::string	value;

		if (!ReadValue(_id, _time, value))
		{
			return	false;	
		}

		_value = IsTrue(value);

		return	true;
	}
	catch(exception& e)
	{
		return	false;
	}

}

bool	DeviceSNMP::ReadValue(SNMP::OID const& _oid, time_t& _time, std::string& _value)
{
	if (!DeviceSNMP::IsOpened())
	{
		if (!Open())
		{
			TRACE_ERROR("Session is not opened.");
			return	false;
		}
	}

	return	session_.AsyncReadValue(_oid, _time, _value);
}

bool	DeviceSNMP::ReadValue(SNMP::OID const& _oid, std::string& _value)
{
	if (!DeviceSNMP::IsOpened())
	{
		if (!Open())
		{
			TRACE_ERROR("Session is not opened.");
			return	false;
		}
	}

	time_t	time;

	return	session_.AsyncReadValue(_oid,  time, _value);	
}

bool	DeviceSNMP::WriteValue(std::string const& _id, std::string const& _value)
{
	bool	ret_value = false;

	if (!DeviceSNMP::IsOpened())
	{
		if (!Open())
		{
			TRACE_ERROR("Session is not opened.");
			return	false;
		}
	}

	SNMP::OID oid = GetOID(_id);
	if (oid.length != 0)
	{
		ret_value = session_.AsyncWriteValue(oid, _value);
	}
	else
	{
		TRACE_ERROR("OID not found.");
	}

	return	ret_value;
}

bool	DeviceSNMP::WriteValue(std::string const& _id, bool _value)
{
	bool	ret_value = false;

	if (!DeviceSNMP::IsOpened())
	{
		if (!Open())
		{
			TRACE_ERROR("Session is not opened.");
			return	false;
		}
	}

	SNMP::OID oid = GetOID(_id);
	if (oid.length != 0)
	{
		if (_value)
		{
			TRACE_ENTRY;
			ret_value = session_.AsyncWriteValue(oid, "1");
		}
		else
		{
			TRACE_ENTRY;
			ret_value = session_.AsyncWriteValue(oid, "0");
		}
	}
	else
	{
		TRACE_ERROR("OID not found.");
	}

	return	ret_value;
}

bool	DeviceSNMP::WriteValue(SNMP::OID const& _oid, std::string const& _value)
{
	if (!DeviceSNMP::IsOpened())
	{
		if (!Open())
		{
			TRACE_ERROR("Session is not opened.");
			return	false;
		}
	}

	return	session_.AsyncWriteValue(_oid,  _value);	
}

bool	DeviceSNMP::WriteValue(SNMP::OID const& _oid, bool _value)
{
	if (!DeviceSNMP::IsOpened())
	{
		if (!Open())
		{
			TRACE_ERROR("Session is not opened.");
			return	false;
		}
	}

	if (_value)
	{
		return	session_.AsyncWriteValue(_oid,  "1");	
	}
	else
	{
		return	session_.AsyncWriteValue(_oid,  "0");	
	}
}

void	DeviceSNMP::Preprocess()
{
	Device::Preprocess();
}

void	DeviceSNMP::Postprocess()
{
	Device::Postprocess();
}

bool	DeviceSNMP::AddMIBPath(std::string const& _path)
{

	add_mibdir(_path.c_str());

	return	true;
}

bool	DeviceSNMP::ReadAllMIBs()
{
	return	true;//snmp_master.ReadAllMIBs();
}

bool	DeviceSNMP::ReadMIB(std::string const& _file_name)
{
	return true;//snmp_master.ReadMIB(_file_name);
}

bool	DeviceSNMP::InsertToDB(Kompex::SQLiteStatement*	_statement)
{
	try
	{
		ostringstream	query;
		uint32_t		index = 0;

		query << "INSERT INTO devices (_id, _type, _time, _enable, _name, _opt0, _opt1, _opt2, _opt3) VALUES (?,?,?,?,?,?,?,?);";
		TRACE_INFO("Query : " << query.str());

		_statement->Sql(query.str());
		_statement->BindString(++index, id_);
		_statement->BindString(++index, std::string(type_));
		_statement->BindInt(++index, 	time_t(date_));
		_statement->BindInt(++index, 	enable_);
		_statement->BindString(++index, name_);
		_statement->BindString(++index, ip_);
		_statement->BindString(++index, module_);
		_statement->BindString(++index, community_);
		_statement->BindString(++index, ToString(timeout_));

		_statement->ExecuteAndFree();

	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR("Failed to insert device to DB.");
		return	false;
	}

	return	true;
}

const char*	DeviceSNMP::Type()
{
	return	OBJECT_TYPE_DEV_SNMP;
}

