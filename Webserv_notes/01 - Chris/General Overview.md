📆 2025-04-25 🕓 10:50
Tags: 

---

Basic needed concepts:

- Winsocket
- OSI Model
- Client-server architecture
- IP-Adresses
- Ports
- 7 Steps of a server
- 5 Steps of a Client
- Load DLL
- Create Socket
- Bind Socket
- Listen
- Accept
- Connect
- Setting up the server
- Setting up the client
- Connecting client and server


# Winsock
Sockets were initially inveted for Unix
Winsock is a low level API

- [ ] What is the Linux Version?

# OSI Model
Open System Interconnections

7 Layers (Remember: **P**lease **D**o **N**ot **T**hrow **S**alam **P**izza **A**way)
### Highest to lowest
Layer 7: Application (Input Updates/ State Updates)
Layer 6: Presentation (Serialization Buffering)
Layer 5: Session (Sockets)
Layer 4: Transport (TCP/UDP)
Layer 3: Network (IP)
Layer 2: Data Link (Ethernet (MAC))
Layer 1: Physical (Wired (Cables), Wireless ...)

![[OSI-Model.png| 800]]


# Architecture
- Connection is always between 2 devices
- Each device uses it own IP and port Number
- One side Server / One side Client
- Server is continually waiting for incomming connections --> called **listening**
	- it is done on a certain IP and port number
- 