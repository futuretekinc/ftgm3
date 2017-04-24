#include "message.h"
#include <string>
#include <iomanip>
#include <mutex>
#include <map>
#include "active_object.h"

using namespace std;

struct	MessageTypeString
{
	Message::Type	type;
	string			name;
}
message_type_string[] =
{
	{ Message::UNKNOWN,	"Unknown" },
	{ Message::START,		"start"	},
	{ Message::STARTED,	"started"	},
	{ Message::STOP,		"stop"	},
	{ Message::STOPPED,	"stopped"	},
	{ Message::ACTIVATED,	"activated" },
	{ Message::DEACTIVATED,"deactivated" },
	{ Message::TEST,		"test"	},
	{ Message::QUIT,		"quit"	},
	{ Message::SESSION_DISCONNECTED, "Session disconnected" },
	{ Message::PACKET_RECEIVED,	"Packet received" }
};

static std::mutex							active_object_map_lock;
static std::map<std::string,ActiveObject*>	active_object_map;

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

void	Message::Send(std::string const& _id, Message* _message)
{
	active_object_map_lock.lock();
	auto object_it = active_object_map.find(_id);
	if (object_it == active_object_map.end())
	{
		TRACE_ERROR2 << "Unknwon target[" << _id << "]" << Trace::End;
		delete _message;
	}
	else
	{
		TRACE_INFO2 << "Packet send to target[" << _id << "]" << Trace::End;
		object_it->second->Post(_message);	
	}
	active_object_map_lock.unlock();
}

void	Message::SendPacket(ValueID const& _target, ValueID const& _sender, void* _data, uint32_t _length)
{
	try
	{
		Message*	message = new MessagePacket(_sender, _data, _length);

		Send(_target, message);
	}
	catch(std::bad_alloc &e)
	{
		TRACE_ERROR2 << "Failed to create message!" << Trace::End;
	}
}

void	Message::SendPacket(ValueID const& _target, ValueID const& _sender, std::string const& _message)
{
	try
	{
		Message*	message = new MessagePacket(_sender, _message.c_str(), _message.size());

		Send(_target, message);
	}
	catch(std::bad_alloc &e)
	{
		TRACE_ERROR2 << "Failed to create message!" << Trace::End;
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
	 	if (_message->type == message_type_string[i].type)
		{
			return	message_type_string[i].name;
		}
	}

	return	unknown;
}


Message::Type	Message::ToType
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

	return	UNKNOWN;
}
	

const std::string&	Message::ToString
(
	Type _type
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
: Message(TEST), message(_message)
{
}

MessageActivated::MessageActivated()
: Message(ACTIVATED)
{
}

MessageDeactivated::MessageDeactivated()
: Message(DEACTIVATED)
{
}

MessageQuit::MessageQuit()
: Message(QUIT)
{
}

MessagePacket::MessagePacket(ValueID const& _id, void const* _data, uint32_t _length)
: Message(PACKET), sender(_id), length(_length)
{
	if (_length == 0)
	{
		data = NULL;
	}
	else
	{
		data = new uint8_t[_length + 1];

		memset(data, 0, _length + 1);
		memcpy(data, _data, _length);
	}
}

MessagePacket::~MessagePacket()
{
	if(data != NULL)
	{
		delete data;	
	}
}


void	MessagePacket::Dump(ostream& os) const
{
	uint32_t	i;

	os << "Type : PACKET" << std::endl;
	for(i = 0 ; i < length ; i++)
	{
		if ((i % 16) == 0)
		{
			os << std::hex << std::setw(4) << std::setfill('0') << i << " : ";
		}

		os << std::hex << std::setw(2) << std::setfill('0') << (int)((uint8_t *)data)[i] << " ";
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
