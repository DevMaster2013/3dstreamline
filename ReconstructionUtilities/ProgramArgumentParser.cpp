#include "ProgramArgumentParser.h"
#include <stdlib.h>
#include <string.h>
#include <vector>

bool isArgument(const char* argument)
{
	if (argument[0] == '-')
		return true;
	return false;
}

util::ProgramArgumentParser::ProgramArgumentParser()
	: _arguments(nullptr)
{
}

util::ProgramArgumentParser::~ProgramArgumentParser()
{
	if (_arguments)
		delete[] _arguments;
}

void util::ProgramArgumentParser::parse(int argc, char * argv[])
{
	std::vector<Argument> tempArguments;
	int currentIndex = 0;
	while (currentIndex < argc)
	{
		char* currentArgument = argv[currentIndex++];
		if (isArgument(currentArgument))
		{
			const char* argumentValue = strchr(currentArgument, '=');
			if (argumentValue != nullptr)
			{
				Argument arg;
				currentArgument[(int)(argumentValue - currentArgument)] = 0;
				strcpy_s(arg.argName, currentArgument + 2);
				strcpy_s(arg.argValue, argumentValue + 1);
				tempArguments.push_back(arg);
			}
		}
	}

	_argumentsCount = (int)tempArguments.size();
	_arguments = new Argument[_argumentsCount];
	memcpy_s(_arguments, sizeof(Argument) * _argumentsCount, tempArguments.data(), sizeof(Argument) * _argumentsCount);
}

util::ProgramArgumentParser::Argument* util::ProgramArgumentParser::getArgumentInternal(const char* argName)
{
	for (int i = 0; i < _argumentsCount; i++)
	{
		if (strcmp(argName, _arguments[i].argName) == 0)
			return _arguments + i;
	}
	return nullptr;
}
