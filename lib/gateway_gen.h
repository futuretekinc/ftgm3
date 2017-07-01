#ifndef	GATEWAY_GEN_H_
#define	GATEWAY_GEN_H_

#include "gateway.h"

class	GatewayGen : public Gateway
{
public:
	GatewayGen(ObjectManager& _manager, JSONNode const& _properties);

	virtual	bool		IsIncludedIn(std::string const& _type);

	static	const char*	Type();
};

#endif
