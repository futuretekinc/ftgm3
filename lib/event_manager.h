#ifndef	DATABASE_MANAGER_H_
#define	DATABASE_MANAGER_H_

#include <string>
#include <list>
#include "process_object.h"
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"
#include "gateway.h"
#include "device.h"

class	EventManager : public ProcessObject
{
public:
	EventManager();
	EventManager(std::string const& _db_name);
	~EventManager();

			bool		SetProperty(JSONNode const& _property, bool _check = false);
	virtual				operator JSONNode() const;
			
private:
	void		Preprocess();
	void		Process();
	void		Postprocess();

};

#endif
