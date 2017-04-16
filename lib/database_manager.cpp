#include <cstdio>
#include <iomanip>
#include "trace.h"
#include <sstream>
#include "database_manager.h"
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"
#include "endpoint.h"

DatabaseManager::DatabaseManager()
: ActiveObject(), temp_db_(true)
{	
	char	buffer[L_tmpnam];
	tmpnam(buffer);

	file_name_ = std::string(buffer);
}

DatabaseManager::DatabaseManager(std::string const& _db_name)
: ActiveObject(), file_name_(_db_name), temp_db_(false)
{
}

DatabaseManager::~DatabaseManager()
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

DatabaseManager::Table*	DatabaseManager::CreateTable(std::string const& _table_name)
{
	if (!IsTableExist(_table_name))
	{
		std::ostringstream	query;
		std::list<std::string>	field_list;

		Object::GetPropertyFieldList(field_list);

		query << "CREATE TABLE " << _table_name << " (_id TEXT NOT NULL PRIMARY KEY";

		for(auto field = field_list.begin() ; field != field_list.end() ; field++)
		{
			if ((*field) != "id")
			{
				query << ", _" + (*field) + " TEXT";
			}
		}
		query <<");";
		trace_info << "Query : " << query.str() << std::endl;

		try 
		{
			Kompex::SQLiteStatement*	statement = new Kompex::SQLiteStatement(database_);
			if (statement == NULL)
			{
				trace_error << "Failed to create SQL statement!" << std::endl;
				return	NULL;	
			}

			statement->SqlStatement(query.str());

			delete statement;
		}
		catch(Kompex::SQLiteException &exception)
		{
			trace_info << "Failed to create table[" << _table_name <<"]" << std::endl;
			return	NULL;
		}
	}

	Table* new_table = new	Table(this, _table_name);
	if (new_table == NULL)
	{
		trace_error << "Failed to create table[" << _table_name << "]" << std::endl;
	}
	else
	{
		trace_info << "Table[" << _table_name << "] created" << std::endl;
	}

	return	new_table;
}

bool	DatabaseManager::IsTableExist
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
			trace_error << "Failed to create SQL statement!" << std::endl;
			return	false;	
		}

		std::ostringstream	query;

		query << "SELECT COUNT(*) FROM sqlite_master WHERE name=" << "'" << _name << "';";
		trace_info << "Query : " <<  query.str().c_str() << std::endl;

		if (statement->SqlAggregateFuncResult(query.str()) != 0)
		{
			exist = true;
		}

		delete statement;
	}
	catch(Kompex::SQLiteException &exception)
	{
		trace_info  << "DB not initialized." << std::endl;
	}

	return	exist;
}

DatabaseManager::Table::Table(DatabaseManager* _parent, std::string const& _name)
: parent_(_parent), name_(_name)
{
}

bool	DatabaseManager::Table::Add(Properties const& _properties)
{
	const Property*	property = _properties.Get("id");
	if (property == NULL)
	{
		trace_error << "Failed to add object to DB." << std::endl;
		return	false;	
	}

	const Value* value = property->GetValue();	
	if (value == NULL)
	{
		trace_error << "Invalid arguments" << std::endl;
		return	false;	
	}

	std::string id = std::string(*value);

	if (IsExist(std::string(id)))
	{
		trace_error << "The object [" << id << "] already exist." << std::endl;
		return	false;
	}
	else
	{
		trace_info << "The object [" << id << "] not exist!" << std::endl;
	}

	try 
	{
		std::ostringstream	query;
		uint32_t		count= 1;
		uint32_t		index = 0;

		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			trace_error << "Failed to create SQL statement!" << std::endl;
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

		trace_info << "Query : " << query.str() << std::endl;
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
		trace_error << "Failed to add object to table[" << name_ << "]" << std::endl;
		return	false;
	}

	return	true;
}

bool	DatabaseManager::Table::Delete(std::string const& _id)
{
	std::ostringstream	query;

	query << "DELETE FROM " << name_ << " WHERE _id=" << _id << ";";
	trace_info << "Query : " << query.str() << std::endl;

	try 
	{
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			trace_error << "Failed to create SQL statement!" << std::endl;
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
		trace_error << "Failed to delete object from table[" << name_ << "]";
		return	false;
	}

	return	true;
}

bool	DatabaseManager::Table::IsExist(std::string const& _id)
{
	bool	exist = false;

	std::ostringstream   query;

	query << "SELECT COUNT(*) FROM " << name_ << " WHERE _id = " << "'" << _id << "';";
	trace_info << "Query : " << query.str() << std::endl;

	try  
	{    
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			trace_error << "Failed to create SQL statement!" << std::endl;
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
		trace_error << "Failed to check object from table[" << name_ << "]" << std::endl;
	}    


	return	exist;
}

uint32_t	DatabaseManager::Table::GetCount()
{
	std::ostringstream	query;

	query << "SELECT COUNT(*) FROM " << name_ << ";";
	trace_info << "Query : " << query.str() << std::endl;

	uint32_t	count = 0;
	try 
	{
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			trace_error << "Failed to create SQL statement!" << std::endl;
		}
		else
		{
			count = statement->SqlAggregateFuncResult(query.str());
		}

		delete statement;
	}
	catch(Kompex::SQLiteException& exception)
	{
		trace_error << "Failed to check object from table[" << name_ << "]";
	}

	return	count;
}

bool	DatabaseManager::Table::GetProperties(std::string const& _id, Properties& _properties)
{
	if (!IsExist(_id))
	{
		trace_error << "The object [" << _id << "is not exist!" << std::endl;
		return	false;
	}

	std::ostringstream	query;

	query << "SELECT * FROM " << name_ << " WHERE _id = " << _id << ";";
	trace_info << "Query : " << query.str() << std::endl;

	try 
	{
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			trace_error << "Failed to create SQL statement!" << std::endl;
			return	false;	
		}

		statement->Sql(query.str());
		if (statement->FetchRow())
		{
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
		trace_error << "Failed to get object properties" << std::endl;
	}

	return	true;
}

bool	DatabaseManager::Table::SetProperties(std::string const& _id, Properties& _properties)
{
	if (!IsExist(_id))
	{
		trace_error << "The object [" << _id << "is not exist!" << std::endl;
		return	false;
	}

	try 
	{
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			trace_error << "Failed to create SQL statement!" << std::endl;
			return	false;	
		}

		int	index = 0;
		std::ostringstream	query;

		query << "UPDATE " << name_ << " SET ";
		for(auto it = _properties.begin(); it != _properties.end() ; it++)
		{
			if (index != 0)
			{
				query << ", -" + it->GetName() << "=@" << it->GetName();
			}
			else
			{
				query << "-" + it->GetName() << "=@" << it->GetName();
			}
			statement->BindString(++index, std::string(*(it->GetValue())));
		}
		query << "WHERE _id=@id";
		trace_info << "Query : " << query.str() << std::endl;


		statement->Sql(query.str());

		statement->ExecuteAndFree();

		delete statement;
	}
	catch(Kompex::SQLiteException &exception)
	{
		trace_error << "Failed to add object to DB!" << std::endl;
		return	false;
	}

	return	true;
}

bool	DatabaseManager::Table::GetProperties(uint32_t _index, uint32_t _count, std::list<Properties> _properties_list)
{
	std::ostringstream	query;

	query << "SELECT * FROM " << name_ << " ORDER BY _id DESC LIMIT " << _count << " OFFSET " << _index <<";";
	trace_info << "Query : " << std::endl;

	try 
	{
		Kompex::SQLiteStatement*	statement = parent_->CreateStatement();
		if (statement == NULL)
		{
			trace_error << "Failed to create SQLite statement!" << std::endl;
			return	false;
		}

		statement->Sql(query.str());
		while(statement->FetchRow())
		{
			Properties	properties;

			for(int i = 0 ; i < statement->GetColumnCount() ; i++)
			{
				if ((statement->GetColumnType(i) == SQLITE_TEXT) && (statement->GetColumnType(i) == SQLITE3_TEXT))
				{
					std::string name = statement->GetColumnName(i);
					std::string	value = statement->GetColumnString(name);

					properties.Append(name, value);
				}
			}

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
		trace_error << "Failed to get object properties" << std::endl;
	}

	return	true;
}

bool	DatabaseManager::AddDevice(Device *_device)
{
	Properties	properties;

	_device->GetProperties(properties);

	return	device_table_->Add(properties);
}

bool	DatabaseManager::DeleteDevice(std::string const& _id)
{
	return	device_table_->Delete(_id);
}

bool	DatabaseManager::IsDeviceExist(std::string const& _id)
{
	return	device_table_->IsExist(_id);
}

uint32_t	DatabaseManager::GetDeviceCount()
{
	return	device_table_->GetCount();
}

bool	DatabaseManager::GetDeviceProperties(uint32_t _index, uint32_t _count, std::list<Properties>& _properties_list)
{
	return	device_table_->GetProperties(_index, _count, _properties_list);
}

bool	DatabaseManager::GetDeviceProperties(std::string const& _id, Properties& _properties)
{
	return	device_table_->GetProperties(_id, _properties);
}

bool	DatabaseManager::SetDeviceProperties(std::string const& _id, Properties& _properties)
{
	return	device_table_->SetProperties(_id, _properties);
}

bool	DatabaseManager::AddEndpoint(Endpoint *_endpoint)
{
	Properties	properties;

	_endpoint->GetProperties(properties);

	return	endpoint_table_->Add(properties);
}

bool	DatabaseManager::DeleteEndpoint(std::string const& _id)
{
	return	endpoint_table_->Delete(_id);
}

bool	DatabaseManager::IsEndpointExist(std::string const& _id)
{
	return	endpoint_table_->IsExist(_id);
}

uint32_t	DatabaseManager::GetEndpointCount()
{
	return	endpoint_table_->GetCount();
}

bool	DatabaseManager::GetEndpointProperties(uint32_t _index, uint32_t _count, std::list<Properties>& _properties_list)
{
	return	endpoint_table_->GetProperties(_index, _count, _properties_list);
}

bool	DatabaseManager::GetEndpointProperties(std::string const& _id, Properties& _properties)
{
	return	endpoint_table_->GetProperties(_id, _properties);
}

bool	DatabaseManager::SetEndpointProperties(std::string const& _id, Properties& _properties)
{
	return	endpoint_table_->SetProperties(_id, _properties);
}

void	DatabaseManager::Preprocess()
{
	try 
	{
		trace_info << std::setw(16) << "DB Name : " << file_name_ << std::endl;
		database_ = new Kompex::SQLiteDatabase(file_name_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
		trace_info << std::setw(16) << "SQLite Version : " << database_->GetLibVersionNumber() << std::endl;

		device_table_ = CreateTable("devices");	

		endpoint_table_ = CreateTable("endpoints");	
	}
  	catch(Kompex::SQLiteException &exception)
	{
		std::cerr << "\nException occured" << std::endl;
		exception.Show();
		std::cerr << "SQLite result code: " << exception.GetSqliteResultCode() << std::endl;
	}
}

void	DatabaseManager::Process()
{
}

void	DatabaseManager::Postprocess()
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
		std::cerr << "\nException occured" << std::endl;
		exception.Show();
		std::cerr << "SQLite result code: " << exception.GetSqliteResultCode() << std::endl;
	}
}

Kompex::SQLiteStatement*	DatabaseManager::CreateStatement()
{
	return	new Kompex::SQLiteStatement(database_);
}
