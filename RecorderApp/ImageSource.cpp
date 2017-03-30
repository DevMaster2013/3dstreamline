#include "stdafx.h"
#include "ImageSource.h"
#include <fstream>
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

cv::Mat ImageSource::undistortImage(const cv::Mat & image)
{
	cv::Mat undistorted;
	cv::undistort(image, undistorted, _cameraMatrix, _cameraDistortParameters);
	return undistorted;
}

void ImageSource::parseCalibrationFile()
{
	std::ifstream calibStream(_calibrationFile);
	double fx, fy, cx, cy, k1, k2, p1, p2, k3, k4, k5, k6;
	calibStream >> fx >> fy >> cx >> cy >> k1 >> k2 >> p1 >> p2 >> k3 >> k4 >> k5 >> k6;

	_cameraMatrix = cv::Mat::zeros(3, 3, CV_64FC1);
	_cameraDistortParameters = cv::Mat::zeros(1, 8, CV_64FC1);

	_cameraMatrix.at<double>(0, 0) = fx;
	_cameraMatrix.at<double>(0, 1) = 0;
	_cameraMatrix.at<double>(0, 2) = cx;
	_cameraMatrix.at<double>(1, 0) = 0;
	_cameraMatrix.at<double>(1, 1) = fy;
	_cameraMatrix.at<double>(1, 2) = cy;
	_cameraMatrix.at<double>(2, 0) = 0;
	_cameraMatrix.at<double>(2, 1) = 0;
	_cameraMatrix.at<double>(2, 2) = 1;
	_cameraDistortParameters.at<double>(0, 0) = k1;
	_cameraDistortParameters.at<double>(0, 1) = k2;
	_cameraDistortParameters.at<double>(0, 2) = p1;
	_cameraDistortParameters.at<double>(0, 3) = p2;
	_cameraDistortParameters.at<double>(0, 4) = k3;
	_cameraDistortParameters.at<double>(0, 5) = k4;
	_cameraDistortParameters.at<double>(0, 6) = k5;
	_cameraDistortParameters.at<double>(0, 7) = k6;

	calibStream.close();
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
	parseCalibrationFile();
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
