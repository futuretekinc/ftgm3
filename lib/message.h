#ifndef	MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include "value.h"

class	ActiveObject;

struct	Message
{
public:
	enum	Type
	{
		START,
		STARTED,
		STOP,
		STOPPED,
		ACTIVATED,
		DEACTIVATED,
		TEST,
		QUIT,
		UNKNOWN,
		SESSION_DISCONNECTED,
		PACKET_RECEIVED,
		PACKET
	};

	Type		type;
	std::string	id;

					Message(const Message& _message) : type(_message.type), id(_message.id) {};
					Message(Type _type = UNKNOWN) : type(_type), id("") {};
					Message(Type _type, const std::string& _id) : type(_type), id(_id) {};


	virtual	void	Dump(std::ostream& os) const;

	static Type		ToType(const std::string& _string);
	static const std::string&	ToString(Type _type);

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

	MessageStart() : Message(START) {};
	MessageStart(const std::string& _id) : Message(START, _id) {};
};

struct	MessageStarted : Message
{

	MessageStarted() : Message(STARTED) {};
	MessageStarted(const std::string& _id) : Message(STARTED, _id) {};
};

struct	MessageStop : Message
{

	MessageStop() : Message(STOP) {};
	MessageStop(const std::string& _id) : Message(STOP, _id) {};
};

struct	MessageStopped : Message
{

	MessageStopped() : Message(STOPPED) {};
	MessageStopped(const std::string& _id) : Message(STOPPED, _id) {};
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

#endif
