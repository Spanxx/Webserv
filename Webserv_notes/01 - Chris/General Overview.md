📆 2025-04-25 🕓 10:50
Tags: 

---

# Basic concepts:
- OSI Model
- Client-server architecture
- IP-Adresses
- Ports
- 7 Steps of a server
- 5 Steps of a Client
- [[TCP_IP Sockets]]
- Setting up the server
- Setting up the client
- Connecting client and server

# OSI Model
Open System Interconnections

7 Layers (Remember: **P**lease **D**o **N**ot **T**hrow **S**alam **P**izza **A**way)
### Highest to lowest
Layer 7: Application (Input Updates/ State Updates)
Layer 6: Presentation (Serialization Buffering)
Layer 5: Session (Sockets)
Layer 4: Transport (TCP/UDP/SPX)
Layer 3: Network (IP)
Layer 2: Data Link (Ethernet (MAC))
Layer 1: Physical (Wired (Cables), Wireless ...)

TCP = Transmisson Control Protocol
UDP = User Datagram Protocol
SPX = Sequenced Packet Exchange

![[OSI-Model.png| 800]]

To implement HTTP, we only care about **4th Layer: Transport Layer**.
The Transport layer is primarily responsible for ensuring that data is transferred from one point to another reliably and without errors. For example, the Transport layer makes sure data are sent and received in the correct sequence.

# Architecture
- Connection is always between 2 devices
- Each device uses it own IP and port Number
- One side Server / One side Client
- Server is continually waiting for incomming connections --> called **listening**
	- it is done on a certain IP and port number

# IP Adresses
- Server and client uses an IP and a port number
	- it is configured during Network setup unless it is allocated dynamically
- A machine with more than one network interface card (NIC) has more then one IP Adress
- Port number of the server is usually specified within the code
- Client port number is allocated by the O/S
- Loopback Adress 127.0.0.1 refers to the current machine
	- can be used during development to test both, client and server on a single machine
	- [ ] how can we test this?

# Ports
- Port number (16 bit adress)
- integer between 1 and 65.535
- Ports 1 ... 1023 are described as well know ports and are reserver for specific applications
	- e.g. Port 21 - FTP
- Recommended to choose a number over 1024

# Sockets
- Def: A pipe between 2 computers on a network through which data flows (Mulholland 2004)

```ad-info
A socket is the mechanism that most popular operating systems provide to give programs access to the network. It allows messages to be sent and received between applications (unrelated processes) on different networked machines.==
```

- Both sides of the connection use a socket
- Sockets are two-way, data can be sent and received on a socket
- 2 Common types for a socket
	- Streaming socket (SOCK_STREAM) TCP
	- Datagram socket (SOCK_DGRAM) UDP

## Create a Socket
- the server creates a new socket
- when created it is yet to be bound to an IP or port number
	- Status Unbound: No Ip Adress / No Port number
	- Status Bound: IP Adress / Port number
- After socket is bound, it is put into the listening state, wating for incomming connections on the port

- Client creates its own socket with IP Adress & Port Number
	- -->status is Bound
- Send Request to connect to Server
- Server accepts the Request
	- Server duplicates itself to connects to the client
	- Original Server stays in listen mode to check for other clients

# 7 Steps of a Server
```ad-attention
All these functions are the windows version. Unix Version will be looked up and added to the file later.
```

1. Initialize WSA - WSAStartup() 
2. Create a socket - socket()
3. Bind the socket - bind()
4. Liste on the socket - listen()
5. Accept a connection - accept(), connect()
6. Send and receive data - recv(), send(), recvfrom(), sendto()
7. Disconnect - closesocket()

# 5 Steps of a Client
1. Initialize WSA - WSAStartup() 
2. Create a socket - socket()
3. Connect to the server - connect()
4. Send and receive data - recv(), send(), recvfrom(), sendto()
5. Disconnect - closesocket()


# Sources
- https://www.youtube.com/watch?v=gntyAFoZp-E&list=PLHpOzseM8O8GY1p0LV0FkJ-_vwzpcQiPv
- https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa