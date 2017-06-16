#ifndef	GATEWAY_GEN_H_
#define	GATEWAY_GEN_H_

#include "gateway.h"

class	GatewayGen : public Gateway
{
public:
	GatewayGen(ObjectManager& _manager, Properties const& _properties);
	GatewayGen(ObjectManager& _manager, JSONNode const& _properties);

	virtual	bool		IsIncludedIn(ValueType const& _type);

	static	const	ValueType&	Type();
};

#endif
