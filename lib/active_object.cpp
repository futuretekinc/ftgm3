#include <unistd.h>
#include <iomanip>
#include <map>
#include <string>
#include "trace.h"
#include "defined.h"
#include "active_object.h"
#include "timer.h"
#include "message_queue.h"

using namespace	std;


ActiveObject::ActiveObject()
: Object(), stop_(true), loop_interval_(ACTIVE_OBJECT_LOOP_INTERVAL)
{
	Message::RegisterRecipient(id_, this);
}

ActiveObject::~ActiveObject()
{
	Message::UnregisterRecipient(id_);
}

void	ActiveObject::SetEnable(bool _enable)
{
	if (enable_ != _enable)
	{
		enable_ = _enable;
		if (enable_)
		{
			Start();	
		}
		else
		{
			Stop();	
		}
	
	}
}

Object::Stat	ActiveObject::GetStat() const
{
	if (enable_)
	{
		if (stop_)	
		{
			return	STOP;	
		}
		else
		{
			return	RUN;	
		}
	}
	else
	{
		return	DISABLED;	
	}
}

void	ActiveObject::Start()
{
	Timer	timer;

	if (enable_)
	{
		timer += 1000 * TIME_MILLISECOND;

		if (!thread_.joinable())
		{
			thread_ = thread(ThreadMain, this);
			TRACE_INFO << "Object[" << id_ << "] has been started." << Trace::End;
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
			TRACE_INFO << "Already started!" << Trace::End;
		}
	}
	else
	{
		TRACE_ERROR << "The object[" << id_ << "] is disabled." << Trace::End;	
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

void	ActiveObject::Run()
{
	Start();

	while(!IsRunning())
	{
		usleep(1000);
	}

	while(IsRunning())
	{
		usleep(1000);
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
			TRACE_INFO << "The loop interval of object[" << id_ <<"] was set to " << loop_interval_ << Trace::End;
			return	true;
		}

		const ValueString* value2 = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value2 != NULL)
		{
			loop_interval_ = strtoul(value2->Get().c_str(), 0, 10);
			TRACE_INFO << "The loop interval of object[" << id_ <<"] was set to " << loop_interval_ << Trace::End;
			return	true;
		}

		TRACE_INFO << "Property loop interval value type is incorrect!" << Trace::End;
	}
	else
	{
		return	Object::SetProperty(_property, create);
	}

	return	false;
}

bool	ActiveObject::IsRunning()
{
	return	!stop_;
}

void	ActiveObject::Print(std::ostream& os) const
{
	Object::Print(os);
	os << std::setw(16) << "Loop Interval : " << loop_interval_ << " us" << Trace::End;
}

bool	ActiveObject::Post(Message* _message)
{
	message_queue_.Push(_message);

	return	true;
}

void	ActiveObject::OnMessage(Message* _message)
{
}

void	ActiveObject::Preprocess()
{
}

void	ActiveObject::Process()
{
	if (message_queue_.Count() != 0)
	{
		Message*	message = message_queue_.Pop();

		OnMessage(message);

		delete message;
	}
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

//		if (std::string(_object->name_) == "shell")
//		{
//			std::cout << "shell!!" << loop_timer.RemainTime() << std::endl;
//		}
		usleep(loop_timer.RemainTime());
	}

	_object->Postprocess();
}
