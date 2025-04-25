#include <sys/socket.h>

...
if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
	std::cerr << "Error: can not create socket!\n";
	return (0);
}