#include "server.h"
#include "utils.h"

Server *Server::instance = nullptr;

char Server::buffer[Server::BUF_SIZE];

// TODO(roy4801): make create socket more generic
Server::Server(std::string ip, size_t port)
{
	// Create socket
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Failed to create a socket");
		exit(EXIT_FAILURE);
	}
	// Set address
	addr.sin_family = AF_INET;
	if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0)
	{
		perror("Failed to convert address");
		exit(EXIT_FAILURE);
	}
	addr.sin_port = htons(port);
	// Bind
	if(bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("Failed to bind the socket");
		exit(EXIT_FAILURE);
	}
	// Listen
	if(listen(fd, Server::BACKLOG) < 0)
	{
		perror("Failed to listen");
		exit(EXIT_FAILURE);
	}
	//
	printf("Listening on %s:%zu ...\n", ip.c_str(), port);
}

Server::~Server()
{
	close(fd);
}

// Static
bool Server::close_socket(int fd)
{
	if(shutdown(fd, SHUT_RDWR) < 0)
	{
		printf("Failed to shutdown socket\nErr: %d %s\n", errno, strerror(errno));
		return false;
	}
	if(close(fd) < 0)
	{
		printf("Failed to close socket\nErr: %d %s\n", errno, strerror(errno));
		return false;
	}
	return true;
}

// Member
void Server::closeClient(int clfd)
{
	clients.remove(clfd);
	Server::close_socket(clfd);
}

bool Server::acceptConnections()
{
	int nsock;
	int addrlen = sizeof(addr);
	if((nsock = accept(fd, (struct sockaddr *)&addr, (socklen_t*)&addrlen)) < 0)
	{
		perror("Failed to accept connection");
		return false;
	}
	clients.push_back(nsock);
	return true;
}

// TODO(roy4801): handle when siz >= 1024
std::string Server::read(int cl, size_t siz)
{
	char buf[1025] = {0};
	std::string whole;
	int bytes_read;
	//
	bytes_read = recv(cl, buf, 1024, 0);
	whole = buf;
	// int nowRead = 0;
	//
	// do {
	// 	bytes_read = recv(cl, buf, sizeof(buf), 0);
	// 	if (bytes_read > 0)
	// 	{
	// 		whole += std::string(buf);
	// 	}
 //    }
 //    while (bytes_read > 0);
	//
	return whole;
}

int Server::peekBuf(int cl)
{
	char buffer[1024];
	int b = recv(cl, buffer, sizeof(buffer), MSG_PEEK);
	return b;
}

// TODO(roy4801): split the output into chunks
int Server::send(int cl, std::string s)
{
	packi32(Server::buffer, s.size());
	for(int i = 0; i < s.size(); i++)
		Server::buffer[4+i] = s[i];
	return ::send(cl, Server::buffer, s.size()+4, 0);
}

int Server::send(int cl, char *buf, size_t len)
{
	packi32(Server::buffer, len);
	for(int i = 0; i < len; i++)
		Server::buffer[4+i] = buf[i];
	return ::send(cl, Server::buffer, len+4, 0);
}

int Server::sendline(int cl, std::string s)
{
	std::string tmp(s);
	tmp += '\n';
	return send(cl, tmp);
}

// 0 success
int Server::sendFile(int cl, std::string path)
{
	// Check if the file exists
	struct stat res;
	if(stat(path.c_str(), &res) < 0)
	{
		printf("No such file or dictionary: %s", path.c_str());
		return 1;
	}
	// Open the file
	int fileSize = res.st_size;
	std::fstream f(path, std::ios::in | std::ios::binary);
	if(!f.is_open())
	{
		printf("Failed to open: %s\n", path.c_str());
		return 1;
	}
	//
	int times = ceil((double)fileSize / Server::BUF_SIZE);
	printf(">> Split into %d packets\n", times);
	char buf[Server::BUF_SIZE];
	int readSize, remain = fileSize; // read size
	int debug = 0;
	// TODO(roy4801): refactor this
	packi32(buf, times);
	send(cl, buf, 4);
	while(true)
	{
		printf(">> dbg %d\n", debug++);
		// Read Server::BUF_SIZE bytes if it can
		readSize = remain < Server::BUF_SIZE ? remain : Server::BUF_SIZE;
		f.read(buf, readSize);
		// std::cout << f.eof() << ' ' << f.fail() << ' ' << f.bad() << '\n';
		if(f.bad())
		{
			printf("Failed to read: %s\n", path.c_str());
			return 1;
		}
		if(f.eof() || !remain)
			break;
		
		remain -= readSize;
		printf("Remain = %d\n", remain);

		// Send buffer to the client
		if(send(cl, buf, readSize) < 0)
		{
			printf("Failed to send: %s\n", path.c_str());
			printf("%s\n", strerror(errno));
			return 1;
		}
	}
	return 0;
}