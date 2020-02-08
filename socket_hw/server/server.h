#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
#include <cmath>

#include <vector>
#include <list>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Server
{
public:
	static Server *instance; // singleton
	static Server* getServer(std::string ip, size_t port)
	{
		if(!instance)
			instance = new Server(ip, port);
		return instance;
	}
public:
	Server() = delete;
	Server(std::string ip, size_t port);
	~Server();

	bool recreate_socket(); // TODO
	static bool close_socket(int fd);

	static constexpr int BACKLOG = 10; // Num of waiting in queue
	static constexpr int BUF_SIZE = 4096;
	static char buffer[BUF_SIZE];

	std::list<int> getClients() {return clients;} // BUG: When a socket is closed, the client is still in the list.
	void closeClient(int fd);
private:
	int fd, sock;
	struct sockaddr_in addr;
	// TODO(roy4801): make client class
	std::list<int> clients;
public:
	enum LISTEN_ADDR
	{
		LISTEN_ANY,
		LISTEN_LOCAL,
		LISTEN_SPECIFIC
	};
	std::string read(int cl, size_t siz);
	int peekBuf(int cl);
	int send(int cl, std::string s);
	int send(int cl, char *buf, size_t len);
	int sendline(int cl, std::string s);
	int sendFile(int cl, std::string path);
	bool acceptConnections();
};