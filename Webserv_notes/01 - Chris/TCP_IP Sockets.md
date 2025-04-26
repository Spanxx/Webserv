📆 2025-04-25 🕓 17:08
Tags: #socket #tcp #ip

---
# Programming with TCP/IP sockets

1. Create the socket
2. Identify the socket
3. On the server, wait for an incoming connection
4. Send and receive messages
5. Close the socket

# 1. Create the socket
- a socket, `server_fd`, is created with the *socket* system call

`int server_fd = socket(domain, type, protocol);`

**Good to know:**
All parameters, as well as the return value are integers
#### domain, or adress family
communication domain in which the socket should be created. 
Some of address families are:

```
AF_INET (IP)
AF_INET6 (IPv6)
AF_UNIX (local channel, similar to pipes)
AF_ISO (ISO protocols)
AF_NS (Xerox Network Systems protocols)
```

Adress families determine what variant of the `sockaddr` (Step 2) struct to use and what elements it contains for that specific communication type.
#### type (type of service)
- is selected according to the properties requiered by the application
- `SOCK_STREAM (virtaul circuit service)`
- `SOCK_DGRAM (datagram service)`
- `SOCK_RAW (direct IP service)`
- check with address family to see which services are available

#### protocol
- indicate a specific protocol to use in supporting the sockets operation
- useful when famiilies (adress familiy) have more than one protocol to support a given type of service
- return value is a file descriptor (small int)

For TCP/IP sockets, we want to specifiy the IP adress family (`AF_INET`) and virtual circuit service (`SOCK_STREAM`)
- only one form of virtual circuit service = no variations of the protocol = last argument (protocol) is 0

```cpp title: "example for creating a socket"
#include <sys/socket.h>
...
if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
std::cerr << "Error: can not create socket!\n";
return (0);
}
```
# 2. Identify (name) the socket

- naming = assigning a transport address to it (port number and IP Address)
	- is called binding
- `bind` system call is used for it
- sockets were designed to work with various different types of communication interfaces --> interface of the socket is very general
	- it doesn't take a port number, it takes a `sockaddr` structure whose format is determined on the address family (type of network)
- in UNIX domain sockets, *bind* creates a file in the file system

system call for `bind` is

```cpp
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
```
#### Parameter
**socket** = socket what was created in step 1
**sockaddr** = container that allows the OS to read the first couple bytes to identify the address family

For IP networking, we use `struct sockaddr_in`, which is defined in the header `netinet/in.h`

```title:"sockaddr_in struct"
struct sockaddr_in  
{  
	__uint8_t sin_len;  
	sa_family_t sin_family;  
	in_port_t sin_port;  
	struct in_addr sin_addr;  
	char sin_zero[8];  
};
```

❗Before calling *bind*, we need to fill out this structure. The three key parts we need to set are:

**sin_family**
- address family what we used to set up the socket (AF_INET)
**sin_port**
- port number (transport address)
- we can assign it or leave the O/S assigning it
	- client (usually let the O/S pick any available port number by specifying port 0)
	- server (generally pick a specific port number --> clients need to know for connecting)
**sin_addr**
- your machines IP address
- to let the O/S use whatever it wants use --> 0.0.0.0 it is defined by the symbolic constant `INADDR_ANY`

**address_len** = address structure may differ based on the type of transport
We can simply use`sizeof(struct sockaddr_in)`

```cpp title:"example to bind a socket"
#include <sys/socket.h>
...
struct sockaddr_in address;
const int PORT = 8080; //where the clients can reach it

/* htonl converts a long integer (e.g. address) to a network representation */ 
/* htons converts a short integer (e.g. port) to a network representation */
memset((char *)&address, 0, sizeof(address));  
address.sin_family = AF_INET;  
address.sin_addr.s_addr = htonl(INADDR_ANY);  
address.sin_port = htons(PORT);

if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
{
	std::cerr << "ERROR: bind failed!";
	return (0);
}
```














## Sources:
- https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa