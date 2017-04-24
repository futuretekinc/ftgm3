#include <cstdio>
#include <iomanip>
#include "trace.h"
#include <sstream>
#include "data_manager.h"
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"
#include "endpoint.h"

DataManager::DataManager()
: ActiveObject(), temp_db_(true)
{	
	Date	date;

	trace.SetClassName(GetClassName());

	id_ 	= std::string("data_manager");
	enable_ = true;
	date 	= Date::GetCurrentDate();

	file_name_ = "dm-" + date.ToString() + ".db";
}

DataManager::DataManager(std::string const& _db_name)
: ActiveObject(), file_name_(_db_name), temp_db_(false)
{
	trace.SetClassName(GetClassName());

	id_ 	= std::string("data_manager");
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

DataManager::Table*	DataManager::CreateTable(std::string const& _table_name, std::list<std::string>& field_list)
{
	if (!IsTableExist(_table_name))
	{
		std::ostringstream	query;


		query << "CREATE TABLE " << _table_name << " (_id TEXT NOT NULL PRIMARY KEY";

		for(auto field = field_list.begin() ; field != field_list.end() ; field++)
		{
			if ((*field) != "id")
			{
				query << ", _" + (*field) + " TEXT";
			}
		}
		query <<");";
		TRACE_INFO << "Query : " << query.str() << Trace::End;

		try 
		{
			Kompex::SQLiteStatement*	statement = new Kompex::SQLiteStatement(database_);
			if (statement == NULL)
			{
				TRACE_ERROR << "Failed to create SQL statement!" << Trace::End;
				return	NULL;	
			}

			statement->SqlStatement(query.str());

			delete statement;
		}
		catch(Kompex::SQLiteException &exception)
		{
			TRACE_INFO << "Failed to create table[" << _table_name <<"]" << Trace::End;
			return	NULL;
		}
	}

	Table* new_table = new	Table(this, _table_name);
	if (new_table == NULL)
	{
		TRACE_ERROR << "Failed to create table[" << _table_name << "]" << Trace::End;
	}
	else
	{
		TRACE_INFO << "Table[" << _table_name << "] created" << Trace::End;
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
			TRACE_ERROR << "Failed to create SQL statement!" << Trace::End;
			return	false;	
		}

		std::ostringstream	query;

		query << "SELECT COUNT(*) FROM sqlite_master WHERE name=" << "'" << _name << "';";
		TRACE_INFO << "Query : " <<  query.str().c_str() << Trace::End;

		if (statement->SqlAggregateFuncResult(query.str()) != 0)
		{
			exist = true;
		}

		delete statement;
	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_INFO  << "DB not initialized." << Trace::End;
	}

	return	exist;
}

DataManager::Table::Table(DataManager* _parent, std::string const& _name)
: parent_(_parent), name_(_name)
{
	trace.SetClassName(GetClassName());
}

bool	DataManager::Table::Add(Properties const& _properties)
{
	const Property*	property = _properties.Get("id");
	if (property == NULL)
	{
		TRACE_ERROR2 << "Failed to add object to DB." << Trace::End;
		return	false;	
	}

	const Value* value = property->GetValue();	
	if (value == NULL)
	{
		TRACE_ERROR << "Invalid arguments" << Trace::End;
		return	false;	
	}

	std::string id = std::string(*value);

	if (IsExist(std::string(id)))
	{
		TRACE_ERROR << "The object [" << id << "] already exist." << Trace::End;
		return	false;
	}

	try 
	{
		std::ostringstream	query;
		uint32_t		count= 1;
		uint32_t		index = 0;

		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR << "Failed to create SQL statement!" << Trace::End;
			return	false;	
		}

		query << "INSERT INTO " << name_ << "(_id";
		for(auto it = _properties.begin() ; it != _properties.end() ; it++)
		{
			if (it->GetName() != "id")
			{
				query << ", _" << it->GetName();
				count++;
			}
		}

		query << ") VALUES (?";
		for(uint32_t i = 1 ; i < count ; i++)
		{
			query << ",?";
		}
		query << ");";

		TRACE_INFO << "Query : " << query.str() << Trace::End;
		statement->Sql(query.str());
		statement->BindString(++index, id);

		for(auto it = _properties.begin() ; it != _properties.end() ; it++)
		{
			if (it->GetName() != "id")
			{
				const Value*	value = it->GetValue();

				statement->BindString(++index, std::string(*value));
			}
		}

		statement->ExecuteAndFree();

		delete statement;
	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR << "Failed to add object to table[" << name_ << "]" << Trace::End;
		return	false;
	}

	return	true;
}

bool	DataManager::Table::Delete(std::string const& _id)
{
	std::ostringstream	query;

	query << "DELETE FROM " << name_ << " WHERE _id=" << _id << ";";
	TRACE_INFO << "Query : " << query.str() << Trace::End;

	try 
	{
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR << "Failed to create SQL statement!" << Trace::End;
			return	false;	
		}

		statement->Sql(query.str());

		if (statement->GetColumnCount() != 0)
		{
			statement->FreeQuery();
			return	true;
		}

		statement->FreeQuery();

		delete statement;
	}
	catch(Kompex::SQLiteException& exception)
	{
		TRACE_ERROR << "Failed to delete object from table[" << name_ << "]";
		return	false;
	}

	return	true;
}

bool	DataManager::Table::IsExist(std::string const& _id)
{
	bool	exist = false;

	std::ostringstream   query;

	query << "SELECT COUNT(*) FROM " << name_ << " WHERE _id = " << "'" << _id << "';";
	TRACE_INFO << "Query : " << query.str() << Trace::End;

	try  
	{    
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR << "Failed to create SQL statement!" << Trace::End;
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
		TRACE_ERROR << "Failed to check object from table[" << name_ << "]" << Trace::End;
	}    


	return	exist;
}

uint32_t	DataManager::Table::GetCount()
{
	std::ostringstream	query;

	query << "SELECT COUNT(*) FROM " << name_ << ";";
	TRACE_INFO << "Query : " << query.str() << Trace::End;

	uint32_t	count = 0;
	try 
	{
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR << "Failed to create SQL statement!" << Trace::End;
		}
		else
		{
			count = statement->SqlAggregateFuncResult(query.str());
		}

		delete statement;
	}
	catch(Kompex::SQLiteException& exception)
	{
		TRACE_ERROR << "Failed to check object from table[" << name_ << "]";
	}

	return	count;
}

bool	DataManager::Table::GetProperties(std::string const& _id, Properties& _properties)
{
	if (!IsExist(_id))
	{
		TRACE_ERROR << "The object [" << _id << "is not exist!" << Trace::End;
		return	false;
	}

	std::ostringstream	query;

	query << "SELECT * FROM " << name_ << " WHERE _id = " << _id << ";";
	TRACE_INFO << "Query : " << query.str() << Trace::End;

	try 
	{
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR << "Failed to create SQL statement!" << Trace::End;
			return	false;	
		}

		statement->Sql(query.str());
		if (statement->FetchRow())
		{
			TRACE_INFO << "Fetch Row : " << statement->GetColumnCount() << Trace::End;
			for(auto it = _properties.begin(); it != _properties.end() ; it++)
			{
				std::string	value = statement->GetColumnString(it->GetName());

				it->Set(value);
			}

		}
		statement->FreeQuery();

		delete statement;
	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR << "Failed to get object properties" << Trace::End;
	}

	return	true;
}

bool	DataManager::Table::SetProperties(std::string const& _id, Properties& _properties)
{
	if (!IsExist(_id))
	{
		TRACE_ERROR << "The object [" << _id << "is not exist!" << Trace::End;
		return	false;
	}

	std::ostringstream	query;

	try 
	{
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR << "Failed to create SQL statement!" << Trace::End;
			return	false;	
		}

		int	index = 0;
		query << "UPDATE " << name_ << " SET ";
		for(auto it = _properties.begin(); it != _properties.end() ; it++)
		{
			if (index++ != 0)
			{
				query << ", _" + it->GetName() << "=@" << it->GetName();
			}
			else
			{
				query << "_" + it->GetName() << "=@" << it->GetName();
			}
		}
		query << " WHERE _id=@id;";
		TRACE_INFO << "Query : " << query.str() << Trace::End;


		statement->Sql(query.str());

		index = 0;
		for(auto it = _properties.begin(); it != _properties.end() ; it++)
		{
			statement->BindString(++index, std::string(*(it->GetValue())));
			TRACE_INFO << it->GetName() << " : " << std::string(*(it->GetValue())) << std::endl;
		}
		statement->BindString(++index, _id);
		TRACE_INFO << "id : "<<  _id << Trace::End;

		statement->ExecuteAndFree();

		delete statement;
	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR << "Failed to update! - " << exception.GetErrorDescription() << std::endl;
		TRACE_ERROR << "Propertyes : " << _properties << std::endl;
		TRACE_ERROR << "Query : " << query.str() << Trace::End;
		return	false;
	}

	return	true;
}

bool	DataManager::Table::GetProperties(uint32_t _index, uint32_t _count, std::list<Properties>& _properties_list)
{
	std::ostringstream	query;

	query << "SELECT * FROM " << name_ << " ORDER BY _id DESC LIMIT " << _count << " OFFSET " << _index <<";";
	TRACE_INFO << "Query : " << query.str() << Trace::End;

	try 
	{
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			TRACE_ERROR << "Failed to create SQLite statement!" << Trace::End;
			return	false;
		}

		statement->Sql(query.str());
		while(statement->FetchRow())
		{
			Properties	properties;

			TRACE_INFO << "Fetch Row : " << statement->GetColumnCount() << Trace::End;

			for(int i = 0 ; i < statement->GetColumnCount() ; i++)
			{
				std::string name = statement->GetColumnName(i);

				if ((statement->GetColumnType(i) == SQLITE_TEXT) && (statement->GetColumnType(i) == SQLITE3_TEXT))
				{
					std::string	value = statement->GetColumnString(name);

					TRACE_INFO << name.substr(1, name.size()) << " : " << value << Trace::NL;
					properties.Append(name.substr(1, name.size() - 1), value);
				}
			}
			TRACE_INFO << Trace::End;

			if (properties.Count() != 0)
			{
				_properties_list.push_back(properties);	
			}
		}
		statement->FreeQuery();

		delete statement;
	}
	catch(Kompex::SQLiteException &exception)
	{
		TRACE_ERROR << "Failed to get object properties" << Trace::End;
	}

	return	true;
}

DataManager::ValueTable::ValueTable(DataManager* _parent, std::string const& _endpoint_id)
: parent_(_parent)
{
	table_name_ = "table_ep_" + _endpoint_id;
	trace.SetClassName(GetClassName());
}

bool	DataManager::ValueTable::Add(Date const& _date, Value const* _value)
{
	if (parent_ == NULL)
	{
		return	false;
	}

	if (!parent_->IsTableExist(table_name_))
	{
		return	false;	
	}

	std::ostringstream	query;

	query << "INSERT INFO " << table_name_ << "(_time, _value) values(" << _date << ", " << std::string(*_value) << ");";
	
	Kompex::SQLiteStatement*	statement = new Kompex::SQLiteStatement(parent_->database_);
	statement->SqlStatement(query.str());

	delete statement;

	return	true;
}

DataManager::ValueTable*	DataManager::CreateValueTable(std::string const& _endpoint_id)
{
	std::string	table_name_ = "table_ep_" + _endpoint_id;
	ValueTable*	value_table = NULL;

	if (!IsTableExist(table_name_))
	{
		std::ostringstream	query;


		query << "CREATE TABLE " << table_name_ << " (_time TEXT NOT NULL PRIMARY KEY, _value TEXT);";

		TRACE_INFO << "Query : " << query.str() << Trace::End;

		try 
		{
			Kompex::SQLiteStatement*	statement = new Kompex::SQLiteStatement(database_);
			if (statement == NULL)
			{
				TRACE_ERROR << "Failed to create SQL statement!" << Trace::End;
				return	NULL;
			}

			statement->SqlStatement(query.str());

			delete statement;
		}
		catch(Kompex::SQLiteException &exception)
		{
			TRACE_INFO << "Failed to create table[" << table_name_ <<"]" << Trace::End;
			return	NULL;
		}
	}

	try
	{
		value_table = new ValueTable(this, _endpoint_id);
	}
	catch(std::bad_alloc& e)
	{
		TRACE_ERROR << "Failed to create value table!" << Trace::End;
		return	NULL;
	}

	value_table_map_[_endpoint_id] = value_table;
	return	value_table;
}


DataManager::ValueTable*	DataManager::GetValueTable(std::string const& _endpoint_id)
{
	return	value_table_map_[_endpoint_id];
}

bool	DataManager::AddValue(std::string const& _endpoint_id, Date const& _date, Value const* _value)
{
	ValueTable*	_value_table = value_table_map_[_endpoint_id];
	if (_value_table)
	{
		return	false;
	}

	return	_value_table->Add(_date, _value);
}


bool	DataManager::AddDevice(Device *_device)
{
	Properties	properties;

	_device->GetProperties(properties);

	if (device_table_->Add(properties) != true)
	{
		return	false;
	}

	std::list<Endpoint*>	_endpoint_list;

	_device->GetEndpointList(_endpoint_list);
	for(auto it = _endpoint_list.begin(); it != _endpoint_list.end() ; it++)
	{
		AddEndpoint(*it);
	}

	return	true;
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

bool	DataManager::GetDeviceProperties(uint32_t _index, uint32_t _count, std::list<Properties>& _properties_list)
{
	return	device_table_->GetProperties(_index, _count, _properties_list);
}

bool	DataManager::GetDeviceProperties(std::string const& _id, Properties& _properties)
{
	return	device_table_->GetProperties(_id, _properties);
}

bool	DataManager::SetDeviceProperties(std::string const& _id, Properties& _properties)
{
	return	device_table_->SetProperties(_id, _properties);
}

bool	DataManager::AddEndpoint(Endpoint *_endpoint)
{
	Properties	properties;

	_endpoint->GetProperties(properties);

	return	endpoint_table_->Add(properties);
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

bool	DataManager::GetEndpointProperties(uint32_t _index, uint32_t _count, std::list<Properties>& _properties_list)
{
	return	endpoint_table_->GetProperties(_index, _count, _properties_list);
}

bool	DataManager::GetEndpointProperties(std::string const& _id, Properties& _properties)
{
	return	endpoint_table_->GetProperties(_id, _properties);
}

bool	DataManager::SetEndpointProperties(std::string const& _id, Properties& _properties)
{
	return	endpoint_table_->SetProperties(_id, _properties);
}

void	DataManager::Preprocess()
{
	try 
	{
		TRACE_INFO << std::setw(16) << "DB Name : " << file_name_ << Trace::End;
		database_ = new Kompex::SQLiteDatabase(file_name_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
		TRACE_INFO << std::setw(16) << "SQLite Version : " << database_->GetLibVersionNumber() << Trace::End;

		std::list<std::string>	device_field_list;
		Device::GetPropertyFieldList(device_field_list);
		device_table_ = CreateTable("devices", device_field_list);	

		std::list<std::string>	endpoint_field_list;
		Endpoint::GetPropertyFieldList(endpoint_field_list);
		endpoint_table_ = CreateTable("endpoints", endpoint_field_list);	

		
		for(uint32_t i = 0 ; i < GetEndpointCount() ; i++)
		{
			std::list<Properties>	properties_list;

			GetEndpointProperties(i, 1, properties_list);

			for(auto it = properties_list.begin(); it != properties_list.end() ; it++)
			{
				const Property*	id_property = it->Get("id");
				if (id_property != NULL)
				{
					const Value*	value = id_property->GetValue();

					value_table_map_[std::string(*value)]= CreateValueTable(std::string(*value));
				}
			}
		}
		
	}
  	catch(Kompex::SQLiteException &exception)
	{
		std::cerr << "\nException occured" << Trace::End;
		exception.Show();
		std::cerr << "SQLite result code: " << exception.GetSqliteResultCode() << Trace::End;
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
		std::cerr << "\nException occured" << Trace::End;
		exception.Show();
		std::cerr << "SQLite result code: " << exception.GetSqliteResultCode() << Trace::End;
	}
}

Kompex::SQLiteStatement*	DataManager::CreateStatement()
{
	return	new Kompex::SQLiteStatement(database_);
}
