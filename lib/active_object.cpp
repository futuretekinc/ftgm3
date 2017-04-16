#include <unistd.h>
#include <iomanip>
#include "trace.h"
#include "defined.h"
#include "active_object.h"
#include "timer.h"

using namespace	std;

ActiveObject::ActiveObject()
: stop_(true), loop_interval_(ACTIVE_OBJECT_LOOP_INTERVAL)
{
}

void	ActiveObject::Start()
{
	Timer	timer;

	timer += 1000 * TIME_MILLISECOND;

	if (!thread_.joinable())
	{
		thread_ = thread(ThreadMain, this);
		trace_info << "Thread started : " << thread_.get_id() << std::endl;
		while(timer.RemainTime() != 0)
		{
			if (!stop_)
			{
				break;
			}
			usleep(1000);
		}
	}
	else
	{
		trace_info << "Already started!" << std::endl;
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

bool	ActiveObject::GetProperties(Properties& _properties) const
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
	trace_info << __func__ << " called" << std::endl;
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
