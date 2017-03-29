#include "stdafx.h"
#include "ImageSource.h"

#include "ImageFolderSource.h"

ImageSource::ImageSource(int sourceType)
	: _calibrationFile("")
	, _outputFolder("")
	, _inputName("")
	, _frameRate(1)
	, _endOfStream(false)
	, _sourceType(sourceType)
{
}

ImageSource::ImageSource(int sourceType, const std::string& inputName, int frameRate)
	: _calibrationFile("")
	, _outputFolder("")
	, _inputName(inputName)
	, _frameRate(frameRate)
	, _endOfStream(false)
	, _sourceType(sourceType)
{
}

ImageSource::~ImageSource()
{
}

ImageSource * ImageSource::createImageSource(int sourceType)
{
	if (sourceType == 0)
		return new ImageFolderSource();
	return nullptr;
}

const char * ImageSource::getImageSourceName(int sourceType)
{
	if (sourceType == 0)
		return "Image Folder";
	return "Unknown Source";
}

int ImageSource::getFrameRate() const
{
	return _frameRate;
}

void ImageSource::setFrameRate(int frameRate)
{
	_frameRate = frameRate;
}

int ImageSource::getSourceType() const
{
	return 0;
}

void ImageSource::setSourceType(int sourceType)
{
}

const std::string & ImageSource::getInputName()
{
	return _inputName;
}

void ImageSource::setInputName(const std::string & inputName)
{
	_inputName = inputName;
}

const std::string & ImageSource::getCalibrationFile()
{
	return _calibrationFile;
}

void ImageSource::setCalibrationFile(const std::string & calibFile)
{
	_calibrationFile = calibFile;
}

const std::string & ImageSource::getOutputFolder()
{
	return _outputFolder;
}

void ImageSource::setOutputFolder(const std::string & outFolder)
{
	_outputFolder = outFolder;
}

bool ImageSource::isEndOfStream() const
{
	return _endOfStream;
}
