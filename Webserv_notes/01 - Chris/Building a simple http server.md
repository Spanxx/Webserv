📆 2025-04-26 🕓 17:18
Tags: 

---

# 1.  Creating a Server class

**Unknown header `<netinet/in.h>`**

**About `netinet/in.h`:**
This header file provides constants and structures needed for **networking** in C, specifically for handling **IP addresses** and **protocols** (like TCP/UDP). It's a standard part of **POSIX** (used on Linux and other Unix-based systems).

### Key things `netinet/in.h` gives us:
--> look here: [[TCP_IP Sockets]]

1. **`struct sockaddr_in`**  
    This is a structure that we use to specify an **IP address** and **port** for both the server and client. It's crucial for binding the server socket to an IP/port and for handling incoming connections.
    
2. **Constants for IP Protocol**  
    Constants like `AF_INET` (for IPv4) and `SOCK_STREAM` (for TCP connections) are defined in this header.
    
3. **Functionality for Network Operations**  
    This header is part of what enables us to:
    - **Bind** a socket to an address/port
    - **Listen** for incoming connections
    - **Accept** connections
    - **Send/receive** data over the network


# Copy and assignment constructor

Sockets (file descriptors) **are resources** managed by the operating system.  
You **should not simply copy** the `_serverSocket` file descriptor when copying the object.  
Otherwise, two objects would close the same socket — **bad!**

👉 **Correct behavior:**

- The copy constructor and assignment operator should either:
    - **Not allow copying** (best for now — we can disable them).
    - **Or** create a new socket and bind again (complicated and weird for a Server).

✅ **Best solution for now (simple and safe):**
- **Disable** the copy constructor and assignment operator by making them private and not implementing them.


