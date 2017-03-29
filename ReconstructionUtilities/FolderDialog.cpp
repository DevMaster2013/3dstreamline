#include "FolderDialog.h"

util::FolderDialog::FolderDialog()
	: FolderDialog("")
{
}

util::FolderDialog::FolderDialog(const std::string & initialPath)
	: _selectedPath(new std::string(initialPath))
{
}

util::FolderDialog::~FolderDialog()
{
	if (_selectedPath != nullptr)
		delete _selectedPath;
}

bool util::FolderDialog::showDialog()
{
	char path[MAX_PATH];
	const char * path_param = _selectedPath->c_str();

	BROWSEINFO bi = { 0 };
	bi.lpszTitle = TEXT("Browse for folder...");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)path_param;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != 0)
	{
		//get the name of the folder and put it in path
		SHGetPathFromIDList(pidl, path);

		//free memory used
		IMalloc * imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}

		_selectedPath->assign(path);
		return true;
	}

	return false;
}

const char* util::FolderDialog::getFolderPath()
{
	return _selectedPath->c_str();
}

int util::FolderDialog::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	return 0;
}
