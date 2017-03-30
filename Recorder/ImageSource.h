#pragma once

#include <opencv2\opencv.hpp>
class ImageSource
{
protected:
	int _frameRate;
	bool _endOfStream;

public:
	ImageSource(int frameRate);
	virtual ~ImageSource();

public:
	virtual void processNextImage(cv::Mat& image, std::string& grappedFileName) = 0;

public:
	int getFrameRate() const;
	void setFrameRate(int frameRate);
	bool isEndOfStream() const;
};
