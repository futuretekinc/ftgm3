#ifndef	MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include "value.h"

#define	MSG_TYPE_GLOBAL					0x00010000
#define	MSG_TYPE_START					(MSG_TYPE_GLOBAL + 1)
#define	MSG_TYPE_STARTED				(MSG_TYPE_GLOBAL + 2)
#define	MSG_TYPE_STOP					(MSG_TYPE_GLOBAL + 3)
#define	MSG_TYPE_STOPPED				(MSG_TYPE_GLOBAL + 4)
#define	MSG_TYPE_ACTIVATED				(MSG_TYPE_GLOBAL + 5)
#define	MSG_TYPE_DEACTIVATED			(MSG_TYPE_GLOBAL + 6)
#define	MSG_TYPE_TEST					(MSG_TYPE_GLOBAL + 7)
#define	MSG_TYPE_QUIT					(MSG_TYPE_GLOBAL + 8)
#define	MSG_TYPE_UNKNOWN				(MSG_TYPE_GLOBAL + 9)
#define	MSG_TYPE_SESSION_DISCONNECTED	(MSG_TYPE_GLOBAL + 10)
#define	MSG_TYPE_PACKET_RECEIVED		(MSG_TYPE_GLOBAL + 11)
#define	MSG_TYPE_PACKET					(MSG_TYPE_GLOBAL + 12)
#define	MSG_TYPE_KEEP_ALIVE				(MSG_TYPE_GLOBAL + 13)

class	ActiveObject;

struct	Message
{
public:

	uint32_t	type;
	std::string	id;

					Message(const Message& _message) : type(_message.type), id(_message.id) {};
					Message(uint32_t	_type = MSG_TYPE_UNKNOWN) : type(_type), id("") {};
					Message(uint32_t	_type, const std::string& _id) : type(_type), id(_id) {};


	virtual	void	Dump(std::ostream& os) const;

	static uint32_t	ToType(const std::string& _string);
	static const std::string&	ToString(uint32_t _type);

	static	void	RegisterRecipient(std::string const& _id, ActiveObject* _object);
	static	void	UnregisterRecipient(std::string const& _id);
	static	void	Send(std::string const& _id, Message* _message);

	static	void	SendPacket(ValueID const& _target, ValueID const& _sendoer, void* _data, uint32_t _len);
	static	void	SendPacket(ValueID const& _target, ValueID const& _sendoer, std::string const& _message);

	friend	const std::string&	ToString(Message* _message);
	friend	std::ostream& ::operator<<(std::ostream& os, Message const& _message);
};


struct	MessageStart : Message
{

	MessageStart() : Message(MSG_TYPE_START) {};
	MessageStart(const std::string& _id) : Message(MSG_TYPE_START, _id) {};
};

struct	MessageStarted : Message
{

	MessageStarted() : Message(MSG_TYPE_STARTED) {};
	MessageStarted(const std::string& _id) : Message(MSG_TYPE_STARTED, _id) {};
};

struct	MessageStop : Message
{

	MessageStop() : Message(MSG_TYPE_STOP) {};
	MessageStop(const std::string& _id) : Message(MSG_TYPE_STOP, _id) {};
};

struct	MessageStopped : Message
{

	MessageStopped() : Message(MSG_TYPE_STOPPED) {};
	MessageStopped(const std::string& _id) : Message(MSG_TYPE_STOPPED, _id) {};
};

struct	MessageTest : Message
{

	std::string	message;

	MessageTest(const std::string& _message);
};

struct	MessageActivated : Message
{

	MessageActivated();
};

struct	MessageDeactivated : Message
{

	MessageDeactivated();
};

struct	MessageQuit : Message
{
	MessageQuit();
};

struct MessagePacket : Message	
{
	ValueID		sender;

	uint8_t*	data;
	uint32_t	length;

	MessagePacket(ValueID const& _id, void const* _pdata, uint32_t _length);
	~MessagePacket();

	virtual	void	Dump(std::ostream& os) const;
};

struct MessageKeepAlive : Message	
{
	ValueID		id;

	MessageKeepAlive(ValueID const& _id);
};

#endif