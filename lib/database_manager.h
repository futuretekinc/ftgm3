#ifndef	DATABASE_MANAGER_H_
#define	DATABASE_MANAGER_H_

#include <string>
#include <list>
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"
#include "device.h"

class	DatabaseManager : public ActiveObject
{
public:
	DatabaseManager();
	DatabaseManager(std::string const& _db_name);
	~DatabaseManager();

	class	Table
	{
	public:
		Table(DatabaseManager* _parent, std::string const& _name);
			
				bool		Add(Properties const& _properties);
				bool		Delete(std::string const& _id);
				bool		IsExist(std::string const& _id);
				uint32_t	GetCount();
				bool		GetProperties(uint32_t _index, uint32_t _count, std::list<Properties> _properties_list);
				bool		GetProperties(std::string const& _id, Properties& _properties);
				bool		SetProperties(std::string const& _id, Properties& _properties);

	protected:
		DatabaseManager*	parent_;
		std::string			name_;
	};


	bool		AddDevice(Device* _device);
	bool		DeleteDevice(std::string const& _id);
	bool		IsDeviceExist(std::string const& _id);
	uint32_t	GetDeviceCount();
	bool		GetDeviceProperties(uint32_t _index, uint32_t _count, std::list<Properties>& _list);
	bool		GetDeviceProperties(std::string const& _id, Properties& _properties);
	bool		SetDeviceProperties(std::string const& _id, Properties& _properties);

	bool		AddEndpoint(Endpoint* _endpoint);
	bool		DeleteEndpoint(std::string const& _id);
	bool		IsEndpointExist(std::string const& _id);
	uint32_t	GetEndpointCount();
	bool		GetEndpointProperties(uint32_t _index, uint32_t _count, std::list<Properties>& _list);
	bool		GetEndpointProperties(std::string const& _id, Properties& _properties);
	bool		SetEndpointProperties(std::string const& _id, Properties& _properties);

private:
	Table*		CreateTable(std::string const& _name);
	bool		IsTableExist(std::string const& _name);
	Table*		GetTable(std::string const& _name);
	Kompex::SQLiteStatement*	CreateStatement();

	void		Preprocess();
	void		Process();
	void		Postprocess();

	bool						temp_db_;
	std::string					file_name_;
	Kompex::SQLiteDatabase*		database_;
	Table*						device_table_;
	Table*						endpoint_table_;

};

#endif
