#ifndef	OBJECT_MANAGER_H_
#define	OBJECT_MANAGER_H_

#include <string>
#include <map>
#include "active_object.h"
#include "device.h"

class	ObjectManager : public ActiveObject
{
public:
	ObjectManager();
	~ObjectManager();

			bool	Load(std::string const& _file_name);
			bool	Load(const char *_json);
			bool	Load(JSONNode const& _json);

protected:
	virtual	void	Preprocess();
	virtual	void	Process();
	virtual	void	Postprocess();

	std::map<std::string,Device*>	device_map_;
};

#endif
