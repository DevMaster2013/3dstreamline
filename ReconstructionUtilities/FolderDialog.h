#pragma once

#include "DLLExportDefiniton.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <string>
#include <shlobj.h>
#include <iostream>
#include <sstream>

namespace util
{
	class RECONUTIL_API FolderDialog
	{
	private:
		std::string* _selectedPath;

	public:
		FolderDialog();
		FolderDialog(const std::string& initialPath);
		~FolderDialog();

	public:
		bool showDialog();
		const char* getFolderPath();

	private:
		static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	};
}