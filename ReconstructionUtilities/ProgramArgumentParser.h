#pragma once

#include "DLLExportDefiniton.h"
#include <sstream>

namespace util
{
	class RECONUTIL_API ProgramArgumentParser
	{
	private:
		struct Argument
		{
			char argName[4096];
			char argValue[4096];
		};

	private:
		Argument* _arguments;
		int _argumentsCount;

	public:
		ProgramArgumentParser();
		~ProgramArgumentParser();

	public:
		void parse(int argc, char* argv[]);	
		template<typename T>
		T getArgument(const char* argName)
		{
			Argument* arg = getArgumentInternal(argName);
			if (arg == nullptr)
				return T();
			std::stringstream str(arg->argValue);
			T value;
			str >> value;
			return value;
		}

		template<>
		std::string getArgument(const char* argName)
		{
			Argument* arg = getArgumentInternal(argName);
			if (arg == nullptr)
				return "";
			return arg->argValue;
		}

	private:
		Argument* getArgumentInternal(const char* argName);
	};
}
