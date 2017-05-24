#ifndef	BUFFER_H_
#define	BUFFER_H_

class	Buffer
{
public:
	Buffer(int	max_size = 1024);
	~Buffer();
	
	int		Size();
	int		MaxSize();
	int		Get(char *_buffer, int _size);
	int		Put(char *_buffer, int _size);	

protected:
	int		first_;
	int		last_;
	char*	buffer_;
	int		max_size_;
};

#endif
