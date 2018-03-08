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
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define port 1025

const char firmwareUploadCmd('F');
const char pageUploadCmd('P');
const char continueCmd('C');
const char endCmd('E');
const char verifyCmd('V');
const unsigned short bufferSize(256);

int tcpSocket; //store tcp socket file descriptor
in_addr ipAddr; //store ipv4 address in network nyte order (MSB)
int firmware; //firmware file decriptor
sockaddr_in saddr; //socket address
char buf[bufferSize]; //multi usage buffer

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
	if (argc != 3) //catch invalid number of arguments
	{
	    displayUsage();
	    throw("\nInvalid number of arguments");
	}
	if (!inet_aton(argv[1], &ipAddr)) //validate ip address
	{
	    displayUsage();
	    throw("\nInvalid ipv4 address");
	}
	firmware = open(argv[2], O_RDONLY); //try to open firmware file
	if (firmware == -1) //if firmware file could not be opened
	{
	    displayUsage();
	    throw("\nUnable to open firmware file for reading");
	}
	tcpSocket = socket(AF_INET, SOCK_STREAM, 0); //try to create a tcp socket
	if (tcpSocket == -1) throw("Failed to create a socket file descriptor"); //catch errors
	saddr = {AF_INET, htons(port), ipAddr};
	if (connect(tcpSocket, (sockaddr *)&saddr, sizeof(saddr)) == -1) throw("Failed to connect to Arduino Due"); //try to connect to arduino server, catch errors
	if (write(tcpSocket, &firmwareUploadCmd, 1) < 1) throw("Failed to send upload firmware command"); //try to send upload command
	if (read(tcpSocket, buf, 1) < 1) throw("Failed to receive firmware upload acknownledgement from Arduino Due"); //check for ack
	if (buf[0] != firmwareUploadCmd) throw("Start upload command refused");
	int dataRead = 0; //stores data read command result
	do
	{
	    dataRead = read(firmware, buf, bufferSize); //try to read data from file
	    if (dataRead == -1) throw("Failed to read firmware data"); //read error
	    else if (dataRead == 0) break; //no more data, exit loop
	    if (write(tcpSocket, &pageUploadCmd, 1) < 1) throw("Failed to send page upload command"); //try to send page upload command
	    if (write(tcpSocket, &bufferSize, 2) < 2) throw("Failed to send page upload size"); //try to send page size
	    if (write(tcpSocket, buf, dataRead) < dataRead) throw("Failed to send firmware data"); //try to send data read
	    if (read(tcpSocket, buf, 1) < 1) throw("Failed to receive continue command after page transfer"); //wait signal from arduino to continue upload
	    if (buf[0] != continueCmd) throw("Page transfer aborted"); //catch wrong continue cmd
	}
	while (dataRead >= 0); //loop until there is no data to read in file
	if (write(tcpSocket, &endCmd, 1) < 1) throw("Failed to send end upload command"); //send end command	
	if (read(tcpSocket, buf, 1) < 1) throw("Firmware upload not ended sucessfully");
	if (buf[0] != endCmd) throw("Firmware upload not ended successfully");
	
	
	if (close(firmware) == -1) throw("Failed to close firmware file"); //try to close file
	if (close(tcpSocket) == -1) throw("Failed to close socket"); //try to close socket
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
