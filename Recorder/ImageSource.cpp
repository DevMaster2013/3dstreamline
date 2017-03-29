#include "ImageSource.h"

ImageSource::ImageSource(int frameRate)
	: _frameRate(frameRate)
	, _endOfStream(false)
{
}

ImageSource::~ImageSource()
{
}

int ImageSource::getFrameRate() const
{
	return _frameRate;
}

void ImageSource::setFrameRate(int frameRate)
{
	_frameRate = frameRate;
}

bool ImageSource::isEndOfStream() const
{
	return _endOfStream;
}
