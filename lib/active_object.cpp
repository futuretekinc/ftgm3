#include <unistd.h>
#include <iomanip>
#include "defined.h"
#include "active_object.h"
#include "timer.h"

using namespace	std;

ActiveObject::ActiveObject()
: loop_interval_(ACTIVE_OBJECT_LOOP_INTERVAL)
{
}

void	ActiveObject::Start()
{
	if (!thread_.joinable())
	{
		thread_ = thread(ThreadMain, this);
	}
}

void	ActiveObject::Stop()
{
	if (thread_.joinable())
	{
		stop_ = true;
		thread_.join();	
	}
}

bool	ActiveObject::GetProperties(Properties& _properties)
{
	if (Object::GetProperties(_properties))
	{
		_properties.Append("loop_interval", loop_interval_);	

		return	true;
	}

	return	false;
}

bool	ActiveObject::SetProperty(Property const& _property, bool create)
{
	if (_property.GetName() == "loop_interval")
	{
		const ValueInt* value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (value != NULL)
		{
			loop_interval_ = value->Get();
			return	true;
		}
	}

	return	Object::SetProperty(_property, create);
}

bool	ActiveObject::IsRunning()
{
	return	!stop_;
}

void	ActiveObject::Print(std::ostream& os) const
{
	Object::Print(os);
	os << std::setw(16) << "Loop Interval : " << loop_interval_ << " us" << std::endl;
}

void	ActiveObject::Preprocess()
{
}

void	ActiveObject::Process()
{
}

void	ActiveObject::Postprocess()
{
}

void	ActiveObject::ThreadMain(ActiveObject* _object)
{
	Timer	loop_timer;

	_object->Preprocess();

	_object->stop_ = false;
	while(!_object->stop_)
	{
		loop_timer += _object->loop_interval_;

		_object->Process();

		usleep(loop_timer.RemainTime());
	}

	_object->Postprocess();
}
