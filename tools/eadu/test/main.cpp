#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
//#include <fcntl.h>
#include <sys/types.h>
//#include <sys/syscall.h>
//#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

/*void sigpipeHandler(int signum, siginfo_t *info, void *ucontext)
{
	
}*/

int tcpSocket;

int main(int argc, char * argv[])
{
	tcpSocket = socket(AF_INET, SOCK_STREAM, 0); //create ipv4 tcp socket
	if (tcpSocket == -1)
	{
		std::cout << "socket creation error: " << errno << "\n";
		return -1;
	}
	else std::cout << "socket " << tcpSocket << " created\n";
	/*int id = getpid();
	std::cout << "process id: " << id << "\n";
	id = syscall(SYS_gettid);
	std::cout << "thread id: " << id << "\n";
	f_owner_ex owner;
	owner.type = F_OWNER_PID;
	owner.pid = id;
	if (fcntl(tcpSocket, F_SETOWN_EX, &owner)) std::cout << "fcntl F_SETOWN_EX error: " << errno << "\n"; //direct io signals to actual process id
	else std::cout << "fcntl F_SETOWN_EX success\n";
	if (fcntl(tcpSocket, F_SETFL, O_ASYNC)) std::cout << "fcntl F_SETFL O_ASYNC error: " << errno << "\n"; //set O_ASYNC status flag
	else std::cout << "fcntl F_SETFL O_ASYNC success\n";
	if (fcntl(tcpSocket, F_SETSIG, SIGPIPE)) std::cout << "fcntl F_SETSIG SIGPIPE error: " << errno << "\n"; //F_SETSIG enable signal for file descriptor may not be necessary
	else std::cout << "fcntl F_SETSIG SIGPIPE success\n";
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	if (sigemptyset(&(act.sa_mask))) std::cout << "sigemptyset error: " << errno << "\n"; //empty signal set, catch error
	act.sa_sigaction = sigpipeHandler;
	if (sigaction(SIGPIPE, &act, 0)) std::cout << "custom signal handler sigaction error: " << errno << "\n"; //set custom signal handler with sigaction(2)
	else std::cout << "custom signal handler sigaction success\n";*/
	in_addr addr;
	if (!inet_aton("192.168.0.110", &addr))
	{
		std::cout << "inet_aton invalid address\n";
		return -1;
	}
	else std::cout << "successfully converted ipv4 address to network byte\n";
	sockaddr_in saddr = {AF_INET, htons(80), addr};
	if (connect(tcpSocket, (sockaddr*)&saddr, sizeof(saddr)))
	{
		std::cout << "connection failed error: " << errno << "\n";
		return -1;
	}
	else std::cout << "successfully connected\n";
	const char msg[] = "hello world";
	if (write(tcpSocket, msg, std::char_traits<char>::length(msg)) == -1)
	{
		std::cout << "write error: " << errno << "\n";
		return -1;
	}
	else std::cout << "write successfull\n";
	char buffer[20];
	if (read(tcpSocket, buffer, 2000) == -1)
	{
		std::cout << "read error: " << errno << "\n";
		return -1;
	}
	else
	{
		std::cout << "read success, data received: \n" << buffer;
	}
	if (tcpSocket != -1) close(tcpSocket);
	std::cout << "end of code\n";
	return 0;
}