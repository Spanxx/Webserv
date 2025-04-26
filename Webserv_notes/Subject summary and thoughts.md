📆 2025-04-26 🕓 10:38
Tags: #subject #todo 

---

```ad-important
Any external library and Boost libraries are forbidden
```
# 🧠 Thoughts 

**Before starting this project**
- [ ] read the RFC
- Version from 1999 (obsolet)
	- [RFC 2616](https://www.rfc-editor.org/info/rfc2616),  [RFC 2617](https://www.rfc-editor.org/info/rfc2617)
- Current version
	- [RFC 9110](https://www.rfc-editor.org/info/rfc9110) Semantics, Content and Authentication
	- [RFC 9111](https://www.rfc-editor.org/info/rfc9111) Caching
	- [RFC 9112](https://www.rfc-editor.org/info/rfc9112) Message Syntax and Routing
	
	- [RFC 7230](https://www.rfc-editor.org/info/rfc7230) & [RFC 7231](https://www.rfc-editor.org/info/rfc7231) are older version, but easier to understand.
	- ✅ We should use **RFC 9110** and **9112** as our main reference.  
		- (And occasionally check old 7230 examples for easier explanations.)

- [ ]  telnet and NGINX
	--> [[Testing Telnet & NGINX]]
- [ ] what is a CGI?

# Requirements

- [ ] take config file
- [ ] cannot execve another web server
- [ ] remain non-blocking at all times / handle client disconnect
- [ ] non-blocking &  onlly 1 poll() (or equivalent) for all I/O (listen included)
- [ ] poll() must monitor read and write simultaneously
- [ ] NO read or write without poll()
- [ ] Checking `errno` is striclty forbidden after read and write ❗
- [ ] not required to use poll() before reading config file
- [ ] Every macro and define is allowd (like FD_SET, FD_CLR, FD_ISSET and, FD_ZERO)
	- [ ] how do they work?
- [ ] request should never hang indefinitely
- [ ] compatible with standard web browser of our choice
- [ ] consider that NGINX is HTTP 1.1 compliant and may be used to compare headers and answer behaviors ❓
- [ ] accurate HTTP response status code
- [ ] default error pages (if not provided)
- [ ] can't use fork for anything other than CGI (like PHP, Python...)
- [ ] serve a fully static website
- [ ] able to upload files
- [ ] Methods: GET, POST and DELETE
- [ ] Stress test successfull
- [ ] able to listen to multiple ports (see config file)
# Executable Command

#### `./webserv [configuration file`]

# Configuration file

in file, you should be able to:
- [ ] choose port and host
- [ ] set up server name
- [ ] first server for host:port = default (responds to all requests)
- [ ] default error pages
- [ ] max size for client request bodies
- [ ] set up routes with one or multiple rules/configs (no regexp)
	- [ ] list of accepted HTTP methods for route
	- [ ] HTTP redirect
	- [ ] directory or file where requested file shold be located
		(e.g. if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is /tmp/www/pouic/toto/pouet)
	- [ ] enable or disable directory listening
	- [ ] set default file to server if request is directory
	- [ ] execute CGI based on file extension (e.g. .php)
	- [ ] works with POST and GET
	- [ ] allow file upload and configure where they are saved
- [ ] CGI
	- [ ] use full path as PATH_INFO
	- [ ] chunked requests needed to be unchunked / CGI expects EOF at end of body
	- [ ] same for output, if no content length is returned from CGI = EOF will mark the end
	- [ ] programm should call CGI with requested file as first argument
	- [ ] CGI should run in correct directory for relative path file access
	- [ ] server should support at least one CGI (php-CGI, Python....)

We have to provide config files and default files for testing.
# Bonus part

- [ ] support cookies and session management (provide simple examples)
- [ ] handle multiple CGI
# External functions overview

| Function       | Header File                                      | Short Description                                                 |
| -------------- | ------------------------------------------------ | ----------------------------------------------------------------- |
| execve         | `<unistd.h>`                                     | Replaces current process image with a new one.                    |
| pipe           | `<unistd.h>`                                     | Creates a unidirectional data channel (pipe).                     |
| strerror       | `<cstring>` (`<string.h>`)                       | Returns string describing an error number.                        |
| gai_strerror   | `<netdb.h>`                                      | Returns string describing a `getaddrinfo` error.                  |
| errno          | `<cerrno>` (`<errno.h>`)                         | Global variable holding last error code.                          |
| dup            | `<unistd.h>`                                     | Duplicates a file descriptor.                                     |
| dup2           | `<unistd.h>`                                     | Duplicates a file descriptor to a specific descriptor.            |
| fork           | `<unistd.h>`                                     | Creates a new child process.                                      |
| socketpair     | `<sys/socket.h>`                                 | Creates a pair of connected sockets.                              |
| htons          | `<arpa/inet.h>`                                  | Converts short integer to network byte order.                     |
| htonl          | `<arpa/inet.h>`                                  | Converts long integer to network byte order.                      |
| ntohs          | `<arpa/inet.h>`                                  | Converts short from network to host byte order.                   |
| ntohl          | `<arpa/inet.h>`                                  | Converts long from network to host byte order.                    |
| select         | `<sys/select.h>` or `<sys/time.h>`, `<unistd.h>` | Monitors multiple file descriptors for readiness.                 |
| poll           | `<poll.h>`                                       | Monitors multiple file descriptors (more scalable than `select`). |
| epoll_create   | `<sys/epoll.h>`                                  | Creates an epoll instance.                                        |
| epoll_ctl      | `<sys/epoll.h>`                                  | Controls an epoll instance (add, remove, modify fds).             |
| epoll_wait     | `<sys/epoll.h>`                                  | Waits for events on the epoll instance.                           |
| kqueue         | `<sys/event.h>`, `<sys/time.h>`                  | Creates a new kernel event queue (BSD).                           |
| kevent         | `<sys/event.h>`, `<sys/time.h>`                  | Registers events with a kqueue (BSD).                             |
| socket         | `<sys/socket.h>`                                 | Creates a socket.                                                 |
| accept         | `<sys/socket.h>`                                 | Accepts a connection on a socket.                                 |
| listen         | `<sys/socket.h>`                                 | Listens for incoming socket connections.                          |
| send           | `<sys/socket.h>`                                 | Sends data through a socket.                                      |
| recv           | `<sys/socket.h>`                                 | Receives data from a socket.                                      |
| chdir          | `<unistd.h>`                                     | Changes the current working directory.                            |
| bind           | `<sys/socket.h>`                                 | Binds a socket to an address/port.                                |
| connect        | `<sys/socket.h>`                                 | Connects a socket to a remote address.                            |
| getaddrinfo    | `<netdb.h>`                                      | Resolves hostname and service to address info.                    |
| freeaddrinfo   | `<netdb.h>`                                      | Frees address info structures.                                    |
| setsockopt     | `<sys/socket.h>`                                 | Sets options on sockets.                                          |
| getsockname    | `<sys/socket.h>`                                 | Gets local address of a socket.                                   |
| getprotobyname | `<netdb.h>`                                      | Gets protocol entry by name.                                      |
| fcntl          | `<fcntl.h>`                                      | Performs operations on file descriptors (non-blocking, etc.).     |
| close          | `<unistd.h>`                                     | Closes a file descriptor.                                         |
| read           | `<unistd.h>`                                     | Reads data from a file descriptor.                                |
| write          | `<unistd.h>`                                     | Writes data to a file descriptor.                                 |
| waitpid        | `<sys/wait.h>`                                   | Waits for a specific child process to change state.               |
| kill           | `<signal.h>`                                     | Sends a signal to a process.                                      |
| signal         | `<signal.h>`                                     | Sets a handler for a signal.                                      |
| access         | `<unistd.h>`                                     | Checks user's permissions for a file.                             |
| stat           | `<sys/stat.h>`                                   | Gets file information (size, permissions, etc.).                  |
| open           | `<fcntl.h>`                                      | Opens a file and returns a file descriptor.                       |
| opendir        | `<dirent.h>`                                     | Opens a directory stream.                                         |
| readdir        | `<dirent.h>`                                     | Reads entries from a directory stream.                            |
| closedir       | `<dirent.h>`                                     | Closes a directory stream.                                        |
