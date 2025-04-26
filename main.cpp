
#include "incl/server.hpp" 

int main()
{
	// browser: 127.0.0.1:555
	Server newServer(5555);
	newServer.start();
	
	return (0);
}