#include <cstring>
#include <iomanip>
#include "shell.h"

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


Shell::Shell
(
	Command* _commands[], 
	int 	_command_count, 
	void* _data
)
{
	out_	= &std::cout;
	data_ 	= _data;
	thread_ = NULL;
	stop_ 	= true;
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

void*	Shell::Data()
{	
	return	data_;	
}


RetValue	Shell::Start
(
	bool _sync
)
{
	if (thread_ != NULL)
	{
		return	RET_VALUE_OK;	
	}

	thread_ = new std::thread(Process, this);

	while(stop_)
	{
		usleep(1000);		
	}

	sync_ = _sync;
	if (sync_)
	{
		thread_->join();	

		thread_ = NULL;
	}

	return	RET_VALUE_OK;
}

RetValue	Shell::Stop()
{
	stop_ = true;

	return	RET_VALUE_OK;
}


void	Shell::Add
(
	Command* _commands[], 
	int 	_command_count
)
{
	thread_ = NULL;
	stop_ 	= true;

	for(int i = 0 ; i < _command_count ; i++)
	{
		auto it = command_map_.find(_commands[i]->name);
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
	auto it = command_map_.begin();

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
	auto it = command_map_.find(name_);
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

void 	Shell::Process
(
	Shell *_shell
)
{
	std::string	command_line;
	std::string	arguments[16];
	int		count;

	_shell->stop_	= false;

	while(!_shell->stop_)
	{
		std::cout << _shell->prompt_ << "> ";
		std::getline(std::cin, command_line);
		count = 0;

		count = Shell::Parser(command_line, arguments, 16);
		if (count != 0)
		{
			typename std::map<const std::string, Command*>::iterator it = _shell->command_map_.find(arguments[0])	;
			if (it != _shell->command_map_.end())
			{
				RetValue	ret_value;

				ret_value = it->second->function(arguments, count, _shell);
				if (ret_value == RET_VALUE_INVALID_ARGUMENTS)
				{
					std::cout << it->second->help << std::endl;
				}
			}
			else
			{
				std::cout << "Command not found : " << arguments[0] << std::endl;	
			}
		}
	}

	std::cout << "finished." << std::endl;
	
}

RetValue	CommandHelp(std::string [], uint32_t, Shell* _shell)
{
	uint32_t	count = _shell->GetCommandCount();
	for(uint32_t i = 0 ; i < count ; i++)
	{
		Shell::Command* command = _shell->GetCommandAt(i);

		std::cout << setw(16) << std::left << command->name << " " << command->short_help << std::endl;
	}

	return	RET_VALUE_OK;
}

Shell::Command	ShellCommandHelp = 
{
	.name		= "help",
	.help		= "<command>",
	.short_help= "Command help",
	.function	= CommandHelp
};

RetValue	CommandQuit(std::string [], uint32_t, Shell* _shell)
{
	_shell->Stop();

	return	RET_VALUE_OK;
}

Shell::Command	ShellCommandQuit = 
{
	.name		= "quit",
	.help		= "Quit",
	.short_help= "Quit",
	.function	= CommandQuit
};

Shell::Command*	ShellDefaultCommands[] =
{
	&ShellCommandQuit,
	&ShellCommandHelp
};

uint32_t	ShellDefaultCommandCount = sizeof(ShellDefaultCommands) / sizeof(Shell::Command*);
