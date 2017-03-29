#pragma once

#include <opencv2\opencv.hpp>
class ImageSource
{
protected:
	std::string _calibrationFile;
	std::string _outputFolder;
	std::string _inputName;
	int _frameRate;
	bool _endOfStream;
	int _sourceType;

public:
	ImageSource(int sourceType);
	ImageSource(int sourceType, const std::string& inputName, int frameRate);
	virtual ~ImageSource();

public:
	virtual void grabImage(cv::Mat& image, std::string& grappedFileName) = 0;

public:
	static ImageSource* createImageSource(int sourceType);
	static const char* getImageSourceName(int sourceType);

public:
	int getFrameRate() const;
	void setFrameRate(int frameRate);
	int getSourceType() const;
	void setSourceType(int sourceType);
	const std::string& getInputName();
	void setInputName(const std::string& inputName);
	const std::string& getCalibrationFile();
	void setCalibrationFile(const std::string& calibFile);
	const std::string& getOutputFolder();
	void setOutputFolder(const std::string& outFolder);
	bool isEndOfStream() const;
};
