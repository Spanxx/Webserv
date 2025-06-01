#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"

int	Server::createServerSocket(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		throw ServerException("Creating server socket failed!");
	}

	//prepare server adress structure
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;				//IPv4
	serverAddr.sin_addr.s_addr = INADDR_ANY;		//bind to any local adress
	serverAddr.sin_port = htons(port);				//port in network byte order

	int yes = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));	// reuse port

	if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		throw ServerException("Bind failed!");
		// std::cerr << "Error binding server socket!\n";
		// exit(1);
	}
	return (sock);
}