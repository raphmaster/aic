/*
Name:Enhanced Arduino Due Uploader
Synopsis: eadu ip file
Description: Upload firmware file to an arduino due running enhanced bootloader or firmware at the specified ip.
Arguments:
ip: ipv4 address of arduino due to which we will transfer firmware file
file: path to firmware file to transfer to arduino due
Exit status: 0->success, -1->error

Notes:

TODO:
catch errors
open firmware
create socket
connect
write to arduino to say that we are sending firmware
wait for answer from arduino
send data (page per page or as a whole)
verify data sent
say to arduino that firmware transfer is done
close socket
*/

#include <iostream>
#include <sys/socket.h>

int tcpSocket; //store tcp socket file descriptor

/*
Display usage and description
*/
void displayUsage()
{
	std::cout << "Name:\n\n\tEnhanced Arduino Due Uploader\n\nSynopsis:\n\n\teadu ip file\n\nDescription:\n\n\tUpload firmware file to an Arduino Due running enhanced bootloader or firmware at the specified ip.\n\nArguments:\n\n\tip: ipv4 address of arduino due\n\tfile: path to firmware file\n\nExit Status:\n\n\t0: upload successfull\n\t-1: upload error\n";
}

int main(int argc, char *argv[])
{
	try
	{
		if (argc < 3) //catch invalid number of arguments
		{
			displayUsage();
			return -1;
		}
		//recover ip argument and check for correctness
		//open firmware, will catch invalid file path
		tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (tcpSocket == -1) throw("Failed to create a socket file descriptor");
		
	}
	catch(const char *e)
	{
		std::cout << e << "\n" << "Error code: " << errno << "\n"; //display error message
		return -1; //return error code
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << "\n"; //display error message
		return -1; //return error code
	}
	return 0;
}