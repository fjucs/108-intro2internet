#include <memory>

#include "server.h"
#include "utils.h"

void welcome(std::shared_ptr<Server> s, int cl)
{
	char msg[] =
	" _____            _        _     _   _ _    _ \n"
	"/  ___|          | |      | |   | | | | |  | |\n"
	"\\ `--.  ___   ___| | _____| |_  | |_| | |  | |\n"
	" `--. \\/ _ \\ / __| |/ / _ \\ __| |  _  | |/\\| |\n"
	"/\\__/ / (_) | (__|   <  __/ |_  | | | \\  /\\  /\n"
	"\\____/ \\___/ \\___|_|\\_\\___|\\__| \\_| |_/\\/  \\/ \n";
	char info[] = "Socket Programming hw by roy4801\n406262515 鍾秉桓\nType `help` for the help of commands\n";
	char buffer[4096];
	sprintf(buffer, "%s%s", msg, info);
	size_t siz = strlen(buffer);
	s->send(cl, buffer, siz);
}

void usage(std::shared_ptr<Server> s, int cl)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf),
		"Usage:\n"
		"  ls - list files\n"
		"  d <filename> - download file\n"
		"  quit/exit - bye\n");
	s->send(cl, buf, sizeof(buf));
}

void serverCLIUsage(char *name)
{
	printf("Usage: %s <listen ip> <port>\n", name);
}

// TODO(roy4801): make command logic more flexible.
int main(int argc, char *argv[])
{
	std::string ip;
	int port;
	if(argc == 3)
	{
		ip = argv[1];
		port = strtol(argv[2], nullptr, 10);
	}
	else
	{
		serverCLIUsage(argv[0]);
		exit(EXIT_FAILURE);
	}

	std::shared_ptr<Server> s(Server::getServer(ip, port));

	while(s->acceptConnections())
	{
		int pid = fork();
		if(pid < 0)
			puts("ERROR on fork");
		if(pid == 0) // child
		{
			int ifd = s->getClients().back();
			welcome(s, ifd);

			s->send(ifd, "> ");
			while(true)
			{
				int siz = s->peekBuf(ifd);
				if(siz > 0)
				{
					std::string res = s->read(ifd, 1024);
					std::vector<std::string> v;
					res = rstrip(res, "\n");
					split(v, res);

					printf("Received cmd from [%d] : %s\n", ifd, res.c_str());

					if(v[0] == "ls")
					{
						std::vector<std::string> v;
						list_dir(v, ".");
						std::string data;

						for(auto &i : v)
							data += i + "\n";
						s->send(ifd, data);
					}
					else if(v[0] == "d")
					{
						printf(">> v.size()=%d\n", v.size());
						if(v.size() == 2)
						{
							std::string &file = v[1];
							// if(checkFileExists(file))
							s->sendFile(ifd, file);
						}
						else
						{
							char buf[4];
							packi32(buf, 1);
							s->send(ifd, buf, sizeof(buf));
							s->sendline(ifd, "Usage: d <filename>");
						}
					}
					else if(v[0] == "help")
					{
						usage(s, ifd);
					}
					else if(v[0] == "quit" || v[0] == "exit")
					{
						break;
					}
					else
					{
						s->send(ifd, "Unknown command\n");
					}

					s->send(ifd, "> ");
				}
			}

			s->closeClient(ifd);
			exit(0);
		}
		else if(pid > 0)
		{
			printf("Now clients = %lu\n", s->getClients().size());
		}
	}
}