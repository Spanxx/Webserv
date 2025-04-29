
- guide: https://hackmd.io/@laian/SJZHcOsmT
- server receives data/requests from client, processes them and sends response
- HTTP protocol is set of rules for the communication : https://datatracker.ietf.org/doc/html/rfc2616
- main parts of request/response: header + body
- header: info on request / response --> context information 
	- starts with request-line ( specifies request method, request URI and HTTP version), example ```GET /index.html HTTP/1.1```
- body: content of request/ response (can be empty) 
	- starts and ends with ```\r\n\r\n```
	- response line (specifies HTTP version, status code, status message)

- connection between clients and servers are facilitated by sockets, which are the communication link between two processes on a network.
- A socket is just a file descriptor, created using the function:```int socket(int domain, int type, int protocol);```


setup: 
1. make connection with sockets (int socket (.....))
2. bind socket descriptor to a port on the computer (int bind(.....))
3. check out byte orders and possibly convert? with htons/htonl
4. configure socket (int setsocket(.....))
5. reset ports so we can reuse them 
6. accept incoming client connections (int client_fd = accept(.....))
7. make socket non-blocking (fcntl(...))
8. use poll or select to monitor sockets
9. read from the socket
10. parse the http request 
11. build the http response
12. send response to the client 
13. close the client sock
14. clean up and repeat 


Problems to avoid: 
- **blocking**: trying to read from a socket with no data going into it (halts a server --> solution: polling --> lets systems know which sockets have data ready to perform operations on them)
- **chunking**: when large amount of data has to be sent (server will freeze --> solution: read/parse only specified lenghts of data at a time, define a certain buffer size), chunking can be applied in large responses to GET requests and POST requests with large body 

**what is:

- request methods (a.o.):
	- GET: When a client requests a piece of information such as a webpage from the server
	- POST: When a client sends a piece of information (files) to the server
	- DELETE: When a client requests the server to delete the specified resource.
- request URI: The **Uniform Resource Identifier** in the request line (e.g., `/index.html`). It tells the server which resource is being requested.
- TCP: – Transmission Control Protocol: - A **connection-oriented**, **reliable** transport protocol. Ensures data is delivered **in order**, **without duplication** or **loss**. Used by HTTP to send requests and receive responses between client and server.
- nginx: server configuration format. uses a declarative configuration file typically found at `/etc/nginx/nginx.conf`. Can define virtual hosts, reverse proxy settings, security rules, and more.




Here's a good initial breakdown of responsibilities and tasks for **Phase 1 (MVP): basic HTTP server with GET support)**:

---

## 🔧 Person 1: **Networking & Connections**

**Focus:** Handle the **low-level socket setup** and the **connection loop**.

### Tasks:

- Set up the socket (`socket`, `bind`, `listen`, `accept`)
    
- Handle client connections (blocking mode for MVP)
    
- Read HTTP requests and pass them to a parser
    
- Send simple HTTP responses (200 OK with static HTML)
    
- Implement graceful socket shutdown and cleanup
    
- (Optional MVP+) Start adding non-blocking or `poll` setup
    

**Deliverable:** A running server that can accept connections and return static HTML on GET requests.

---

## 📬 Person 2: **Request Parsing & Response Formatting**

**Focus:** Understand and parse incoming HTTP requests and create correct responses.

### Tasks:

- Parse request line (`GET /index.html HTTP/1.1`)
    
- Parse headers (e.g. Host, Content-Length)
    
- Detect valid/invalid requests (e.g. 400, 404, 405)
    
- Build correct HTTP response headers
    
- Work with Person 3 to load correct files from disk
    

**Deliverable:** A parser that can handle basic HTTP 1.1 GET requests and generate proper HTTP responses.

---

## 📁 Person 3: **File Serving & Configuration**

**Focus:** Serve files from disk and load basic configuration.

### Tasks:

- Load requested file from disk (`open`, `read`, `stat`)
    
- Return 404 or 403 errors appropriately
    
- Determine MIME types for `.html`, `.jpg`, `.css`, etc.
    
- Load and parse basic config file (e.g. which port, root directory)
    
- Provide config data to Person 1
    

**Deliverable:** A file handler that can respond to a GET request with the right file contents or error code.

---

## 🔁 Integration Plan

- Sync up after 1–2 days with a dummy request format (e.g. `"GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"`)
    
- Make sure Person 1 → Person 2 → Person 3 have clear interfaces (e.g. `RequestParser::parse()`, `FileServer::getFile()`).
    
- MVP goal: A browser can request `http://localhost:PORT/` and see a working HTML page.
    

---

## After MVP Is Done

You can then expand into:

- POST requests
    
- Configuration file parsing (multiple servers, error pages)
    
- CGI execution
    
- Chunked transfer encoding
    
- Multiplexing (`poll`, `select`)
    
- Non-blocking mode
    
- Logging, testing, and error handling