#ifndef	VALUE_H_
#define	VALUE_H_

#include <string>
#include <ostream>
#include <list>
#include "time2.h"

///////////////////////////////////////////////////////////////
// Class Value
///////////////////////////////////////////////////////////////
class	Value
{
public:
	virtual	~Value();

	virtual	bool		Set(const std::string& _value) = 0;
	virtual	Value*		Duplicate() const = 0;
	virtual	operator	std::string() const = 0;

	virtual	void		Print(std::ostream&	os) const = 0;
	friend	Value*	::Duplicate(Value const *);
	friend	std::ostream& ::operator<<(std::ostream& os, Value const& _value);
};

///////////////////////////////////////////////////////////////
// Class ValueNumber
///////////////////////////////////////////////////////////////
class	ValueNumber : public Value
{
public:
protected:
};

///////////////////////////////////////////////////////////////
// Class ValueInt
///////////////////////////////////////////////////////////////
class	ValueInt : public ValueNumber
{
public:
	ValueInt(int _value = 0);

			ValueInt& 	operator=(int _value);
						operator std::string() const;

			int			Get() const;
			void		Set(int _value);
			bool		Set(const std::string& _value);
			Value*		Duplicate() const;

			void		Print(std::ostream&	os) const;
protected:
	int	value_;
};

///////////////////////////////////////////////////////////////
// Class ValueUInt32
///////////////////////////////////////////////////////////////
class	ValueUInt32 : public ValueNumber
{
public:
	ValueUInt32(uint32_t _value = 0);

			ValueUInt32& 	operator=(uint32_t _value);
							operator std::string() const;

			uint32_t	Get() const;
			void		Set(uint32_t _value);
			bool		Set(const std::string& _value);
			Value*		Duplicate() const;

			void		Print(std::ostream&	os) const;
protected:
	uint32_t	value_;
};

///////////////////////////////////////////////////////////////
// Class ValueUInt64
///////////////////////////////////////////////////////////////
class	ValueUInt64 : public ValueNumber
{
public:
	ValueUInt64(uint64_t _value = 0);

			ValueUInt64& 	operator=(uint64_t _value);
							operator std::string() const;

			uint64_t	Get() const;
			void		Set(uint64_t _value);
			bool		Set(const std::string& _value);
			Value*		Duplicate() const;

			void		Print(std::ostream&	os) const;
protected:
	uint64_t	value_;
};

///////////////////////////////////////////////////////////////
// Class ValueBool
///////////////////////////////////////////////////////////////
class	ValueBool : public ValueNumber
{
public:
	ValueBool(bool _value = false);

			ValueBool& 	operator=(bool _value);
						operator bool() const;
						operator std::string() const;

			void		Set(bool _value);
			bool		Set(const std::string& _value);
			bool		Get() const;
			Value*		Duplicate() const;

			void		Print(std::ostream&	os) const;
protected:
	bool	value_;
};

///////////////////////////////////////////////////////////////
// Class ValueFloat
///////////////////////////////////////////////////////////////
class	ValueFloat : public ValueNumber
{
public:
	ValueFloat(float _value = 0);

			ValueFloat& operator=(float _value);
						operator float() const;
						operator std::string() const;

			bool		operator ==(const ValueFloat& _value);
			bool		operator >(const ValueFloat& _value);
			bool		operator >=(const ValueFloat& _value);
			bool		operator <(const ValueFloat& _value);
			bool		operator <=(const ValueFloat& _value);

			void		Set(float _value);
			bool		Set(const std::string& _value);
			float		Get() const;
			Value*		Duplicate() const;

			void		Print(std::ostream&	os) const;
protected:
	float	value_;
};

///////////////////////////////////////////////////////////////
// Class ValueString
///////////////////////////////////////////////////////////////
class	ValueString : public Value
{
public:
	ValueString();
	ValueString(char * _value);
	ValueString(std::string const& _value);

			ValueString& 	operator=(const std::string& _value);
						 	operator std::string() const;
			bool			operator==(std::string const& _value) const;
			bool			operator==(char *_value) const;

			bool			Set(const std::string& _value);
	const	std::string&	Get() const; 
			Value*			Duplicate() const;

						
			void		Print(std::ostream&	os) const;
protected:
	std::string	value_;	
};

///////////////////////////////////////////////////////////////
// Class ValueStringLimit
///////////////////////////////////////////////////////////////
class	ValueStringLimit: public ValueString
{
public:
	ValueStringLimit(uint32_t limit);

			ValueStringLimit& 	operator=(std::string& _value);
			ValueStringLimit& 	operator=(char *_value);

			bool				Set(const std::string& _value);
protected:
	uint32_t	limit_;	
};

///////////////////////////////////////////////////////////////
// Class ValueID
///////////////////////////////////////////////////////////////
class	ValueID : public ValueStringLimit
{
public:
	ValueID();
	ValueID(const std::string& _value);

			Value*		Duplicate() const;
	
	static	bool		IsValid(std::string const& _id);
};

///////////////////////////////////////////////////////////////
// Class	ValueName
///////////////////////////////////////////////////////////////
class	ValueName : public ValueStringLimit
{
public:
	ValueName();
	ValueName(const std::string& _value);

			Value*		Duplicate() const;

	static	bool		IsValid(std::string const& _name);
};

///////////////////////////////////////////////////////////////
// Class ValueIP
///////////////////////////////////////////////////////////////
class	ValueIP : public ValueString
{
public:
	ValueIP(const ValueIP& _ip);
	ValueIP(const std::string& _ip);

			bool		Set(const std::string& _value);
			Value*		Duplicate() const;
	const	ValueIP&	operator=(const std::string& _value);

	static	bool	IsValidIP(const std::string& _ip);
};

///////////////////////////////////////////////////////////////
// Class ValueUnit
///////////////////////////////////////////////////////////////
class	ValueUnit : public ValueString
{
public:
	ValueUnit(const ValueUnit& _unit);
	ValueUnit(const std::string& _unit);

			bool		Set(const std::string& _value);
			Value*		Duplicate() const;
	const	ValueUnit&	operator=(const std::string& _value);
};


///////////////////////////////////////////////////////////////
// Class ValueTime
///////////////////////////////////////////////////////////////
class	ValueTime : public Value
{
public:
	ValueTime(const ValueTime& _date);
	ValueTime(const Time& _date);

	const	ValueTime&	operator=(Time const& _value);
						operator std::string() const;

			bool		Set(const std::string& _value);
	const	Time&		Get() const;
			Value*		Duplicate() const;


			void		Print(std::ostream&	os) const;
protected:
	Time	value_;
};


///////////////////////////////////////////////////////////////
// Class ValueDate
///////////////////////////////////////////////////////////////
class	ValueDate : public Value
{
public:
	ValueDate(const ValueDate& _date);
	ValueDate(const Date& _date);

	const	ValueDate&	operator=(Date const& _value);
						operator std::string() const;

			bool		Set(const std::string& _value);
	const	Date&		Get() const;
			Value*		Duplicate() const;


			void		Print(std::ostream&	os) const;
protected:
	Date	value_;
};

///////////////////////////////////////////////////////////////
// Class ValueProperties
///////////////////////////////////////////////////////////////

class	Properties;
class	ValueProperties : public Value
{
public:
	ValueProperties(ValueProperties const& _value);
	ValueProperties(Properties const& _value);
	~ValueProperties();

	const	ValueProperties&	operator=(Properties const& _value);
						operator std::string() const;

			bool			Set(const std::string& _value);
	const	Properties&		Get() const;
			Value*			Duplicate() const;


			void		Print(std::ostream&	os) const;
protected:
	Properties*	value_;
};


///////////////////////////////////////////////////////////////
// Class ValuePropertiesList
///////////////////////////////////////////////////////////////

class	PropertiesList;
class	ValuePropertiesList : public Value
{
public:
	ValuePropertiesList(ValuePropertiesList const& _value);
	ValuePropertiesList(PropertiesList const& _value);
	~ValuePropertiesList();

	const	ValuePropertiesList&	operator=(PropertiesList const& _value);
						operator std::string() const;

			bool			Set(const std::string& _value);
	const	PropertiesList&	Get() const;
			Value*			Duplicate() const;


			void		Print(std::ostream&	os) const;
protected:
	PropertiesList* 	value_;
};


#endif
