#include <cstring>
#include <iomanip>
#include "shell.h"
#include "trace.h"
#include "exception.h"

using namespace std;

extern	Shell::Command*	ShellDefaultCommands[];
extern	uint32_t	ShellDefaultCommandCount;
#if 0
bool	IsCorrectOption
(
	const std::string& _argument, 
	const char *_option
)
{
	return	(strcasecmp(_argument.c_str(), _option) == 0);
}
#endif

Shell::Command::Command
(
	const Command& _command
)
{
	name = _command.name;
	help = _command.help;
	short_help = _command.short_help;
	function = _command.function;
	sub.resize(_command.sub.size());

	for(uint32_t i = 0 ; i < _command.sub.size() ; i++)
	{
		sub[i] = new Command(*_command.sub[i]);	
	}
}

Shell::Command::Command
(
	const std::string&  _name, 
	const std::string&  _help, 
	const std::string& 	_short_help, 
	RetValue	(*_function)(std::string [], uint32_t, Shell*)
)
{
	name = _name;
	help = _help;
	short_help = _short_help;
	function = _function;
}

Shell::Command::Command
(
	const std::string&  _name, 
	const std::string&  _help, 
	const std::string& 	_short_help, 
	RetValue	(*_function)(std::string [], uint32_t, Shell*),
	Command*	_sub_list,
	uint32_t	_sub_count
)
:	sub(_sub_count)
{
	name = _name;
	help = _help;
	short_help = _short_help;
	function = _function;

	for(uint32_t i = 0 ; i < _sub_count ; i++)
	{
		sub[i] = new Command(_sub_list[i]);
	}
}

Shell::Command::~Command()
{
	for(uint32_t i = 0 ; i < sub.size() ; i++)
	{
		delete sub[i];	
	}
}

Shell::Shell
(
	Command* _commands[], 
	int 	_command_count, 
	Object* _object
)
: ActiveObject()
{
	trace.SetClassName(GetClassName());
	name_ 	= "shell";
	enable_ = true;

	out_	= &std::cout;
	object_	= _object;
	sync_   = false;
	prompt_	= "shell";
	max_command_width_ = 8;

	for(int i = 0 ; i < ShellDefaultCommandCount ; i++)
	{
		command_map_[ShellDefaultCommands[i]->name] = ShellDefaultCommands[i];	
	}

	for(int i = 0 ; i < _command_count ; i++)

	{
		typename std::map<const std::string, Command *>::iterator it = command_map_.find(_commands[i]->name);
		if (it == command_map_.end())
		{
			command_map_[_commands[i]->name] = _commands[i];	

		}
	}
}

Shell::~Shell()
{
	typename std::map<const std::string, Command*>::iterator	it = command_map_.begin();
	for(;it != command_map_.end() ; it++)
	{
	}
}

std::ostream&	Shell::Out()
{
	return	*out_; 
}

Object*	Shell::GetObject()
{	
	return	object_;	
}


void	Shell::Add
(
	Command* _commands[], 
	int 	_command_count
)
{
	for(int i = 0 ; i < _command_count ; i++)
	{
		std::map<std::string, Command *>::iterator it = command_map_.find(_commands[i]->name);
		if (it == command_map_.end())
		{
			command_map_[_commands[i]->name] = _commands[i];	
		}
	}
}

uint32_t	Shell::GetCommandCount()
{
	return	command_map_.size();	
}

Shell::Command*	Shell::GetCommandAt
(
	uint32_t index
)
{
	std::map<std::string, Command *>::iterator it = command_map_.begin();

	while (it != command_map_.end()) 
	{
		if (index == 0)
		{
			return	it->second;
		}

		index--;
		it++;
	}

	return	NULL;
}

Shell::Command*	Shell::GetCommand
(
	const std::string& name_
)
{
	std::map<std::string, Command*>::iterator it = command_map_.find(name_);
	if (it != command_map_.end()) 
	{
		return	it->second;
	}

	return	NULL;
}

uint32_t	Shell::GetCommandWidth()
{
	return	max_command_width_;	
}


bool		Shell::OnMessage(Message* _message)
{
	return	ActiveObject::OnMessage(_message);
}

int		Shell::Parser
(
	const 	std::string& 	_command_line, 
 			std::string* 	_arguments, 
 			int 			_max_count
 )
{
	int	count = 0;

	if (_command_line.length() != 0)
	{
		char *word = NULL;
		const char *seperators = "\t \n\r";
		char *buffer = new char[_command_line.length() + 1];

		strcpy(buffer, _command_line.c_str());

		word = strtok(buffer, seperators);
		while((word != NULL) && (count < _max_count))
		{
			_arguments[count++] = word;
			word = strtok(NULL, seperators);
		}

		delete [] buffer;
	}

	return	count;
}

void 	Shell::Process()
{
	std::string	arguments[16];
	int		count;

	std::cout << prompt_ << "> ";
	std::getline(std::cin, command_line_);

	count = Shell::Parser(command_line_, arguments, 16);
	if (count != 0)
	{
		typename std::map<const std::string, Command*>::iterator it = command_map_.find(arguments[0])	;
		if (it != command_map_.end())
		{
			RetValue	ret_value;

			try
			{
				ret_value = it->second->function(arguments, count, this);
			}
			catch(InvalidArgument& e)
			{
				Out() << e.what() << std::endl;
				std::cout << it->second->help << std::endl;
			}
		}
		else
		{
			std::cout << "Command not found : " << arguments[0] << std::endl;	
		}
	}
}

void	Shell::Postprocess()
{
	std::cout << name_ << " terminated." << std::endl;
}

RetValue	CommandHelp(std::string _argv[], uint32_t _argc, Shell* _shell)
{
	if (_argc == 1)
	{
		uint32_t	count = _shell->GetCommandCount();
		for(uint32_t i = 0 ; i < count ; i++)
		{
			Shell::Command* command = _shell->GetCommandAt(i);

			std::cout << setw(16) << std::left << command->name << " " << command->short_help << std::endl;
		}
	}
	else if (_argc == 2)
	{
		uint32_t	i, count = _shell->GetCommandCount();
		for(i = 0 ; i < count ; i++)
		{
			Shell::Command* command = _shell->GetCommandAt(i);
			if (_argv[1] == command->name)
			{
				std::cout << "Usage : " << command->name << " " << command->help << std::endl;
				break;
			}
		}

		if (i == count)
		{
			return	RET_VALUE_INVALID_ARGUMENTS;
		}
	}
	else
	{
		return	RET_VALUE_INVALID_ARGUMENTS;
	}

	return	RET_VALUE_OK;
}

Shell::Command	ShellCommandHelp = Shell::Command("help", "<command>", "Command help", CommandHelp);
Shell::Command	ShellCommandHelp2 = Shell::Command("?", "<command>", "Command help", CommandHelp);

RetValue	CommandQuit(std::string [], uint32_t, Shell* _shell)
{
	_shell->Stop();

	return	RET_VALUE_OK;
}

Shell::Command	ShellCommandQuit = Shell::Command("quit", "Quit", "Quit", CommandQuit);

Shell::Command*	ShellDefaultCommands[] =
{
	&ShellCommandQuit,
	&ShellCommandHelp,
	&ShellCommandHelp2
};

uint32_t	ShellDefaultCommandCount = sizeof(ShellDefaultCommands) / sizeof(Shell::Command*);
