#ifndef	SHELL_H_
#define	SHELL_H_

#include "defined.h"
#include <string>
#include <strings.h>
#include <map>
#include <vector>
#include <thread>
#include <unistd.h>
#include <iostream>
#include "active_object.h"

class	Shell : public ActiveObject
{
public:
	struct Command
	{
		std::string	name;
		std::string	short_help;
		std::string	help;	
		RetValue	(*function)(std::string _arguments[], uint32_t _count, Shell* _shell);

		std::vector<Command*>	sub;

		Command(const Command& _command);
		Command(const std::string&  _name, const std::string&  _help, const std::string& _short_help, RetValue	(*_function)(std::string [], uint32_t, Shell*));
		Command(const std::string&  _name, const std::string&  _help, const std::string& _short_help, RetValue	(*_function)(std::string [], uint32_t, Shell*), Command* _sub_command_list, uint32_t _sub_command_count);
		~Command();
	};

	Shell(Command* _commands[], int 	_command_count, Object* _object);
	~Shell();

	std::ostream&	Out();
	Object*			GetObject();

			void	Add(Command* _commands[], int 	_count);
		uint32_t	GetCommandCount();
		Command*	GetCommandAt(uint32_t _index);
		Command*	GetCommand(const std::string& _name);
		uint32_t	GetCommandWidth();

			bool	OnMessage(Message* _message);
protected:
	static int		Parser(const std::string& _command_line, std::string* _arguments, int _max_count);
			void 	Process();
			void	Postprocess();
			
			int		GetLine(std::string& _line);

	Object*			object_;
	bool			sync_;
	std::string		prompt_;
	std::string		command_line_;
	std::map<std::string, Command *>	command_map_;
	std::ostream	*out_;
	uint32_t		max_command_width_;

};

inline bool	IsCorrectOption(const std::string& _argument, const char *_option)
{
	return	strcasecmp(_argument.c_str(), _option) == 0;
}
#endif
