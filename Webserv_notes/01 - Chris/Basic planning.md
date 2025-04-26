📆 2025-04-26 🕓 13:58
Tags:  

---

1. **Networking Basics**
    - What is **[[HTTP]]** exactly?
    - How do **TCP/IP connections** work? --> [[TCP_IP Sockets]]
    - What is a **socket**, and how does a server accept connections?
2. **HTTP Protocol Handling**
    - Understand **HTTP request format** (headers, method, URL, etc.)
    - Understand **HTTP response format** (status line, headers, body)
    - How to parse incoming HTTP requests correctly and build valid HTTP responses.
3. **Concurrency**
    - How to handle **multiple clients** connecting at the same time?
    - Options: single-threaded, multi-threaded, or event-driven (async)?
4. **File Handling (Static Content Serving)**
    - Read files like HTML, CSS, JS from disk.
    - Correctly set the **Content-Type** headers based on file types.
5. **Security Basics** (even simple ones!)
    - Prevent **path traversal attacks** (e.g., `GET /../../etc/passwd`).
    - Handle bad input gracefully.
6. **Browser Expectations**
    - Browsers can be picky. They expect certain headers and proper formatting.
    - Maybe even small support for 404 Not Found or 500 Internal Server Error.


**First realistic goal:** 
- [ ] Accept connections. 
- [ ] Parse a simple `GET` request.  
- [ ] Serve a basic HTML file ("Hello, world!") back to the browser.


# Creating a Server class

## Unknown header `<netinet/in.h>`

**About `netinet/in.h`:**
This header file provides constants and structures needed for **networking** in C, specifically for handling **IP addresses** and **protocols** (like TCP/UDP). It's a standard part of **POSIX** (used on Linux and other Unix-based systems).

### Key things `netinet/in.h` gives us:

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