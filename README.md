## Webserv project at 42 school
Contributors: Chris (@Spanxx), Henriette (@Henrizz) and Stephanie (@StephVibes)
<br>
<br>
## Project description
The objective of this project is to learn the rules and basics of HTTP and client-server communication. We are asked to build a server that can host various websites with individual databases and specifications determined in a configuration file (similar to NGINX). A special focus is on keeping the server non-blocking at all times, which we tested with siege, to ensure 100% availability even with many simultaneous requests. 
<br>
We handle the following aspects: 
- regular and chunked requests (a feature often used for streaming)
- GET, POST and DELETE methods
- login feature with user and cookie management 
- CGI scripts (using python and php)
- upload and deletion of files 
- html and css
- automatically updated image gallery with javascript
- custom error pages with the adequate error codes
- routing per specificied location blocks
- directory listing for directory listing (when no index page is specified in config file and autoindex is on)

## Usage
To run our program, first download the code, open a terminal, go inside the project directory and type: 'make'
Then type './webserv server.conf' in the terminal to launch the server with our default configuration. You can change the configuration by adding a different file as program argument or change the server.conf file directly.
You can access our default websites in the browser, by either requesting 'localhost:8080' or 'localhost:9090' (or the alternate host:port combination you would put in your own configuration) and then discover all features.
<br>

**! Plase note that the 'SAY HI' button on our websites is a php script configured for linux !**
<br>
<br>
If you want to use this script on mac, you have to change the php shebang of the hello.php script inside the cgi-bin folder to the location your php-cgi is at (to find out where this is, type 'which php-cgi' in the terminal) 


Here’s a clean, structured `README.md` for the **Webserv** project, aligned with the subject and written in the same style as before:

---

# TL:DR

## 📌 Summary

Webserv is a **C++ (98) project** where you implement your own **HTTP server from scratch**. The goal is to understand how web servers work at a low level, including **network communication, request parsing, and response generation**.

The server must:

* Handle real HTTP requests from browsers
* Serve static websites
* Support basic HTTP methods (GET, POST, DELETE)
* Operate efficiently using **non-blocking I/O**

This project focuses heavily on **network programming**, **event-driven architecture**, and **protocol compliance**.

---

## 🧠 Key Concepts

### 1. HTTP Protocol

HTTP (Hypertext Transfer Protocol) is the foundation of web communication.

* Client (browser) sends a request
* Server responds with:

  * Status code (e.g., 200, 404)
  * Headers
  * Body (HTML, files, etc.)

👉 Key idea: *Request → Processing → Response*

---

### 2. TCP Sockets

The server communicates over **TCP sockets**.

* `socket()` → create endpoint
* `bind()` → assign IP/port
* `listen()` → wait for connections
* `accept()` → handle clients

👉 Key idea: *Low-level network communication*

---

### 3. Non-blocking I/O

The server must **never block**.

* Use `poll()` (or `select`, `epoll`, `kqueue`)
* Handle multiple clients simultaneously
* Only read/write when the socket is ready

👉 Key idea: *Event-driven concurrency (single-threaded but scalable)*

---

### 4. Event Loop

Core architecture of the server:

* Monitor all file descriptors
* React to events (read/write)
* Process requests incrementally

👉 Key idea: *Efficient handling of many clients without threads*

---

### 5. HTTP Request Parsing

We had to parse raw HTTP requests:

Example:

```
GET /index.html HTTP/1.1
Host: example.com
```

* Extract:

  * Method (GET, POST, DELETE)
  * Path
  * Headers
  * Body

👉 Key idea: *Transform raw text into structured data*

---

### 6. HTTP Response Generation

The server must construct valid responses:

Example:

```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 123

<html>...</html>
```

* Correct status codes are critical
* Must handle errors (404, 500, etc.)

👉 Key idea: *Strict protocol compliance*

---

### 7. Configuration File

The server behavior is defined via a config file (NGINX-like).

It controls:

* Ports and hosts
* Routes (URL → directory)
* Allowed HTTP methods
* Default files (index.html)
* Error pages
* Upload paths
* Redirections

👉 Key idea: *Separation of configuration and logic*

---

### 8. Static File Serving

Serve files from disk:

* Map URL → filesystem path
* Read file
* Send content to client

👉 Key idea: *Basic web hosting*

---

### 9. CGI (Common Gateway Interface)

Allows execution of external programs (e.g., PHP, Python).

* Server passes request data via environment variables
* Executes script using `fork` + `execve`
* Sends output back to client

👉 Key idea: *Dynamic content generation*

---

### 10. File Upload Handling

Support POST requests with file uploads:

* Parse request body
* Save file to configured directory

👉 Key idea: *Handling client data input*

---

### 11. Error Handling & Resilience

The server must:

* Never crash
* Handle malformed requests
* Provide default error pages

👉 Key idea: *Robustness under all conditions*

---

### 12. Multi-port & Multi-server Support

The server can:

* Listen on multiple ports
* Serve different content per configuration

👉 Key idea: *Flexible server architecture*

---

## 📂 Project Structure (Example)

```id="n5kz2o"
.
├── Makefile
├── includes/
├── srcs/
├── config/
│   └── default.conf
└── www/
    └── index.html
```

---

## ⚙️ Requirements Overview

* Language: **C++98**
* No external libraries
* Must use:

  * `poll()` or equivalent
* Must support:

  * GET, POST, DELETE
  * Static files
  * File uploads
  * CGI execution
* Must be:

  * Non-blocking
  * Stable (no crashes)
  * Browser-compatible

---

## 🎯 Bonus (Optional)

* Cookie handling
* Session management
* Multiple CGI support

---

## 🚀 Conclusion

Webserv is essentially a **mini NGINX/Apache built from scratch**.

It forces you to understand:

* How the web actually works under the hood
* How servers handle concurrency efficiently
* How protocols are implemented in real systems

This project is less about features and more about **correct architecture, robustness, and protocol accuracy**.
