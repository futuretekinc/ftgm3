#include "defined.h"
#include "property.h"
#include "gateway_gen.h"

GatewayGen::GatewayGen(ObjectManager& _manager, Properties const& _properties)
:	Gateway(_manager, GatewayGen::Type())
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, PROPERTY_ALL, true);
}

GatewayGen::GatewayGen(ObjectManager& _manager, JSONNode const& _properties)
:	Gateway(_manager, GatewayGen::Type())
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, PROPERTY_ALL, true);
}

bool	GatewayGen::IsIncludedIn(ValueType const& _type)
{
	if (_type == Type())
	{
		return	true;
	}

	return	Gateway::IsIncludedIn(_type);	
}

const	ValueType&	GatewayGen::Type()
{
	static	ValueType	type_(NODE_TYPE_GW_GEN);

	return	type_;
}

