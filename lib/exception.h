#ifndef	EXCEPTION_H_
#define	EXCEPTION_H_

#include <exception>
#include <string>
#include <sstream>

class	Asserted : public std::exception
{
public:
	explicit Asserted(std::string const& _argument);

	virtual const char* 	what() const throw();

protected:
	std::string	message_;
};

#define	ASSERT(x)	{	if (!(x)) { std::ostringstream oss; oss << ##x; throw Asserted(oss.str());} }

class	InvalidArgument : public std::exception
{
public:
	explicit InvalidArgument(std::string const& _argument);
	explicit InvalidArgument(std::string const& _name, std::string const& _value);

	virtual const char* 	what() const throw();

protected:
	std::string	message_;
};

#define	THROW_INVALID_ARGUMENT(x)	{	std::ostringstream oss; oss << x; throw InvalidArgument(oss.str()); }

class	ObjectNotFound : public std::exception
{
public:
	explicit ObjectNotFound(std::string const& _object_id);

	const	std::string&	GetObjectID()		{	return	object_id_;		}
	virtual const char* 	what() const throw();

protected:
	std::string	object_id_;
	std::string	message_;
};

#define	THROW_OBJECT_NOT_FOUND(x)	{	std::ostringstream oss; oss << x; throw ObjectNotFound(oss.str()); }

class	NotInitialized : public std::exception
{
public:
	explicit	NotInitialized(std::string const& _message);

protected:
	std::string	message_;
};

#endif
