#ifndef	DEVICE_U2U_H_
#define	DEVICE_U2U_H_

#include "device.h"
#include <termios.h>
#include <sys/signal.h>
#include "buffer.h"
#include "serial_u2u.h"

class	DeviceU2U : public Device
{
public:
	DeviceU2U(ObjectManager& _manager, std::string const& _type, bool _half = false);
	~DeviceU2U();

protected:
	virtual	bool		GetOptions(JSONNode& _optoins);
	virtual	bool		SetOption(JSONNode const& _option, bool _check = false);
	bool RequestandWait(uint8_t* _request, uint32_t _request_len, uint8_t* _response, uint32_t _max_response_len, uint32_t& _response_len, uint32_t _timeout);
			void		Preprocess();
			void		Process();
			void		Postprocess();

	Serialu2u			serial_u2u_;
	uint32_t			timeout_;	
};

#endif
