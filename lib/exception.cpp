#include "defined.h"
#include "utils.h"

Asserted::Asserted(std::string const& _argument)
{
	std::ostringstream	oss;

	oss << "Asserted : " << _argument;

	message_ = oss.str();
}

const char* Asserted::what() const throw()
{
	return	message_.c_str();
}

void	ThrowFunctionFailed(std::string const& _argument)
{
	std::ostringstream	oss;

	oss << "Invalid argument[" << _argument << "]";
	throw std::invalid_argument(oss.str());
}

InvalidArgument::InvalidArgument(std::string const& _argument)
{
	std::ostringstream	oss;

	oss << "InvalidArgument : " << _argument;

	message_ = oss.str();
}

InvalidArgument::InvalidArgument(std::string const& _name, Value const* _value)
{
	std::ostringstream	oss;

	oss << "The value type of the " << _name << " is invalid.[" << _value->GetTypeString() << "]";

	message_ = oss.str();
}

const char* InvalidArgument::what() const throw()
{
	return	message_.c_str();
}

ObjectNotFound::ObjectNotFound(std::string const& _id)
{
	std::ostringstream	oss;

	object_id_  = _id;
	oss << "ObjectNotFound : " << _id;

	message_ = oss.str();
}

const char* ObjectNotFound::what() const throw()
{
	return	message_.c_str();
}

