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
	enum Type
	{
		TYPE,
		NUMBER,
		BOOL,
		INT,
		UINT32,
		UINT64,
		FLOAT,
		STRING,
		LIMITED_STRING,
		NAME,
		IP,
		ID,
		UNIT,
		TIME,
		DATE,
		PROPERTIES,
		PROPERTIES_LIST
	}; 

			Value();
			Value(Date const& _date);
	virtual	~Value();

	virtual	Type		GetType() const = 0;
	virtual	std::string	GetTypeString() const;

	virtual	bool		Set(const std::string& _value) = 0;
	virtual	Value*		Duplicate() const = 0;
	virtual	operator	std::string() const = 0;

	virtual	void		Print(std::ostream&	os) const = 0;
	friend	Value*	::Duplicate(Value const *);
	friend	std::ostream& ::operator<<(std::ostream& os, Value const& _value);

	const	Date&		GetDate() const	{	return	date_;}
protected:
	Date	date_;
};

///////////////////////////////////////////////////////////////
// Class ValueNumber
///////////////////////////////////////////////////////////////
class	ValueNumber : public Value
{
public:
						ValueNumber();
						ValueNumber(Date const& _date);

			Type		GetType() const	{	return	NUMBER;	};
	virtual	std::string	GetTypeString() const;
protected:
};

///////////////////////////////////////////////////////////////
// Class ValueInt
///////////////////////////////////////////////////////////////
class	ValueInt : public ValueNumber
{
public:
	ValueInt(int _value = 0);
	ValueInt(int _value, Date const& _date);

			Type		GetType() const	{	return	INT;	};
	virtual	std::string	GetTypeString() const;

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
	ValueUInt32(uint32_t _value, Date const& _date);

			Type		GetType() const	{	return	UINT32;	};
	virtual	std::string	GetTypeString() const;

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
	ValueUInt64(uint64_t _value, Date const& _date);

			Type		GetType() const	{	return	UINT64;	};
	virtual	std::string	GetTypeString() const;

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
	ValueBool(bool _value, Date const& _date);

			Type		GetType() const	{	return	BOOL;	};
	virtual	std::string	GetTypeString() const;

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
	ValueFloat(float _value, Date const& _date);

			Type		GetType() const	{	return	FLOAT;	};
	virtual	std::string	GetTypeString() const;

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
	float		value_;
	uint32_t	precision_;
};

///////////////////////////////////////////////////////////////
// Class ValueString
///////////////////////////////////////////////////////////////
class	ValueString : public Value
{
public:
	ValueString();
	ValueString(char * _value);
	ValueString(char * _value, Date const& _date);
	ValueString(std::string const& _value);
	ValueString(std::string const& _value, Date const& _date);

			Type		GetType() const	{	return	STRING;	};
	virtual	std::string	GetTypeString() const;

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
// Class ValueType
///////////////////////////////////////////////////////////////
class	ValueType: public ValueString
{
public:
	ValueType(const ValueType& _unit);
	ValueType(const std::string& _unit);
	ValueType(const std::string& _unit, Date const& _date);

			Type		GetType() const	{	return	TYPE;	};
	virtual	std::string	GetTypeString() const;

			bool		Set(const std::string& _value);
			Value*		Duplicate() const;
	const	ValueType&	operator=(const std::string& _value);
};


///////////////////////////////////////////////////////////////
// Class ValueStringLimit
///////////////////////////////////////////////////////////////
class	ValueStringLimit: public ValueString
{
public:
	ValueStringLimit(uint32_t limit);

			Type		GetType() const	{	return	LIMITED_STRING;	};
	virtual	std::string	GetTypeString() const;

			ValueStringLimit& 	operator=(std::string const& _value);
			ValueStringLimit& 	operator=(char const* _value);

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
	ValueID(const std::string& _value, Date const& _date);

			Type		GetType() const	{	return	ID;	};
	virtual	std::string	GetTypeString() const;

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
	ValueName(const std::string& _value, Date const& _date);

			Type		GetType() const	{	return	NAME;	};
	virtual	std::string	GetTypeString() const;

			ValueName& 	operator=(const std::string& _value);

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
	ValueIP(const std::string& _ip, Date const& _date);

			Type		GetType() const	{	return	IP;	};
	virtual	std::string	GetTypeString() const;

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
	ValueUnit(const std::string& _unit, Date const& _date);

			Type		GetType() const	{	return	UNIT;	};
	virtual	std::string	GetTypeString() const;

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
	ValueTime(const Time& _time);
	ValueTime(const Time& _time, Date const& _date);

			Type		GetType() const	{	return	TIME;	};
	virtual	std::string	GetTypeString() const;

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
	ValueDate(const ValueDate& _value);
	ValueDate(const Date& _value);
	ValueDate(const Date& _value, Date const& _date);

			Type		GetType() const	{	return	DATE;	};
	virtual	std::string	GetTypeString() const;

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
	ValueProperties(ValueProperties const& _value, Date const& _date);
	ValueProperties(Properties const& _value);
	ValueProperties(Properties const& _value, Date const& _date);
	~ValueProperties();

			Type		GetType() const	{	return	PROPERTIES;	};
	virtual	std::string	GetTypeString() const;

	const	ValueProperties&	operator=(Properties const& _value);
						operator std::string() const;

			bool		Set(const std::string& _value);
	const	Properties&	Get() const;
			Value*		Duplicate() const;


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
	ValuePropertiesList(ValuePropertiesList const& _value, Date const& _date);
	ValuePropertiesList(PropertiesList const& _value);
	ValuePropertiesList(PropertiesList const& _value, Date const& _date);
	~ValuePropertiesList();

			Type		GetType() const	{	return	PROPERTIES_LIST;	};
	virtual	std::string	GetTypeString() const;

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
