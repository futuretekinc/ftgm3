#include <cstdio>
#include <iomanip>
#include "trace.h"
#include <sstream>
#include "defined.h"
#include "json.h"
#include "exception.h"
#include "data_manager.h"
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"
#include "endpoint.h"
#include "rule.h"


DataManager::Table::Table(DataManager* _manager, std::string const& _name)
: Object(), manager_(_manager)
{
	name_ = _name;
	trace.SetClassName(GetClassName());
	SetParentID(_manager->GetID());
}

bool	DataManager::Table::Add(JSONNode const& _properties)
{
	try
	{
		std::string id = JSONNodeGetID(_properties);
		if (IsExist(std::string(id)))
		{
			TRACE_ERROR("The object [" << id << "] already exist.");
			return	false;
		}

		std::ostringstream	query;
		uint32_t		count= 0;
		uint32_t		index = 0;

		Kompex::SQLiteStatement*	statement = manager_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR("Failed to create SQL statement!");
			return	false;	
		}

		query << "INSERT INTO " << name_ << "(";
		for(JSONNode::const_iterator it = _properties.begin() ; it != _properties.end() ; it++)
		{
			if (count != 0)
			{
				query << ", ";
			}

			query << "_" << it->name();
			count++;
		}

		query << ") VALUES (";
		for(uint32_t i = 0 ; i < count ; i++)
		{
			if (i != 0)
			{
				query << ", ";
			}

			query << "?";
		}
		query << ");";

		TRACE_INFO("Query : " << query.str());
		statement->Sql(query.str());

		for(JSONNode::const_iterator property = _properties.begin() ; property != _properties.end() ; property++)
		{
			if (property->type() == JSON_NODE)
			{
				TRACE_INFO("Bind[" << std::setw(2) << index << "] : " << std::setw(0) << property->write());
				statement->BindString(++index, property->write());
			}
			else
			{
				TRACE_INFO("Bind[" << std::setw(2) << index << "] : " << std::setw(0) << property->as_string());
				statement->BindString(++index, property->as_string());
			}
		}

		statement->ExecuteAndFree();

		delete statement;
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;
	}
	catch(Kompex::SQLiteException &e)
	{
		TRACE_ERROR("Failed to add object to table[" << name_ << "] : " << e.GetString());
		return	false;
	}

	return	true;
}

bool	DataManager::Table::Delete(std::string const& _id)
{
	std::ostringstream	query;
	std::ostringstream	commit;
	query << "DELETE FROM " << name_ << " WHERE _id=" << "'" << _id << "';";
	commit << "COMMIT;";

	TRACE_INFO("Query : " << query.str());
	TRACE_INFO("Commit : " << commit.str());
	try 
	{
		Kompex::SQLiteStatement*	statement = manager_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR("Failed to create SQL statement!");
			return	false;	
		}

		statement->SqlStatement(query.str());
		
		statement->SqlStatement(commit.str());
		if (statement->GetColumnCount() != 0)
		{
			TRACE_INFO("REMOVE SUCCESS");	
			statement->FreeQuery();
			return	true;
		}

		statement->FreeQuery();

		delete statement;
	}
	catch(Kompex::SQLiteException& exception)
	{
		TRACE_ERROR("Failed to delete object from table[" << name_ << "]");
		return	false;
	}

	return	true;
}

bool	DataManager::Table::IsExist(std::string const& _id)
{
	bool	exist = false;

	std::ostringstream   query;

	query << "SELECT COUNT(*) FROM " << name_ << " WHERE _id = " << "'" << _id << "';";
	TRACE_INFO("Query : " << query.str());

	try  
	{    
		Kompex::SQLiteStatement*	statement = manager_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR("Failed to create SQL statement!");
		}
		else
		{
			if (statement->SqlAggregateFuncResult(query.str()) != 0)
			{    
				exist = true;
			}    

			delete statement;
		}
	}    
	catch(Kompex::SQLiteException& exception)
	{    
		TRACE_ERROR("Failed to check object from table[" << name_ << "]");
	}    


	return	exist;
}

uint32_t	DataManager::Table::GetCount()
{
	std::ostringstream	query;

	query << "SELECT COUNT(*) FROM " << name_ << ";";
	TRACE_INFO("Query : " << query.str());

	uint32_t	count = 0;
	try 
	{
		Kompex::SQLiteStatement*	statement = manager_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR("Failed to create SQL statement!");
		}
		else
		{
			count = statement->SqlAggregateFuncResult(query.str());
		}

		delete statement;
	}
	catch(Kompex::SQLiteException& exception)
	{
		TRACE_ERROR("Failed to check object from table[" << name_ << "]");
	}

	return	count;
}

bool	DataManager::Table::GetProperties(std::string const& _id, JSONNode& _properties)
{
	JSONNode	properties;

	if (!IsExist(_id))
	{
		TRACE_ERROR("The object [" << _id << "is not exist!");
		return	false;
	}

	std::ostringstream	query;

	query << "SELECT * FROM " << name_ << " WHERE _id = " << _id << ";";
	TRACE_INFO("Query : " << query.str());

	try 
	{
		Kompex::SQLiteStatement*	statement = manager_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR("Failed to create SQL statement!");
			return	false;	
		}

		statement->Sql(query.str());
		if (statement->FetchRow())
		{
			TRACE_INFO("Fetch Row : " << statement->GetColumnCount());
			for(uint32_t i = 0 ; i  < statement->GetColumnCount() ; i++)
			{
				std::string name = statement->GetColumnName(i);

				if ((statement->GetColumnType(i) == SQLITE_TEXT) || (statement->GetColumnType(i) == SQLITE3_TEXT))
				{
					std::string	value = statement->GetColumnString(name);

					TRACE_INFO(name.substr(1, name.size()) << " : " << value);
					properties.push_back(JSONNode(name.substr(1, name.size() - 1), value));
				}
			}
		}
		statement->FreeQuery();

		delete statement;

		_properties = properties;
	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR("Failed to get object properties");
	}

	return	true;
}

bool	DataManager::Table::SetProperties(std::string const& _id, JSONNode& _properties)
{
	if (!IsExist(_id))
	{
		TRACE_ERROR("The object [" << _id << "is not exist!");
		return	false;
	}

	std::ostringstream	query;

	try 
	{
		Kompex::SQLiteStatement*	statement = manager_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR("Failed to create SQL statement!");
			return	false;	
		}

		int	index = 0;
		query << "UPDATE " << name_ << " SET ";
		for(JSONNode::iterator it = _properties.begin(); it != _properties.end() ; it++)
		{
			if (index++ != 0)
			{
				query << ", _" + it->name() << "=@" << it->name();
			}
			else
			{
				query << "_" + it->name() << "=@" << it->name();
			}
		}
		query << " WHERE _id=@id;";
		TRACE_INFO("Query : " << query.str());


		statement->Sql(query.str());

		index = 0;
		for(JSONNode::iterator it = _properties.begin(); it != _properties.end() ; it++)
		{
			statement->BindString(++index, it->as_string());
			TRACE_INFO("Bind[" << std::setw(2) << index << "] : " << std::setw(0) << it->as_string());
		}
		statement->BindString(++index, _id);
		TRACE_INFO("Bind[" << std::setw(2) << index << "] : " << std::setw(0) <<  _id);

		statement->ExecuteAndFree();

		delete statement;
	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR("Failed to update! - " << exception.GetErrorDescription());
		TRACE_ERROR("Propertyes : " << _properties.write_formatted());
		TRACE_ERROR("Query : " << query.str());
		return	false;
	}

	return	true;
}

bool	DataManager::Table::GetProperties(uint32_t _index, uint32_t _count, JSONNode& _array)
{
	std::ostringstream	query;
	JSONNode			array(JSON_ARRAY);

	query << "SELECT * FROM " << name_ << " ORDER BY _id DESC LIMIT " << _count << " OFFSET " << _index <<";";
	TRACE_INFO("Query : " << query.str());

	try 
	{
		Kompex::SQLiteStatement*	statement = manager_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR("Failed to create SQLite statement!");
			return	false;
		}

		statement->Sql(query.str());
		while(statement->FetchRow())
		{
			JSONNode	properties(JSON_NODE);

			TRACE_INFO("Fetch Row : " << statement->GetColumnCount());

			for(int i = 0 ; i < statement->GetColumnCount() ; i++)
			{
				std::string name = statement->GetColumnName(i);

				if ((statement->GetColumnType(i) == SQLITE_TEXT) || (statement->GetColumnType(i) == SQLITE3_TEXT))
				{
					std::string	value = statement->GetColumnString(name);

					TRACE_INFO(name.substr(1, name.size()) << " : " << value);

					properties.push_back(JSONNode(name.substr(1, name.size() - 1), value));
				}
			}

			if (properties.size() != 0)
			{
				array.push_back(properties);	
			}
		}
		statement->FreeQuery();

		delete statement;
	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR("Failed to get object properties");
	}

	_array = array;

	return	true;
}

DataManager::ValueTable::ValueTable(DataManager* _manager, std::string const& _name)
: Object(), manager_(_manager)
{
	name_ = _name;
	trace.SetClassName(GetClassName());
	SetParentID(_manager->GetID());
}

bool	DataManager::ValueTable::Add(time_t _time, std::string const& _value)
{
	if (manager_ == NULL)
	{
		TRACE_ERROR("Value table manager is not assigned!");
		return	false;
	}

	if (!manager_->IsTableExist(name_))
	{
		TRACE_ERROR("Value table[" << name_ << "] not exist!");
		return	false;	
	}

	std::ostringstream	query;

	query << "INSERT INTO " << name_ << "(_time, _value) values(\"" << _time << "\", \"" << _value << "\");";

	TRACE_INFO("Query : " << query.str());

	Kompex::SQLiteStatement*	statement = new Kompex::SQLiteStatement(manager_->database_);
	try
	{
		statement->SqlStatement(query.str());
		TRACE_INFO("Done");
	}
	catch(Kompex::SQLiteException& e)
	{
		TRACE_ERROR("SQLite error! - " << e.GetString());
	}

	delete statement;


	return	true;
}

DataManager::DataManager()
: ProcessObject(), temp_db_(true)
{	
	Date	date;

	trace.SetClassName(GetClassName());

	name_ 	= std::string("data_manager");
	enable_ = true;
	date 	= Date::GetCurrent();

	file_name_ = "dm-" + date.ToString() + ".db";
}

DataManager::DataManager(std::string const& _db_name)
: ProcessObject(), file_name_(_db_name), temp_db_(false)
{
	trace.SetClassName(GetClassName());

	name_ 	= std::string("data_manager");
	enable_ = true;
}

DataManager::~DataManager()
{
	if (!stop_)
	{
		Stop();		
	}

	if (temp_db_)
	{
		std::remove(file_name_.c_str());
	}
}

const std::string&	DataManager::GetDBFile()
{
	return	file_name_;
}

bool	DataManager::SetDBFile(std::string const& _file_name, bool _check)
{
	if (!_check)
	{
		file_name_ = _file_name;
	}

	return	true;
}

bool	DataManager::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;

	if (_property.name() == TITLE_NAME_DATA_FILE)
	{
		ret_value = SetDBFile(_property.as_string());
	}
	else
	{
		ret_value = ProcessObject::SetProperty(_property, _check);
	}

	return	ret_value;
}

DataManager::operator JSONNode() const
{
	JSONNode	root;

	root.push_back(JSONNode(TITLE_NAME_DATA_FILE, file_name_));

	JSONNode	trace_config = trace;
	trace_config.set_name(TITLE_NAME_TRACE);
	root.push_back(trace_config);

	return	root;
}

DataManager::Table*	DataManager::CreateTable(std::string const& _table_name, std::list<std::string>& field_list)
{
	if (!IsTableExist(_table_name))
	{
		std::ostringstream	query;


		query << "CREATE TABLE " << _table_name << " (_id TEXT NOT NULL PRIMARY KEY";

		for(std::list<std::string>::iterator field = field_list.begin() ; field != field_list.end() ; field++)
		{
			if ((*field) != "id")
			{
				query << ", _" + (*field) + " TEXT";
			}
		}
		query <<");";
		TRACE_INFO("Query : " << query.str());

		try 
		{
			Kompex::SQLiteStatement*	statement = new Kompex::SQLiteStatement(database_);
			if (statement == NULL)
			{
				TRACE_ERROR("Failed to create SQL statement!");
				return	NULL;	
			}

			statement->SqlStatement(query.str());

			delete statement;
		}
		catch(Kompex::SQLiteException &exception)
		{
			TRACE_INFO("Failed to create table[" << _table_name <<"]");
			return	NULL;
		}
	}

	Table* new_table = new	Table(this, _table_name);
	if (new_table == NULL)
	{
		TRACE_ERROR("Failed to create table[" << _table_name << "]");
	}
	else
	{
		TRACE_INFO("Table[" << _table_name << "] created");
	}

	return	new_table;
}

bool	DataManager::IsTableExist
(
	std::string const& _name
)
{
	bool	exist = false;

	try
	{
		Kompex::SQLiteStatement*	statement = new Kompex::SQLiteStatement(database_);
		if (statement == NULL)
		{
			TRACE_ERROR("Failed to create SQL statement!");
			return	false;	
		}

		std::ostringstream	query;

		query << "SELECT COUNT(*) FROM sqlite_master WHERE name=" << "'" << _name << "';";
		TRACE_INFO("Query : " <<  query.str().c_str());

		if (statement->SqlAggregateFuncResult(query.str()) != 0)
		{
			exist = true;
		}

		delete statement;
	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_INFO("DB not initialized.");
	}

	return	exist;
}

DataManager::ValueTable*	DataManager::CreateValueTable(std::string const& _endpoint_id)
{
	std::string	name = "table_ep_" + _endpoint_id;
	ValueTable*	value_table = NULL;

	if (!IsTableExist(name))
	{
		std::ostringstream	query;


		query << "CREATE TABLE " << name << " (_time INT NOT NULL PRIMARY KEY, _value TEXT);";

		TRACE_INFO("Query : " << query.str());

		try 
		{
			Kompex::SQLiteStatement*	statement = new Kompex::SQLiteStatement(database_);
			if (statement == NULL)
			{
				TRACE_ERROR("Failed to create SQL statement!");
				return	NULL;
			}

			statement->SqlStatement(query.str());

			delete statement;
		}
		catch(Kompex::SQLiteException &exception)
		{
			TRACE_INFO("Failed to create table[" << name <<"]");
			return	NULL;
		}
	}

	try
	{
		value_table = new ValueTable(this, name);
	}
	catch(std::bad_alloc& e)
	{
		TRACE_ERROR("Failed to create value table!");
		return	NULL;
	}

	return	value_table;
}


DataManager::ValueTable*	DataManager::GetValueTable(std::string const& _endpoint_id)
{
	return	value_table_map_[_endpoint_id];
}

bool	DataManager::AddValue(std::string const& _endpoint_id, time_t _time, std::string const& _value)
{
	ValueTable*	_value_table = value_table_map_[_endpoint_id];
	if (!_value_table)
	{
		TRACE_ERROR("Endpoint[" << _endpoint_id << "] value table not exist!");
		return	false;
	}

	return	_value_table->Add(_time, _value);
}

bool	DataManager::AddGateway(Gateway *_gateway)
{
	JSONNode	properties;

	_gateway->GetProperties(properties);

	return	gateway_table_->Add(properties);
}

bool	DataManager::DeleteGateway(std::string const& _id)
{
	return	gateway_table_->Delete(_id);
}

bool	DataManager::IsGatewayExist(std::string const& _id)
{
	return	gateway_table_->IsExist(_id);
}

uint32_t	DataManager::GetGatewayCount()
{
	return	gateway_table_->GetCount();
}

bool	DataManager::GetGatewayProperties(uint32_t _index, uint32_t _count, JSONNode& _array)
{
	return	gateway_table_->GetProperties(_index, _count, _array);
}

bool	DataManager::GetGatewayProperties(std::string const& _id, JSONNode& _properties)
{
	return	gateway_table_->GetProperties(_id, _properties);
}

bool	DataManager::SetGatewayProperties(std::string const& _id, JSONNode& _properties)
{
	return	gateway_table_->SetProperties(_id, _properties);
}

bool	DataManager::AddDevice(Device *_device)
{
	JSONNode	properties;

	_device->GetProperties(properties);

	return	device_table_->Add(properties);
}

bool	DataManager::DeleteDevice(std::string const& _id)
{
	return	device_table_->Delete(_id);
}

bool	DataManager::IsDeviceExist(std::string const& _id)
{
	return	device_table_->IsExist(_id);
}

uint32_t	DataManager::GetDeviceCount()
{
	return	device_table_->GetCount();
}

bool	DataManager::GetDeviceProperties(uint32_t _index, uint32_t _count, JSONNode& _array)
{
	return	device_table_->GetProperties(_index, _count, _array);
}

bool	DataManager::GetDeviceProperties(std::string const& _id, JSONNode& _properties)
{
	return	device_table_->GetProperties(_id, _properties);
}

bool	DataManager::SetDeviceProperties(std::string const& _id, JSONNode& _properties)
{
	return	device_table_->SetProperties(_id, _properties);
}

///////////////////////////////////////////////////////////////////////////
// Endpoint
///////////////////////////////////////////////////////////////////////////
bool	DataManager::AddEndpoint(Endpoint *_endpoint)
{
	JSONNode	properties;

	_endpoint->GetProperties(properties);

	if (!endpoint_table_->Add(properties))
	{
		TRACE_ERROR("Failed to add properties to table");
		return	false;
	}

	try
	{
		std::string	id = JSONNodeGetID(properties);

		value_table_map_[id]= CreateValueTable(id);
	}
	catch(ObjectNotFound& e)
	{
	}

	return	true;
}

bool	DataManager::DeleteEndpoint(std::string const& _id)
{
	return	endpoint_table_->Delete(_id);
}

bool	DataManager::IsEndpointExist(std::string const& _id)
{
	return	endpoint_table_->IsExist(_id);
}

uint32_t	DataManager::GetEndpointCount()
{
	return	endpoint_table_->GetCount();
}

bool	DataManager::GetEndpointProperties(uint32_t _index, uint32_t _count, JSONNode& _array)
{
	return	endpoint_table_->GetProperties(_index, _count, _array);
}

bool	DataManager::GetEndpointProperties(std::string const& _id, JSONNode& _properties)
{
	return	endpoint_table_->GetProperties(_id, _properties);
}

bool	DataManager::SetEndpointProperties(std::string const& _id, JSONNode& _properties)
{
	return	endpoint_table_->SetProperties(_id, _properties);
}

///////////////////////////////////////////////////////////////////////////
// Rule
///////////////////////////////////////////////////////////////////////////
bool	DataManager::AddRule(Rule *_rule)
{
	JSONNode	properties;

	_rule->GetProperties(properties);

	if (!rule_table_->Add(properties))
	{
		TRACE_ERROR("Failed to add properties to table");
		return	false;
	}

	return	true;
}

bool	DataManager::DeleteRule(std::string const& _id)
{
	return	rule_table_->Delete(_id);
}

bool	DataManager::IsRuleExist(std::string const& _id)
{
	return	rule_table_->IsExist(_id);
}

uint32_t	DataManager::GetRuleCount()
{
	return	rule_table_->GetCount();
}

bool	DataManager::GetRuleProperties(uint32_t _index, uint32_t _count, JSONNode& _array)
{
	return	rule_table_->GetProperties(_index, _count, _array);
}

bool	DataManager::GetRuleProperties(std::string const& _id, JSONNode& _properties)
{
	return	rule_table_->GetProperties(_id, _properties);
}

bool	DataManager::SetRuleProperties(std::string const& _id, JSONNode& _properties)
{
	return	rule_table_->SetProperties(_id, _properties);
}

//////////////////////////////////////////////////////////////////
// Internal
//////////////////////////////////////////////////////////////////
void	DataManager::Preprocess()
{
	try 
	{
		TRACE_INFO(std::setw(16) << "DB Name : " << file_name_);
		database_ = new Kompex::SQLiteDatabase(file_name_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
		TRACE_INFO(std::setw(16) << "SQLite Version : " << database_->GetLibVersionNumber());

		std::list<std::string>	gateway_field_list;
		Gateway::GetPropertyFieldList(gateway_field_list);
		gateway_table_ = CreateTable(DEFAULT_CONST_DB_TABLE_NAME_GATEWAY, gateway_field_list);	

		std::list<std::string>	device_field_list;
		Device::GetPropertyFieldList(device_field_list);
		device_table_ = CreateTable(DEFAULT_CONST_DB_TABLE_NAME_DEVICE, device_field_list);	

		std::list<std::string>	endpoint_field_list;
		Endpoint::GetPropertyFieldList(endpoint_field_list);
		endpoint_table_ = CreateTable(DEFAULT_CONST_DB_TABLE_NAME_ENDPOINT, endpoint_field_list);	

		std::list<std::string>	rule_field_list;
		Rule::GetPropertyFieldList(rule_field_list);
		rule_table_ = CreateTable(DEFAULT_CONST_DB_TABLE_NAME_RULE, rule_field_list);	

		
		for(uint32_t i = 0 ; i < GetEndpointCount() ; i++)
		{
			JSONNode	properties_list;

			GetEndpointProperties(i, 1, properties_list);

			for(JSONNode::iterator it = properties_list.begin(); it != properties_list.end() ; it++)
			{
				try
				{
					std::string id = JSONNodeGetID(*it);

					value_table_map_[id]= CreateValueTable(id);
				}
				catch(ObjectNotFound& e)
				{
				}
			}
		}
		
	}
  	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR("Exception occured");
		exception.Show();
		TRACE_ERROR("SQLite result code: " << exception.GetSqliteResultCode());
	}
}

void	DataManager::Process()
{
}

void	DataManager::Postprocess()
{
	try 
	{
		if (database_ != NULL)
		{
			delete database_;
			database_ = NULL;
		}
	}
  	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR("Exception occured");
		exception.Show();
		TRACE_ERROR("SQLite result code: " << exception.GetSqliteResultCode());
	}
}

Kompex::SQLiteStatement*	DataManager::CreateStatement()
{
	return	new Kompex::SQLiteStatement(database_);
}
