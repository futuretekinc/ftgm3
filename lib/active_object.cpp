#include <unistd.h>
#include <iomanip>
#include <map>
#include <string>
#include "trace.h"
#include "defined.h"
#include "json.h"
#include "active_object.h"
#include "timer.h"
#include "message_queue.h"

using namespace	std;

ActiveObject::ActiveObject()
: Object(), stop_(true), loop_interval_(ACTIVE_OBJECT_LOOP_INTERVAL), thread_(), message_queue_(*this)
{
	Message::RegisterRecipient(id_, this);
}

ActiveObject::ActiveObject(std::string const& _id)
: Object(_id), stop_(true), loop_interval_(ACTIVE_OBJECT_LOOP_INTERVAL), thread_(), message_queue_(*this)
{
	Message::RegisterRecipient(id_, this);
}

ActiveObject::~ActiveObject()
{
	Message::UnregisterRecipient(id_);
}

ActiveObject::operator JSONNode() const
{
	JSONNode	root;

	JSONNode	trace_config = trace;
	trace_config.set_name(TITLE_NAME_TRACE);
	root.push_back(trace_config);

	return	root;
}

bool	ActiveObject::SetEnable(bool _enable)
{
	if (enable_ != _enable)
	{	
		if (Object::SetEnable(_enable))
		{
			if (!enable_)
			{
				Stop();	
			}
		}
		else
		{
			return	false;	
		}
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

bool	ActiveObject::Start(uint32_t _wait_for_init_time)
{
	bool	ret_value = true;
	Timer	timer;

	if (enable_)
	{
		timer += _wait_for_init_time * TIME_MILLISECOND;

		if (!thread_.joinable())
		{
			thread_.create(ActiveObject::ThreadMain, (void *)this);
			TRACE_INFO("Object[" << GetTraceName() << "] has been started.");
			while(timer.RemainTime() != 0)
			{
				if ((!stop_) && IsRunning())
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
			TRACE_INFO("Object[" << GetTraceName() << "] stopped");
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

bool	ActiveObject::SetLoopInterval(uint32_t _interval)
{
	loop_interval_ = _interval;

	JSONNodeUpdate(updated_properties_, TITLE_NAME_LOOP_INTERVAL, loop_interval_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	ActiveObject::SetLoopInterval(std::string const& _interval, bool _check)
{
	if (!_check)
	{
		loop_interval_ = strtoul(_interval.c_str(), 0, 10);

		JSONNodeUpdate(updated_properties_, TITLE_NAME_LOOP_INTERVAL, loop_interval_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

bool	ActiveObject::GetProperties(JSONNode& _properties, Fields const& _fields)
{
	if (!Object::GetProperties(_properties, _fields))
	{
		return	false;	
	}

	if (_fields.loop_interval)
	{
		_properties.push_back(JSONNode(TITLE_NAME_LOOP_INTERVAL, loop_interval_));
	}

	return	true;
}


bool	ActiveObject::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;

	try
	{
		if (_property.name() == TITLE_NAME_LOOP_INTERVAL)
		{
			ret_value = SetLoopInterval(_property.as_string(), _check);
		}
		else
		{
			ret_value = Object::SetProperty(_property, _check);
		}
	}
	catch(std::exception& e)
	{
		TRACE_ERROR(e.what());
		ret_value = false;
	}

	return	ret_value;
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

bool	ActiveObject::OnMessage(Message* _base)
{
	TRACE_ERROR("Unknown message[" << _base->GetType() << "]");	
	return	true;
}

void	ActiveObject::Preprocess()
{
}

void	ActiveObject::Process()
{
	if (message_queue_.Count() != 0)
	{
		Message*	message = message_queue_.Pop();
		if (message != NULL)
		{
			bool ret_value = OnMessage(message);
			if (ret_value)
			{
				delete message;
			}
		}
		else
		{
			TRACE_ERROR("Queue is abnormal!!");
		}
	}
}

void	ActiveObject::Postprocess()
{
}

void *ActiveObject::ThreadMain(void *data)
{
	Timer	loop_timer;
	ActiveObject* _object = (ActiveObject*)data;

	_object->Preprocess();
	_object->stop_ = false;

	while(!_object->stop_)
	{
		loop_timer += _object->loop_interval_;

		_object->Process();

		usleep(loop_timer.RemainTime());
	}

	TRACE_INFO2(_object, "Entry post process");
	_object->Postprocess();
}

bool	ActiveObject::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<ActiveObject*>(_object) != NULL;
}
