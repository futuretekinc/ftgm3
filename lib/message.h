#ifndef	MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include "value.h"
#include <libjson/libjson.h>

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
#define	MSG_TYPE_CONSUME				(MSG_TYPE_GLOBAL + 14)

class	ActiveObject;

class	Message
{
public:

					Message(const Message& _message);
					Message(uint32_t	_type = MSG_TYPE_UNKNOWN);
					Message(uint32_t	_type, const std::string& _sender);
			
			uint32_t	GetType()		{	return	type_;	}
			uint64_t	GetID()			{	return	id_;	}
			std::string	GetSendoer()	{	return	sender_;	}
			Date		GetTime()		{	return	time_;	}

	virtual	void	Dump(std::ostream& os) const;

	static uint32_t	ToType(const std::string& _string);
	static const std::string&	ToString(uint32_t _type);

	static	void	RegisterRecipient(std::string const& _sender, ActiveObject* _object);
	static	void	UnregisterRecipient(std::string const& _sender);
	static	void	Send(std::string const& _sender, Message* _message);

	static	void	SendPacket(std::string const& _target, std::string const& _sendoer, void* _data, uint32_t _len);
	static	void	SendPacket(std::string const& _target, std::string const& _sendoer, std::string const& _message);

	friend	const std::string&	ToString(Message* _message);
	friend	std::ostream& ::operator<<(std::ostream& os, Message const& _message);

protected:
	uint32_t	type_;
	uint64_t	id_;
	Date		time_;
	std::string	sender_;

};


class	MessageStart : Message
{
public:

	MessageStart() : Message(MSG_TYPE_START) {};
	MessageStart(const std::string& _sender) : Message(MSG_TYPE_START, _sender) {};
};

class	MessageStarted : Message
{
public:

	MessageStarted() : Message(MSG_TYPE_STARTED) {};
	MessageStarted(const std::string& _sender) : Message(MSG_TYPE_STARTED, _sender) {};
};

class	MessageStop : Message
{
public:

	MessageStop() : Message(MSG_TYPE_STOP) {};
	MessageStop(const std::string& _sender) : Message(MSG_TYPE_STOP, _sender) {};
};

class	MessageStopped : Message
{
public:

	MessageStopped() : Message(MSG_TYPE_STOPPED) {};
	MessageStopped(const std::string& _sender) : Message(MSG_TYPE_STOPPED, _sender) {};
};

class	MessageTest : Message
{
public:

	std::string	message;

	MessageTest(const std::string& _message);
};

class	MessageActivated : Message
{
public:

	MessageActivated();
};

class	MessageDeactivated : Message
{
public:

	MessageDeactivated();
};

class	MessageQuit : Message
{
public:
	MessageQuit();
};

class	MessagePacket : public Message	
{
public:
	MessagePacket(std::string const& _sender, void const* _pdata, uint32_t _length);
	~MessagePacket();

			uint32_t	GetSize()	{	return	length_; }
			uint8_t*	GetData()	{	return	data_;	}
			uint8_t		GetData(int n)	{	return	data_[n];	}

	virtual	void	Dump(std::ostream& os) const;

protected:
	uint8_t*	data_;
	uint32_t	length_;
};

class	MessageKeepAlive : Message	
{
public:
	MessageKeepAlive(std::string const& _sender, std::string const& _object_id);

protected:
	std::string	object_id;
};


#endif
