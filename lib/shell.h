#ifndef	SHELL_H_
#define	SHELL_H_

#include "defined.h"
#include <string>
#include <strings.h>
#include <map>
#include <thread>
#include <unistd.h>
#include <iostream>

class	Shell
{
public:
	struct Command
	{
		std::string	name;
		std::string	short_help;
		std::string	help;	

		RetValue	(*function)(std::string _arguments[], uint32_t _count, Shell* _shell);

		Command(const Command& _command);
		Command(const std::string&  _name, const std::string&  _help, const std::string& _short_help, RetValue	(*_function)(std::string [], uint32_t, Shell*));
	};

	Shell(Command* _commands[], int 	_command_count, void* _data);
	~Shell();

	std::ostream&	Out();
	void*			Data();

	RetValue		Start(bool _sync = false);
	RetValue		Stop();
	void			Add(Command* _commands[], int 	_count);
	uint32_t		GetCommandCount();
	Command*		GetCommandAt(uint32_t _index);
	Command*		GetCommand(const std::string& _name);
	uint32_t		GetCommandWidth();

protected:
	static 
	int				Parser(const std::string& _command_line, std::string* _arguments, int _max_count);
	static
	void 			Process(Shell *_shell);

	void*			data_;
	std::thread*	thread_;
	bool			stop_;
	bool			sync_;
	std::string		prompt_;
	std::map<const std::string, Command*>	command_map_;
	std::ostream	*out_;
	uint32_t		max_command_width_;

};

inline bool	IsCorrectOption(const std::string& _argument, const char *_option)
{
	return	strcasecmp(_argument.c_str(), _option);
}
#endif
