#ifndef	SHELL_FTGM_H_
#define	SHELL_FTGM_H_

#include "shell.h"


void	ThrowInvalidArgument(std::string const& _message);

extern	Shell::Command*	shell_ftgm_commands[];
extern	uint32_t		shell_ftgm_command_count;

#endif

