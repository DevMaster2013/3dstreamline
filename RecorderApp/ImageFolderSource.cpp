#include "stdafx.h"
#include "ImageFolderSource.h"
#include <Windows.h>

ImageFolderSource::ImageFolderSource()
	: ImageSource(0, "", 1)
	, _folderImages()
	, _currentImageFrame(0)
{
}

ImageFolderSource::ImageFolderSource(const std::string & input_folder, int frameRate)
	: ImageSource(0, input_folder, frameRate)
	, _folderImages()
	, _currentImageFrame(0)
{
	populateFolderImages();
}

ImageFolderSource::~ImageFolderSource()
{
	_folderImages.clear();
}

void ImageFolderSource::grabImage(cv::Mat& image, std::string& grappedFileName)
{
	if (_currentImageFrame >= _folderImages.size())
	{
		_endOfStream = true;
	}

	grappedFileName = _folderImages[_currentImageFrame];
	const std::string& frameName = _inputName + "\\" + grappedFileName;
	image = cv::imread(frameName);
	_currentImageFrame += _frameRate;
}

void ImageFolderSource::populateFolderImages()
{
	_folderImages.clear();
	WIN32_FIND_DATAA findData;
	HANDLE findHandle = FindFirstFileA((_inputName + "\\*.jpg").c_str(), &findData);
	BOOL found = findHandle != INVALID_HANDLE_VALUE;
	while (found)
	{
		_folderImages.push_back(findData.cFileName);
		found = FindNextFileA(findHandle, &findData);
	}
	FindClose(findHandle);
}
