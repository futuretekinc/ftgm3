#ifndef	EXCEPTION_H_
#define	EXCEPTION_H_

#include <exception>
#include <string>

class	Asserted : public std::exception
{
public:
	explicit Asserted(std::string const& _argument);

	virtual const char* 	what() const throw();

protected:
	std::string	message_;
};

class	InvalidArgument : public std::exception
{
public:
	explicit InvalidArgument(std::string const& _argument);
	explicit InvalidArgument(std::string const& _name, std::string const& _value);

	virtual const char* 	what() const throw();

protected:
	std::string	message_;
};

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

class	NotInitialized : public std::exception
{
public:
	explicit	NotInitialized(std::string const& _message);

protected:
	std::string	message_;
};

#endif
