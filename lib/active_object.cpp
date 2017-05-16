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
		Object::SetEnable(_enable);
#if 0
		if (enable_)
		{
			Start();	
		}
		else
		{
			Stop();	
		}
#endif	
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
			TRACE_INFO("Object[" << GetTraceName() << "] has been started.");
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
			TRACE_INFO("Already started!");
		}
	}
	else
	{
		TRACE_ERROR("The object[" << GetTraceName() << "] is disabled.");	
	}
}

void	ActiveObject::Stop(bool _wait)
{
	try
	{
		if (thread_.joinable())
		{
			stop_ = true;

			if (_wait)
			{
				thread_.join();
			}
		}
	}
	catch(exception& e)
	{
		TRACE_ERROR("Exception occurred[" << e.what());	
	}
}

void	ActiveObject::Run()
{
	Start();

	while(!thread_.joinable())
	{
		usleep(1000);
	}

	while(!stop_)
	{
		usleep(1000);
	}

	thread_.join();
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

bool	ActiveObject::SetPropertyInternal(Property const& _property, bool create)
{
	if (_property.GetName() == "loop_interval")
	{
		const ValueInt* value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (value != NULL)
		{
			loop_interval_ = value->Get();
			TRACE_INFO("The loop interval of object[" << GetTraceName() <<"] was set to " << loop_interval_);
			return	true;
		}

		const ValueString* value2 = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value2 != NULL)
		{
			loop_interval_ = strtoul(value2->Get().c_str(), 0, 10);
			TRACE_INFO("The loop interval of object[" << GetTraceName() <<"] was set to " << loop_interval_);
			return	true;
		}

		TRACE_INFO("Property loop interval value type is incorrect!");
	}
	else
	{
		return	Object::SetPropertyInternal(_property, create);
	}

	return	false;
}

bool	ActiveObject::IsRunning()
{
	//return	!stop_;
	return	thread_.joinable();
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
