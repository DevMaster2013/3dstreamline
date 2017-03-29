#include <iostream>
#include <iomanip>
#include <string>
#include "..\ReconstructionUtilities\ProgramArgumentParser.h"
#include "..\ReconstructionUtilities\Server.h"
#include "ImageFolderSource.h"

class ConnectionObserver : public util::ICommunicatorObserver
{
	virtual void onDataRecieved(void * data, size_t dataLength) override
	{
	}
	virtual void onClinetConnected(const char * clientName) override
	{
		std::cout << "Client: " << clientName << " Connected\n";
	}
};

void showUsage()
{
	std::cout << "Recorder is a tool used to grab images from different sources\n";
	std::cout << "Usage : \n";
	std::cout << "Recorder --source_type=1 --input_folder=\"C:\\Images\" --frame_rate=4 --output_folder=\"D:\\Images\"\n";
	std::cout << "Options:\n";
	std::cout << "--source_type : is the type of image source it contain only one value now (1 : image folder)\n";
	std::cout << "--input_folder : the input folder of the source image\n";
	std::cout << "--frame_rate : the frame rate where to grab images\n";
	std::cout << "--output_folder : the folder where the grapped image will be there\n";
}

int main(int argc, char* argv[])
{
	// Check the input argemnts
	if (argc < 5)
	{
		std::cerr << "Invalid arguments...\n";
		showUsage();
		return -1;
	}

	// Parse the arguments
	util::ProgramArgumentParser argments;
	argments.parse(argc, argv);

	// Read the arguments
	int source_type = argments.getArgument<int>("source_type");
	std::string input_folder = argments.getArgument<std::string>("input_folder");
	int frame_rate = argments.getArgument<int>("frame_rate");
	std::string output_folder = argments.getArgument<std::string>("output_folder");

	// Declare the source grabber
	ImageSource* source = nullptr;
	if (source_type == 1)	// Folder Source
	{
		source = new ImageFolderSource(input_folder, frame_rate);
	}

	// the jpeg compression parameters
	std::vector<int> compressionParameters;
	compressionParameters.push_back(CV_IMWRITE_JPEG_QUALITY);
	compressionParameters.push_back(100);

	// Start the sending server
	util::Server server;
	server.addObserver(new ConnectionObserver());
	server.connect("", "4000");

	// Starting grap the images
	while (!source->isEndOfStream())
	{
		// Get the image file name
		std::string imageFileName = "";

		// Grab the image
		cv::Mat image;
		source->grabImage(image, imageFileName);

		// Save the image in the output folder
		std::string fullPath = output_folder + "\\" + imageFileName;
		cv::imwrite(fullPath, image, compressionParameters);

		// Log the written image
		std::cout << "Image : " << std::setw(16) << imageFileName << " is grabbed\n";

		// Send the image name to the clients
		server.send((void*)(fullPath.c_str()), fullPath.length());
	}

	// Delete the grabber
	delete source;
}