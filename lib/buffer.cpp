#include <cstring>
#include "buffer.h"

Buffer::Buffer(int	_max_size)
{
	if (_max_size <= 0)
	{
		max_size_ = 1024;
	}
	else
	{
		max_size_ = _max_size;
	}

	max_size_ += 1;
	buffer_ = new char[max_size_];
	first_ = 0;
	last_ = 0;
}

Buffer::~Buffer()
{
	delete buffer_;
}

int		Buffer::Size()
{
	return	(last_ + max_size_ - first_) % max_size_;
}

int		Buffer::MaxSize()
{
	return	max_size_ - 1;
}
int		Buffer::Get(char *_buffer, int _size)
{
	int	data_len = 0;
	if(Size() < _size)
	{
		_size = Size();	
	}
	
	if ((first_ + _size) >= max_size_)
	{
		data_len = (max_size_ - first_ - 1);
		memcpy(_buffer, &buffer_[first_], data_len);
		memcpy(&_buffer[data_len], buffer_, _size - data_len);
	}
	else
	{
		memcpy(_buffer, &buffer_[first_], _size);
	}
	
	first_ = (first_ + _size) % max_size_;

	return	_size;
}

int		Buffer::Put(char *_buffer, int _size)
{
	if ((last_ + _size) < max_size_)
	{
		memcpy(&buffer_[last_], _buffer, _size);	
		last_ += _size;
	}
	else
	{
		memcpy(&buffer_[last_], _buffer, max_size_ - last_);	
		memcpy(buffer_, &_buffer[max_size_ - last_], _size - (max_size_ - last_));
		last_ = (last_ + _size) % max_size_;
		first_= (last_ + 1) % max_size_;
	}

	return	Size();
}

