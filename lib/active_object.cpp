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


ActiveObject::ActiveObject(Object* _parent)
: Object(_parent), stop_(true), loop_interval_(ACTIVE_OBJECT_LOOP_INTERVAL)
{
	Message::RegisterRecipient(id_, this);
}

ActiveObject::ActiveObject(ValueID const& _id, Object* _parent)
: Object(_id, _parent), stop_(true), loop_interval_(ACTIVE_OBJECT_LOOP_INTERVAL)
{
	Message::RegisterRecipient(id_, this);
}

ActiveObject::~ActiveObject()
{
	Message::UnregisterRecipient(id_);
}

bool	ActiveObject::Load(JSONNode const& _json)
{
	bool	ret_value = true;

	if (_json.name() == TITLE_NAME_TRACE)
	{
		ret_value = trace.Load(_json);
	}
	else
	{
		TRACE_ERROR("Invalid json format");
		ret_value = false;
	}

	return	ret_value;
}

ActiveObject::operator JSONNode() const
{
	JSONNode	root;

	JSONNode	trace_config = trace;
	trace_config.set_name(TITLE_NAME_TRACE);
	root.push_back(trace_config);

	return	root;
}

bool	ActiveObject::SetEnable(bool _enable, bool _store)
{
	if (enable_ != _enable)
	{	
		return	Object::SetEnable(_enable, _store);
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

	return	true;
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

bool	ActiveObject::Start()
{
	bool	ret_value = true;
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
				usleep(TIME_MILLISECOND);
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
		ret_value = false;
	}

	return	ret_value;
}

bool	ActiveObject::Stop(bool _wait)
{
	bool	ret_value = true;
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
		ret_value = false;
	}

	return	ret_value;
}

void	ActiveObject::Run()
{
	Start();

	while(!thread_.joinable())
	{
		usleep(TIME_MILLISECOND);
	}

	while(!stop_)
	{
		usleep(TIME_MILLISECOND);
	}

	thread_.join();
}

bool	ActiveObject::SetLoopInterval(Time const& _interval, bool _store)
{
	loop_interval_ = _interval;

	if (_store)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	ActiveObject::GetProperties(Properties& _properties) const
{
	if (Object::GetProperties(_properties))
	{
		_properties.AppendLoopInterval(loop_interval_);	

		return	true;
	}

	return	false;
}

bool	ActiveObject::SetPropertyInternal(Property const& _property, bool create)
{
	if (_property.GetName() == TITLE_NAME_LOOP_INTERVAL)
	{
		const ValueInt* value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (value != NULL)
		{
			return	SetLoopInterval(value->Get(), !create);
		}

		const ValueString* value2 = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value2 != NULL)
		{
			return	SetLoopInterval(strtoul(value2->Get().c_str(), 0, 10), !create);
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
