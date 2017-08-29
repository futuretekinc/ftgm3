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

bool	ActiveObject::Start(uint32_t _timeout)
{
	bool	ret_value = true;

	if (enable_)
	{
		if (locker_.TryLock(_timeout))
		{
			if (activated_.TryLock(0))
			{
				activated_.Unlock();

				waiting_for_initialization_.Lock();
				thread_.create(ActiveObject::ThreadMain, (void *)this);

				if (waiting_for_initialization_.TryLock(_timeout))
				{
					TRACE_INFO("Object[" << GetTraceName() << "] has been started.");
				}
				else
				{
					TRACE_ERROR("The object[" << GetTraceName() <<"] has exceeded initialization wait time.");
				}
				waiting_for_initialization_.Unlock();
			}
			else
			{
				TRACE_INFO("The object[" << GetTraceName() << "] is already activated.");
				ret_value = false;
			}
		
			locker_.Unlock();
		}
		else
		{
			TRACE_INFO("The object[" << GetTraceName() << "] exceeded the start initialization time.");
			ret_value = false;
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
		if (locker_.TryLock(0))
		{
			if (!activated_.TryLock(0))
			{
				TRACE_INFO("Object[" << GetTraceName() << "] stopped");
				stop_ = true;

				if (_wait)
				{
					thread_.join();
				}
			}
			else
			{
				activated_.Unlock();
				TRACE_INFO("The object[" << GetTraceName() << "] is not activated!");
			}
		}
		else
		{
			TRACE_INFO("The object[" << GetTraceName() << "] exceeded the cleanup time.");
			ret_value = false;
		}
	}
	catch(exception& e)
	{
		TRACE_ERROR("Exception occurred[" << e.what());	
		ret_value = false;
	}

	return	ret_value;
}

bool	ActiveObject::Run(uint32_t _timeout)
{
	if (!Start(_timeout))
	{
		return	false;
	}

	while(!thread_.joinable())
	{
		usleep(TIME_MILLISECOND);
	}

	while(!stop_)
	{
		usleep(TIME_MILLISECOND);
	}

	thread_.join();

	return	true;
}

bool	ActiveObject::SetLoopInterval(uint32_t _interval)
{
	loop_interval_ = _interval;
#if 0
	JSONNodeUpdate(updated_properties_, TITLE_NAME_LOOP_INTERVAL, loop_interval_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}
#endif
	return	true;
}

bool	ActiveObject::SetLoopInterval(std::string const& _interval, bool _check)
{
	if (!_check)
	{
		loop_interval_ = strtod(_interval.c_str(), 0);
#if 0
		JSONNodeUpdate(updated_properties_, TITLE_NAME_LOOP_INTERVAL, loop_interval_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
#endif
	}

	return	true;
}

bool	ActiveObject::GetProperty(uint32_t _type, JSONNode& _property)
{
	bool	ret_value = true;

	if (_type == PROPERTY_STAT_FLAG)
	{
		if (GetState() == DISABLED)
		{
			_property = JSONNode(TITLE_NAME_STAT, ToString(GetState())); 
		}
		else
		{
			if (IsRunning())
			{
				_property = JSONNode(TITLE_NAME_STAT, "run");
			}
			else
			{
				_property = JSONNode(TITLE_NAME_STAT, "stop");
			}
		}
	}
	else
	{
		ret_value = Object::GetProperty(_type, _property);
	}

	return	ret_value;
}

#if 0
bool	ActiveObject::GetProperties(JSONNode& _properties, Fields const& _fields)
{
	if (!Object::GetProperties(_properties, _fields))
	{
		return	false;	
	}

#if 0
	if (_fields.loop_interval)
	{
		_properties.push_back(JSONNode(TITLE_NAME_LOOP_INTERVAL, loop_interval_));
	}
#endif

	return	true;
}
#endif

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

	TRACE_INFO2(_object, "The object[" << _object->GetTraceName() << "] start to activate.");
	_object->activated_.Lock();

	_object->Preprocess();
	_object->stop_ = false;
	TRACE_INFO2(_object, "The object[" << _object->GetTraceName() << "] has completed initialization.");
	_object->waiting_for_initialization_.Unlock();

	TRACE_INFO2(_object, "Object[" <<_object->GetName() << ": " << _object->loop_interval_);
	while(!_object->stop_)
	{
		loop_timer.Add(_object->loop_interval_);

		_object->Process();
		usleep(loop_timer.RemainTime());
	}

	TRACE_INFO2(_object, "Entry post process");
	_object->Postprocess();
	_object->activated_.Unlock();
}

bool	ActiveObject::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<ActiveObject*>(_object) != NULL;
}
