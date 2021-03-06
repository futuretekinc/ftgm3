#include <exception>
#include "server_linker_test.h"
#include "trace.h"
#include "time2.h"

ServerLinkerTest::ServerLinkerTest()
: ServerLinker(), interval_(1)
{
}

void	ServerLinkerTest::Preprocess()
{

	ServerLinker::Preprocess();

	timer_.Set(Date::GetCurrent());
	timer_.Add(interval_);
}

void	ServerLinkerTest::Process()
{
	if (timer_.RemainTime() == 0)
	{
		timer_.Add(interval_);

		Date	date;	
		for(std::map<std::string, UpLink*>::iterator it = up_link_map_.begin(); it != up_link_map_.end() ; it++)
		{
			it->second->Send(date.ToString());
		}
	}

	ServerLinker::Process();
}

void	ServerLinkerTest::Postprocess()
{
}

bool	ServerLinkerTest::OnMessage(Message* _message)
{
	return	ServerLinker::OnMessage(_message);
}
