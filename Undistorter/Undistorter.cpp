#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include "..\ReconstructionUtilities\ProgramArgumentParser.h"
#include "..\ReconstructionUtilities\Server.h"
#include "..\ReconstructionUtilities\Client.h"

class ClientObserver : public util::ICommunicatorObserver
{
	virtual void onDataRecieved(void * data, size_t dataLength) override
	{
		char* dataStr = (char*)data;
		dataStr[dataLength] = 0;
		std::cout << "Incomming data --> " << "Length : " << dataLength << " - Data : " << dataStr << "\n";
	}
	virtual void onClinetConnected() override
	{
	}
};

void showUsage()
{
	std::cout << "Undistorter is a tool used to undistort images coming from the grabber\n";
	std::cout << "Usage : \n";
	std::cout << "Undistorter --output_folder=\"D:\\Images\"\n";
	std::cout << "Options:\n";
	std::cout << "--output_folder : the folder where the undistorted images will be there\n";
}

void doThread()
{
	util::Client client;
	client.addObserver(new ClientObserver());
	client.initialize();
	client.connect("127.0.0.1", "4000");

	Sleep(1000);
	unsigned char data[] = { 254, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 12, 255 };
	client.send(data, sizeof(data));

	while (true)
	{
		Sleep(1);
	}
}

int main(int argc, char* argv[])
{
	//// Check the input argemnts
	//if (argc < 2)
	//{
	//	std::cerr << "Invalid arguments...\n";
	//	showUsage();
	//	return -1;
	//}

	//// Parse the arguments
	//util::ProgramArgumentParser argments;
	//argments.parse(argc, argv);

	//// Read the arguments
	//std::string output_folder = argments.getArgument<std::string>("output_folder");

	// Start the listener 
	

	std::thread th = std::thread(doThread);
	th.join();

	return 0;
}