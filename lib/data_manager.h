#ifndef	DATABASE_MANAGER_H_
#define	DATABASE_MANAGER_H_

#include <string>
#include <list>
#include "process_object.h"
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"
#include "gateway.h"
#include "device.h"
#include "rule.h"

class	DataManager : public ProcessObject
{
public:
	class	Table : public Object
	{
		public:
			Table(DataManager* _manager, std::string const& _name);

			std::string	GetClassName()	const	{	return	"DataManager::Table";	};
			bool		Add(JSONNode const& _properties);
			bool		Delete(std::string const& _id);
			bool		IsExist(std::string const& _id);
			uint32_t	GetCount();
			bool		GetProperties(uint32_t _index, uint32_t _count, JSONNode& _array);
			bool		GetProperties(std::string const& _id, JSONNode& _properties);
			bool		SetProperties(std::string const& _id, JSONNode& _properties);
			bool    	GetVersion(std::string const& _id, std::string& _version);
			bool    	SetVersion(std::string const& _id, std::string const& _version);
		protected:
			DataManager*	manager_;
	};

	class	ValueTable : public Object
	{
		public:
			ValueTable(DataManager* _manager, std::string const& _endpoint_id);

			bool	Add(time_t _time, std::string const& _value);

		protected:
			DataManager*	manager_;
	};

	DataManager();
	DataManager(std::string const& _db_name);
	~DataManager();

	const std::string&	GetDBFile();
			bool		SetDBFile(std::string const& _file_name, bool _check = false);

			bool		SetProperty(JSONNode const& _property, bool _check = false);
	virtual				operator JSONNode() const;

			bool		AddGateway(Gateway* _device);
			bool		DeleteGateway(std::string const& _id);
			bool		IsGatewayExist(std::string const& _id);
			uint32_t	GetGatewayCount();
			bool		GetGatewayProperties(uint32_t _index, uint32_t _count, JSONNode& _array);
			bool		GetGatewayProperties(std::string const& _id, JSONNode& _properties);
			bool		SetGatewayProperties(std::string const& _id, JSONNode& _properties);
		
			bool		AddDevice(Device* _device);
			bool		DeleteDevice(std::string const& _id);
			bool		IsDeviceExist(std::string const& _id);
			uint32_t	GetDeviceCount();
			bool		GetDeviceProperties(uint32_t _index, uint32_t _count, JSONNode& _array);
			bool		GetDeviceProperties(std::string const& _id, JSONNode& _properties);
			bool		SetDeviceProperties(std::string const& _id, JSONNode& _properties);
		
			bool		AddEndpoint(Endpoint* _endpoint);
			bool		DeleteEndpoint(std::string const& _id);
			bool		IsEndpointExist(std::string const& _id);
			uint32_t	GetEndpointCount();
			bool		GetEndpointProperties(uint32_t _index, uint32_t _count, JSONNode& _array);
			bool		GetEndpointProperties(std::string const& _id, JSONNode& _properties);
			bool		SetEndpointProperties(std::string const& _id, JSONNode& _properties);
		
			ValueTable*	GetValueTable(std::string const& _endpoint_id);
			bool		AddValue(std::string const& _endpoint_id, time_t _time, std::string const& _value);

			bool		AddRule(Rule* _endpoint);
			bool		DeleteRule(std::string const& _id);
			bool		IsRuleExist(std::string const& _id);
			uint32_t	GetRuleCount();
			bool		GetRuleProperties(uint32_t _index, uint32_t _count, JSONNode& _array);
			bool		GetRuleProperties(std::string const& _id, JSONNode& _properties);
			bool		SetRuleProperties(std::string const& _id, JSONNode& _properties);
		
			bool    	AddSystemInfo(JSONNode& properties);
			bool	    	UpdateSystemInfo(std::string const& _id, std::string const& _version);
			bool    	IsSysteminfoExist(std::string const& _id);
private:
	Table*		CreateTable(std::string const& _table_name, std::list<std::string>& field_list);
	ValueTable*	CreateValueTable(std::string const& _endpoint_id);

	bool		IsTableExist(std::string const& _name);
	Table*		GetTable(std::string const& _name);
	Kompex::SQLiteStatement*	CreateStatement();

	void		Preprocess();
	void		Process();
	void		Postprocess();

	bool						temp_db_;
	std::string					file_name_;
	Kompex::SQLiteDatabase*		database_;
	Table*						gateway_table_;
	Table*						device_table_;
	Table*						endpoint_table_;
	Table*						rule_table_;
	Table*						system_info_table_;
	std::map<std::string, ValueTable*>		value_table_map_;

};

#endif
