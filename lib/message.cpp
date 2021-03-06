#include "message.h"
#include <string>
#include <iomanip>
#include <map>
#include "mutex.h"
#include "active_object.h"

using namespace std;

struct	MessageTypeString
{
	uint32_t		type;
	string			name;
}
message_type_string[] =
{
	{ MSG_TYPE_UNKNOWN,	"Unknown" },
	{ MSG_TYPE_START,		"start"	},
	{ MSG_TYPE_STARTED,	"started"	},
	{ MSG_TYPE_STOP,		"stop"	},
	{ MSG_TYPE_STOPPED,	"stopped"	},
	{ MSG_TYPE_ACTIVATED,	"activated" },
	{ MSG_TYPE_DEACTIVATED,"deactivated" },
	{ MSG_TYPE_TEST,		"test"	},
	{ MSG_TYPE_QUIT,		"quit"	},
	{ MSG_TYPE_SESSION_DISCONNECTED, "Session disconnected" },
	{ MSG_TYPE_PACKET_RECEIVED,	"Packet received" }
};

static Mutex								active_object_map_lock;
static std::map<std::string,ActiveObject*>	active_object_map;
static Locker								message_list_locker;
static std::list<Message*>			message_list;

Message::Message(const Message& _message)
: type_(_message.type_), sender_(_message.sender_) 
{
	time_ = Date::GetCurrent();

	id_ = time_.GetMicroSecond();
#if DEBUG_MESSAGE
	message_list_locker.Lock();
	message_list.push_back(this);
	message_list_locker.Unlock();
#endif
}

Message::Message(uint32_t	_type)
: type_(_type), sender_("") 
{
	time_ = Date::GetCurrent();

	id_ = time_.GetMicroSecond();

#if DEBUG_MESSAGE
	message_list_locker.Lock();
	message_list.push_back(this);
	message_list_locker.Unlock();
#endif
}

Message::Message(uint32_t	_type, const std::string& _sender)
: type_(_type), sender_(_sender) 
{
	time_ = Date::GetCurrent();

	id_ = time_.GetMicroSecond();

#if DEBUG_MESSAGE
	message_list_locker.Lock();
	message_list.push_back(this);
	message_list_locker.Unlock();
#endif
}

Message::~Message()
{
#if DEBUG_MESSAGE
	message_list_locker.Lock();

	bool	found = false;
	std::cout << "Message List Count : " << message_list.size() << std::endl;
	for(std::list<Message*>::iterator it = message_list.begin() ; it != message_list.end() ; it++)
	{
		if (this == (*it))
		{
			message_list.erase(it);
			found = true;
			break;	
		}
	}
	message_list_locker.Unlock();

	if (!found)
	{
		std::cerr << "Message invalid!" << std::endl;
	}
#endif
}

void	Message::RegisterRecipient(std::string const& _id, ActiveObject* _object)
{
	active_object_map_lock.lock();
	active_object_map.insert(std::pair<std::string,ActiveObject*>(_id, _object));	
	active_object_map_lock.unlock();
}

void	Message::UnregisterRecipient(std::string const& _id)
{
	active_object_map_lock.lock();
	active_object_map.erase(_id);
	active_object_map_lock.unlock();
}

void	Message::Send(std::string const& _sender, Message* _message)
{
	active_object_map_lock.lock();
	std::map<std::string,ActiveObject*>::iterator object_it = active_object_map.find(_sender);
	if (object_it == active_object_map.end())
	{
		TRACE_ERROR2(NULL, "Unknwon target[" << _sender << "]");
		delete _message;
	}
	else
	{
		TRACE_INFO2(NULL, "Packet send to target[" << _sender << "]");
		object_it->second->Post(_message);	
	}
	active_object_map_lock.unlock();
}

void	Message::SendPacket(std::string const& _target, std::string const& _sender, void* _data, uint32_t _length)
{
	try
	{
		Message*	message = new MessagePacket(_sender, _data, _length);

		Send(_target, message);
	}
	catch(std::bad_alloc &e)
	{
		TRACE_ERROR2(NULL, "Failed to create message!");
	}
}

void	Message::SendPacket(std::string const& _target, std::string const& _sender, std::string const& _message)
{
	try
	{
		Message*	message = new MessagePacket(_sender, _message.c_str(), _message.size());

		Send(_target, message);
	}
	catch(std::bad_alloc &e)
	{
		TRACE_ERROR2(NULL, "Failed to create message!");
	}
}

const 
string&	ToString
(
	Message* _message
)
{
	static	string	unknown("unknown");

	for(int i = 0 ; sizeof(message_type_string) / sizeof(MessageTypeString) ; i++)
	{
	 	if (_message->GetType() == message_type_string[i].type)
		{
			return	message_type_string[i].name;
		}
	}

	return	unknown;
}


uint32_t	Message::ToType
(
	const std::string& _string
)
{
	for(int i = 0 ; sizeof(message_type_string) / sizeof(MessageTypeString) ; i++)
	{
	 	if (_string == message_type_string[i].name)
		{
			return	message_type_string[i].type;
		}
	}

	return	MSG_TYPE_UNKNOWN;
}
	

const std::string&	Message::ToString
(
	uint32_t	_type
)
{
	for(int i = 0 ; sizeof(message_type_string) / sizeof(MessageTypeString) ; i++)
	{
	 	if (_type == message_type_string[i].type)
		{
			return	message_type_string[i].name;
		}
	}

	return	message_type_string[0].name;
}

std::ostream& operator<<(std::ostream& os, Message const& _message)
{
	_message.Dump(os);

	return	os;
}

void	Message::Dump(std::ostream& os) const
{
}


MessageTest::MessageTest
(
	const string& _message
)
: Message(MSG_TYPE_TEST), message(_message)
{
}

MessageActivated::MessageActivated()
: Message(MSG_TYPE_ACTIVATED)
{
}

MessageDeactivated::MessageDeactivated()
: Message(MSG_TYPE_DEACTIVATED)
{
}

MessageQuit::MessageQuit()
: Message(MSG_TYPE_QUIT)
{
}

MessagePacket::MessagePacket(std::string const& _sender, void const* _data, uint32_t _length)
: Message(MSG_TYPE_PACKET, _sender), length_(_length)
{
	if (_length == 0)
	{
		data_ = NULL;
	}
	else
	{
		data_ = new uint8_t[_length + 1];

		memset(data_, 0, _length + 1);
		memcpy(data_, _data, _length);
	}
}

MessagePacket::~MessagePacket()
{
	if(data_ != NULL)
	{
		delete data_;	
	}
}


void	MessagePacket::Dump(ostream& os) const
{
	uint32_t	i;

	os << "Type : PACKET" << std::endl;
	for(i = 0 ; i < length_ ; i++)
	{
		if ((i % 16) == 0)
		{
			os << std::hex << std::setw(4) << std::setfill('0') << i << " : ";
		}

		os << std::hex << std::setw(2) << std::setfill('0') << (int)((uint8_t *)data_)[i] << " ";
		if ((i + 1) % 16 == 0)
		{
			os<< std::endl;	
		}
	}

	if (i % 16 != 0)
	{
		os<< std::endl;	
	}
}

MessageKeepAlive::MessageKeepAlive(std::string const& _sender, std::string const& _object_id)
:	Message(MSG_TYPE_KEEP_ALIVE, _sender), object_id(_object_id)
{
}

