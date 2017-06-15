#ifndef	SERVER_LINKER_TEST_H_
#define	SERVER_LINKER_TEST_H_

#include "server_linker.h"

class	ServerLinkerTest : public ServerLinker
{
public:
	ServerLinkerTest();

protected:
		void		Preprocess();
		void		Process();
		void		Postprocess();

		bool		OnMessage(Message* _message);

		Timer		timer_;
		uint32_t	interval_;
};

#endif
