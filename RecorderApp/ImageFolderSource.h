#pragma once

#include "ImageSource.h"
#include "..\ReconstructionUtilities\Stopwatch.h"
#include <vector>
#include <string>

class ImageFolderSource : public ImageSource
{
private:
	std::vector<std::string> _folderImages;
	int _currentImageFrame;

public:
	ImageFolderSource();
	ImageFolderSource(const std::string& input_folder, int frameRate);
	~ImageFolderSource();

public:
	virtual void grabImage(cv::Mat& image, std::string& grappedFileName) override;

private:
	void populateFolderImages();
};
