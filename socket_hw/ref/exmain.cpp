#include "server.h"

char buffer[1024];
const char *hello = "Hello from server";

#define PORT 8080
int main(int argc, char const *argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	   
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	// Set address
	address.sin_family = AF_INET;
	if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
	{
		perror("Failed to convert address");
		exit(EXIT_FAILURE);
	}
	// address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
	address.sin_port = htons( PORT );
	   
	puts("bind");
	if (bind(server_fd, (struct sockaddr *)&address,
								 sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	puts("listen");
	if (listen(server_fd, 10) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	puts("accept");
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
					   (socklen_t*)&addrlen))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	puts("While");
	while(true)
	{
		valread = read( new_socket , buffer, 1024);
		printf("%s\n", buffer);
		send(new_socket, hello, strlen(hello), 0);
	}
	return 0;
}