#include "defined.h"
#include "utils.h"
#include "exception.h"

Asserted::Asserted(std::string const& _argument) throw()
{
	std::ostringstream	oss;

	oss << "Asserted : " << _argument;

	message_ = oss.str();
}

Asserted::~Asserted() throw()
{
}

const char* Asserted::what() const throw()
{
	return	message_.c_str();
}

ShowUsage::ShowUsage(std::string const& _argument) throw()
{
	std::ostringstream	oss;

	oss << "Usage : " << _argument;

	message_ = oss.str();
}

ShowUsage::ShowUsage(std::string const& _name, std::string const& _value) throw()
{
	std::ostringstream	oss;

	oss << "The value type of the " << _name << " is invalid.[" << _value << "]";

	message_ = oss.str();
}

ShowUsage::~ShowUsage() throw()
{
}

const char* ShowUsage::what() const throw()
{
	return	message_.c_str();
}

InvalidArgument::InvalidArgument(std::string const& _argument) throw()
{
	std::ostringstream	oss;

	oss << "InvalidArgument : " << _argument;

	message_ = oss.str();
}

InvalidArgument::InvalidArgument(std::string const& _name, std::string const& _value) throw()
{
	std::ostringstream	oss;

	oss << "The value type of the " << _name << " is invalid.[" << _value << "]";

	message_ = oss.str();
}

InvalidArgument::~InvalidArgument() throw()
{
}

const char* InvalidArgument::what() const throw()
{
	return	message_.c_str();
}

ObjectNotFound::ObjectNotFound(std::string const& _id) throw()
{
	std::ostringstream	oss;

	object_id_  = _id;
	oss << "ObjectNotFound : " << _id;

	message_ = oss.str();
}

ObjectNotFound::~ObjectNotFound() throw()
{
}

const char* ObjectNotFound::what() const throw()
{
	return	message_.c_str();
}

RequestTimeout::RequestTimeout(std::string const& _message) throw()
{ 
	std::ostringstream	oss;
	oss << "Message[" << _message <<"] arrives late or is invalid.";

	message_ = oss.str();
}

RequestTimeout::~RequestTimeout() throw()
{
}

const char* RequestTimeout::what() const throw()
{
	return	message_.c_str();
}

ConnectTimeout::ConnectTimeout(std::string const& _message) throw()
{ 
	std::ostringstream	oss;
	oss << "Connetion timeout : " << _message;

	message_ = oss.str();
}

ConnectTimeout::~ConnectTimeout() throw()
{
}

const char* ConnectTimeout::what() const throw()
{
	return	message_.c_str();
}
