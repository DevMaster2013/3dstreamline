#include "ImageFolderSource.h"
#include <Windows.h>

ImageFolderSource::ImageFolderSource(const std::string & input_folder, int frameRate)
	: ImageSource(frameRate)
	, _inputFolder(input_folder)
	, _folderImages()
	, _currentImageFrame(0)
{
	populateFolderImages();
}

ImageFolderSource::~ImageFolderSource()
{
	_folderImages.clear();
}

void ImageFolderSource::processNextImage(cv::Mat& image, std::string& grappedFileName)
{
	if (_currentImageFrame >= _folderImages.size())
	{
		_endOfStream = true;
	}
	
	if (_currentImageFrame == 0)
		_organizerStopwatch.start();
	else
	{
		// Wait the frame time
		double elapsedTime = _organizerStopwatch.getElapsedTime();
		double frameTime = 1.0 / double(_frameRate);
		frameTime -= elapsedTime;
		if (frameTime > 0)
			Sleep(DWORD(frameTime * 1000));
		_organizerStopwatch.start();
	}

	grappedFileName = _folderImages[_currentImageFrame];
	const std::string& frameName = _inputFolder + "\\" + grappedFileName;
	image = cv::imread(frameName);
	_currentImageFrame += _frameRate;

	// Undistort the image using calibration file


	// the jpeg compression parameters
	std::vector<int> compressionParameters;
	compressionParameters.push_back(CV_IMWRITE_JPEG_QUALITY);
	compressionParameters.push_back(100);

	// Save the image in the output folder undistorted
	std::string fullPath = _OUTPUT_DEBUG_STRING_INFO + "\\" + imageFileName;
	cv::imwrite(fullPath, image, compressionParameters);
}

void ImageFolderSource::populateFolderImages()
{
	_folderImages.clear();
	WIN32_FIND_DATAA findData;
	HANDLE findHandle = FindFirstFileA((_inputFolder + "\\*.jpg").c_str(), &findData);
	BOOL found = findHandle != INVALID_HANDLE_VALUE;
	while (found)
	{
		_folderImages.push_back(findData.cFileName);
		found = FindNextFileA(findHandle, &findData);
	}
	FindClose(findHandle);
}
