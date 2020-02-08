#include <iostream>
#include <string>

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <mutex>

#define PORT 8080
int sock = 0;
struct sockaddr_in serv_addr;

char buffer[4096];
std::string opt;

long int unpacki32(char *buf)
{
    unsigned long int i2 = ((unsigned long int)buf[0]<<24) |
                           ((unsigned long int)buf[1]<<16) |
                           ((unsigned long int)buf[2]<<8)  |
                           buf[3];
    long int i;

    // change unsigned numbers to signed
    if (i2 <= 0x7fffffffu) { i = i2; }
    else { i = -1 - (long int)(0xffffffffu - i2); }

    return i;
}

int recvive()
{
	int bCnt;
	if(recv(sock, buffer, 4, 0) > 0)
	{
		bCnt = unpacki32(buffer);
		
		#if DEBUG
		printf("bCnt=%d\n", bCnt);
		#endif
	}
	else
		return 0;
	return recv(sock, buffer, bCnt, 0);
}

void recvAndPrint()
{
	#if DEBUG
	puts("BEFORE");
	#endif
	
	int rcnt;
	if((rcnt = recvive()) > 0)
	{
		#if DEBUG
		printf("@%d@\n", rcnt);
		printf("[");
		#endif
		
		for(int i = 0; i < rcnt; i++)
			putchar(buffer[i]);
		
		#if DEBUG
		printf("]\n");
		#endif
	}
	
	#if DEBUG
	puts("AFTER");
	#endif
}

int main(int argc, char const *argv[])
{
	// Create a socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	   
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}
   
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}

	recvAndPrint(); // banner
ICMD:
	recvAndPrint(); // >
INPUT:
	getline(std::cin, opt);
	if(opt == "")
	{
		printf("> ");
		goto INPUT;
	}
	send(sock, opt.c_str(), opt.size()+1, 0);

	if(opt == "ls" || opt == "help")
	{
		recvAndPrint();
	}
	else if(opt[0] == 'd')
	{
		recvive();
		int cnt = unpacki32(buffer);

		#if  DEBUG
		printf("cnt=%d\n", cnt);
		#endif
		
		for(int i = 0; i < cnt; i++)
			recvAndPrint();
	}

	if(opt != "quit" && opt != "exit")
		goto ICMD;
	send(sock, "quit", 4, 0);

	shutdown(sock, SHUT_RDWR);
	::close(sock);
	exit(EXIT_SUCCESS);
}