#include "defined.h"
#include "property.h"
#include "gateway_gen.h"

GatewayGen::GatewayGen(ObjectManager& _manager, JSONNode const& _properties)
:	Gateway(_manager, GatewayGen::Type())
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

bool	GatewayGen::IsIncludedIn(std::string const& _type)
{
	if (_type == Type())
	{
		return	true;
	}

	return	Gateway::IsIncludedIn(_type);	
}

const char*	GatewayGen::Type()
{
	return	OBJECT_TYPE_GW_GEN;
}

