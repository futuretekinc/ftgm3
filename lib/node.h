#ifndef	NODE_H_
#define	NODE_H_

#include "active_object.h"
#include "timer.h"

class	ObjectManager;

class	Node : public ActiveObject
{
	friend class	ObjectManager;
public:
	enum	TYPE
	{
		GATEWAY,
		DEVICE,
		ENDPOINT
	};

	Node(ObjectManager& _manager, std::string const& _type);

	virtual	std::string GetType() const;
	//virtual	bool		SetType(std::string const& _type, bool _check = false);

	virtual	std::string	GetModel();

	virtual	bool		IsIncludedIn(std::string const& _type);

	virtual	bool		SetID(std::string const& _id, bool _check = false);

			bool		SetKeepAliveInterval(uint32_t _interval);
			bool		SetKeepAliveInterval(std::string const& _interval, bool _check = false);
			uint32_t	GetKeepAliveInterval()	{	return	keep_alive_interval_;	};
			Date		TimeOfExpire();

			bool		SetRegistered(bool _registered);
			bool		SetRegistered(std::string const& _registered, bool _check = false);
			bool		GetRegistered();

			bool		SetLocation(std::string const& _location, bool _check = false);
			std::string	GetLocation();

	virtual	bool		GetProperty(uint32_t _type, JSONNode& _property);
	//virtual	bool		GetProperties(JSONNode& _properties, Fields const& _fields = PROPERTY_ALL);

			bool		SetProperty(JSONNode const& _property, bool _check = false);

	virtual	bool		GetOptions(JSONNode& _properties);
	virtual	bool		SetOptions(JSONNode const& _properties, bool _check = false);
	virtual	bool		SetOption(JSONNode const& _property, bool _check = false);

			bool		ApplyChanges();


			void		Preprocess();
			void		Process();

	static  const char*	Type();

	static	bool		GetPropertyFieldList(std::list<std::string>& _field_list);
	static	bool		IsIncludeIn(Object* _object);

protected:
	ObjectManager&	manager_;
	std::string		type_;

	std::string		location_;
	uint32_t		keep_alive_interval_;
	Timer			keep_alive_timer_;

	bool			registered_;
	Locker			locker_;
};

#endif
