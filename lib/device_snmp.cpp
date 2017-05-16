#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "defined.h"
#include "trace.h"
#include "object_manager.h"
#include "device_snmp.h"
#include "endpoint.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"

using namespace std;

static bool	snmp_initialized = false;
static bool	mib_initialized = false;
static uint32_t object_count = 0;

DeviceSNMP::OID::OID()
{
	memset(id, 0, sizeof(id));
	length = 0;
}

DeviceSNMP::OID::operator string() const
{
	ostringstream	oss;

	for(int i = 0 ; i < length ; i++)
	{
		oss << "." << id[i];
	}

	return	oss.str();
}

DeviceSNMP::DeviceSNMP(ObjectManager& _manager)
: DeviceIP(_manager, SNMP), module_(""), community_("public"), timeout_(5 * TIME_SECOND), session_(NULL)
{
	trace.SetClassName(GetClassName());
}

DeviceSNMP::DeviceSNMP(ObjectManager& _manager, Properties const& _properties)
: DeviceIP(_manager, SNMP), module_(""), community_("public"), timeout_(5 * TIME_SECOND), session_(NULL)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, true);
}

DeviceSNMP::DeviceSNMP(ObjectManager& _manager, std::string const& _module)
: DeviceIP(_manager, SNMP), module_(_module), community_("public"), timeout_(5 * TIME_SECOND), session_(NULL)
{
	trace.SetClassName(GetClassName());

	if (!snmp_initialized)
	{
		init_snmp("ftgm");

		OID	unknown_oid;
		oid_map_[""] = unknown_oid;

		snmp_initialized = true;
	}

	object_count++;
}

DeviceSNMP::DeviceSNMP(ObjectManager& _manager, std::string const& _module, ValueIP const& _ip)
: DeviceIP(_manager, SNMP, _ip), module_(_module), community_("public"), timeout_(5 * TIME_SECOND), session_(NULL)
{
	trace.SetClassName(GetClassName());

	if (!snmp_initialized)
	{
		init_snmp("ftgm");

		OID	unknown_oid;
		oid_map_[""] = unknown_oid;

		snmp_initialized = true;
	}

	object_count++;
}

DeviceSNMP::~DeviceSNMP()
{
	Close();

	object_count--;

	if (object_count == 0)
	{
		if (mib_initialized)
		{
			shutdown_mib();
			mib_initialized = false;
		}
	}
}

bool	DeviceSNMP::IsIncludedIn(Type _type)
{
	return	(_type == SNMP);
}


bool	DeviceSNMP::Open()
{
	if (session_ == NULL)
	{
		snmp_session	session;
		char			ip[IP_LENGTH_MAX + 1];
		char			community[SNMP_COMMUNITY_LENGTH_MAX + 1];

		snmp_sess_init(&session);

		if (module_.size() != 0)
		{
			read_mib(module_.c_str());
	//		read_all_mibs();
		}

		session.version = SNMP_VERSION_2c;

		session.peername = ip;
		strncpy(session.peername, std::string(ip_).c_str(), IP_LENGTH_MAX);

		session.community = (uint8_t *)community;
		strncpy((char *)session.community, community_.c_str(), SNMP_COMMUNITY_LENGTH_MAX);
		session.community_len = community_.size();

		session_ = snmp_open(&session);
		if (session_ == NULL)
		{
			TRACE_ERROR("Failed to open session.");
		}
		else
		{
			TRACE_INFO("SNMP session[" << ip << ":" << community_<<  "] opened.");	
		}

	}

	return	(session_ != NULL);
}

bool	DeviceSNMP::Close()
{
	if (session_ != NULL)
	{
		snmp_close(session_);	
	
		session_ = NULL;
	}

	return	true;	
}

bool		DeviceSNMP::IsOpened()
{
	return	session_ != NULL;
}

const	std::string&	DeviceSNMP::GetModule()
{
	return	module_;
}

bool	DeviceSNMP::SetModule(std::string const& _module)
{
	module_ = _module;

	return	true;
}

const	std::string&	DeviceSNMP::GetCommunity()
{
	return	community_;
}

bool	DeviceSNMP::SetCommunity(std::string const& _community)
{
	community_ = _community;

	return	true;
}

bool	DeviceSNMP::GetProperties(Properties& _properties) const
{
	if (DeviceIP::GetProperties(_properties))
	{
		_properties.Append("module", module_);
		_properties.Append("community", community_);
		_properties.Append("timeout", timeout_);

		return	true;	
	}

	return	false;
}

bool	DeviceSNMP::SetPropertyInternal(Property const& _property, bool create)
{
	if (_property.GetName() == "module")
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			module_ = value->Get();
			TRACE_INFO("The module set to " << module_);
			return	true;
		}
	}
	else if (_property.GetName() == "community")
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			community_ = value->Get();
			TRACE_INFO("The community set to " << community_);
			return	true;
		}
	}
	else
	{
		return	DeviceIP::SetPropertyInternal(_property, create);
	}

	return	false;
}

Endpoint*	DeviceSNMP::CreateEndpoint(Properties const& _properties)
{
	Endpoint* endpoint = manager_.CreateEndpoint(_properties);
	if (endpoint != NULL)
	{
		Attach(endpoint->GetID());	
	}

	return	endpoint;
}

DeviceSNMP::OID DeviceSNMP::GetOID(std::string const& _id)
{
	std::map<std::string, OID>::iterator it = oid_map_.find(_id);

	if (it != oid_map_.end())
	{
		return	it->second;
	}

	return	oid_map_[""];
}

DeviceSNMP::OID	DeviceSNMP::GetOID(std::string const& _name, uint32_t index)
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
		name += "." + to_string(index);
	}

	OID	oid = DeviceSNMP::GetOID(name);
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

bool	DeviceSNMP::ReadValue(string const& _id, Value *_value)
{
	Open();

	if (session_ == NULL)
	{
		TRACE_ERROR("Session is not opened.");
		return	false;
	}

	OID oid = GetOID(_id);
	if (oid.length == 0)
	{
		TRACE_ERROR("OID not found.");
		Close();
		return	false;
	}

	netsnmp_pdu*	response_pdu = NULL;
	netsnmp_pdu*	request_pdu = snmp_pdu_create(SNMP_MSG_GET);
	if (request_pdu == NULL)
	{
		Close();
		return	false;
	}

	bool	ret = false;
	request_pdu->time = 5;
	snmp_add_null_var(request_pdu, oid.id, oid.length);   

	if (snmp_synch_response(session_, request_pdu, &response_pdu) == STAT_SUCCESS)
	{ 
		if (response_pdu != NULL)
		{
			if (response_pdu->errstat == SNMP_ERR_NOERROR)
			{
				Convert(response_pdu->variables, _value);
				ret = true;
			}
		}
	}
	else
	{
		TRACE_ERROR("Failed to get SNMP!");
	}

	if (response_pdu != NULL)
	{
		snmp_free_pdu(response_pdu);	
	}

	Close();

	return	ret;
}

bool	DeviceSNMP::ReadValue(OID const& _oid, Value *_value)
{
	Open();

	if (session_ == NULL)
	{
		TRACE_ERROR("Session is not opened.");
		return	false;
	}

	netsnmp_pdu*	response_pdu = NULL;
	netsnmp_pdu*	request_pdu = snmp_pdu_create(SNMP_MSG_GET);
	if (request_pdu == NULL)
	{ 
		Close();
		return	false;
	}

	bool	ret = false;
	request_pdu->time = 5;
	snmp_add_null_var(request_pdu, _oid.id, _oid.length);   

	int snmp_ret = snmp_synch_response(session_, request_pdu, &response_pdu);
	if ((snmp_ret != STAT_SUCCESS) || (response_pdu->errstat != SNMP_ERR_NOERROR))
	{
		TRACE_ERROR("Failed to get SNMP[" << std::string(_oid) << "]!");
		if (response_pdu != NULL)
		{	
			TRACE_ERROR("Error : " << snmp_errstring(response_pdu->errstat) << "]!");
		}
	}
	else
	{ 
		if (response_pdu != NULL)
		{
			if (response_pdu->errstat == SNMP_ERR_NOERROR)
			{
				Convert(response_pdu->variables, _value);
				ret = true;
			}
		}
	}

	if (response_pdu != NULL)
	{
		snmp_free_pdu(response_pdu);	
	}

	Close();

	return	ret;
}

bool	DeviceSNMP::Convert
(
	struct variable_list *_variable,
	Value* _value
)
{
	if (_variable == NULL) 
	{   
		return	false;
	}

	switch(_variable->type)
	{   
	case    ASN_INTEGER:
		{   
			int	value = 0;
			switch (_variable->val_len)
			{   
				case    1:  value = (*(int8_t *)_variable->val.integer); break;
				case    2:  value = (*(int16_t *)_variable->val.integer); break;
				case    4:  value = (*(int32_t *)_variable->val.integer); break;
			}   

			if (dynamic_cast<ValueInt*>(_value) != 0)
			{
				dynamic_cast<ValueInt*>(_value)->Set(value);
			}
			else if (dynamic_cast<ValueBool*>(_value) != 0)
			{
				dynamic_cast<ValueBool*>(_value)->Set(value);
			}
			else if (dynamic_cast<ValueFloat*>(_value) != 0)
			{
				dynamic_cast<ValueFloat*>(_value)->Set(value);
			}
			else
			{
				return	false;	
			}
		}   
		break;

	case    ASN_OCTET_STR:
		{   
			if (_variable->val_len != 0)
			{   
				char* buffer = new char [_variable->val_len + 1]; 

				memcpy(buffer, _variable->val.string, _variable->val_len);
				buffer[_variable->val_len] = 0;

				_value->Set(buffer);	
			}   
		}   
		break;
	
	default:
		return	false;
	}   

	return  true;
}

void	DeviceSNMP::Preprocess()
{
	//Open();
}

void	DeviceSNMP::Postprocess()
{
	//Close();
}

bool	DeviceSNMP::AddMIBPath(std::string const& _path)
{
	if (!mib_initialized)
	{
		init_mib();
		mib_initialized = true;
	}

	add_mibdir(_path.c_str());

	return	true;
}

bool	DeviceSNMP::ReadAllMIBs()
{
	if (!mib_initialized)
	{
		init_mib();
		mib_initialized = true;
	}

	return	(read_all_mibs() != 0);
}

bool	DeviceSNMP::ReadMIB(std::string const& _file_name)
{
	if (!mib_initialized)
	{
		init_mib();
		mib_initialized = true;
	}

	return (read_mib(_file_name.c_str()) != 0);
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
		_statement->BindInt(++index, 	GetType());
		_statement->BindInt(++index, 	time_t(date_));
		_statement->BindInt(++index, 	enable_);
		_statement->BindString(++index, name_);
		_statement->BindString(++index, ip_);
		_statement->BindString(++index, module_);
		_statement->BindString(++index, community_);
		_statement->BindString(++index, to_string(timeout_));

		_statement->ExecuteAndFree();

	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR("Failed to insert device to DB.");
		return	false;
	}

	return	true;
}
