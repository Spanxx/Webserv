### Process Management

- **`execve(const char *pathname, char *const argv[], char *const envp[])`**
    Replaces the current process image with a new process image specified by `pathname`.  
    **Returns:** On success, does not return; on error, returns `-1` and sets `errno`
    
- **`fork(void)`**  
    Creates a new process by duplicating the calling process.  
    **Returns:** On success, returns the child's PID in the parent, and `0` in the child; on error, returns `-1` and sets `errno`
    
- **`waitpid(pid_t pid, int *status, int options)`**  
    Waits for state changes in a child process.  
    **Returns:** On success, returns the PID of the child; on error, returns `-1` and sets `errno`.
    
- **`kill(pid_t pid, int sig)`**  
    Sends a signal to a process or group of processes.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`
    
- **`signal(int signum, sighandler_t handler)`**  
    Sets a function to handle the specified signal.  
    **Returns:** On success, returns the previous signal handler; on error, returns `SIG_ERR`.​



### File and Directory Operations

- **`open(const char *pathname, int flags, mode_t mode)`**  
    Opens a file and returns a file descriptor.  
    **Returns:** On success, returns a non-negative file descriptor; on error, returns `-1` and sets `errno`.​
    
- **`read(int fd, void *buf, size_t count)`**  
    Reads data from a file descriptor.  
    **Returns:** On success, returns the number of bytes read; on error, returns `-1` and sets `errno`
    
- **`write(int fd, const void *buf, size_t count)`**  
    Writes data to a file descriptor.  
    **Returns:** On success, returns the number of bytes written; on error, returns `-1` and sets `errno`.​
    
- **`close(int fd)`**  
    Closes a file descriptor.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`.​
    
- **`stat(const char *pathname, struct stat *statbuf)`**  
    The **`stat`** function retrieves detailed information about a file or directory specified by the `pathname`. The information is stored in a `struct stat`, which contains a variety of file attributes, such as file type, permissions, timestamps, size, and more.
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`.​
    
- **`access(const char *pathname, int mode)`**  
    Checks a file's accessibility.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`.​
    
- **`opendir(const char *name)`**  
    Opens a directory stream.  
    **Returns:** On success, returns a pointer to the directory stream; on error, returns `NULL` and sets `errno`.​
    
- **`readdir(DIR *dirp)`**  
    Reads a directory entry.  
    **Returns:** On success, returns a pointer to the directory entry; on end of directory or error, returns `NULL`.
    
- **`closedir(DIR *dirp)`**  
    Closes a directory stream.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`

---

### File Descriptor Operations

- **`dup(int oldfd)`**  
    Duplicates a file descriptor.  
    **Returns:** On success, returns a new file descriptor; on error, returns `-1` and sets `errno`.
    
- **`dup2(int oldfd, int newfd)`**  
    Duplicates a file descriptor to a specified descriptor number.  
    **Returns:** On success, returns `newfd`; on error, returns `-1` and sets `errno`.​
    
- **`fcntl(int fd, int cmd, ...)`**  
    Performs operations on file descriptors.  Commonly, **`fcntl`** is used for setting non-blocking mode on sockets or to manipulate file descriptor flags for controlling behavior during input/output operations.
    **Returns:** Varies depending on `cmd`; on error, returns `-1` and sets `errno`.​
    

---

### Pipes and Sockets

- **`pipe(int pipefd[2])`**  
    Creates a unidirectional data channel.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`. ​
    
- **`socketpair(int domain, int type, int protocol, int sv[2])`**  
    Creates a pair of connected sockets.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`
- **`socket(int domain, int type, int protocol)`**  
    Creates an endpoint for communication.  
    **Returns:** On success, returns a file descriptor; on error, returns `-1` and sets `errno`.​
    
- **`bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)`**  
    Assigns an address to a socket.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`.​
    
- **`listen(int sockfd, int backlog)`**  
    Marks a socket as passive to accept incoming connections.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`.​
    
- **`accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)`**  
    Accepts a connection on a socket.  
    **Returns:** On success, returns a new file descriptor; on error, returns `-1` and sets `errno`.​
    
- **`connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)`**  
    Initiates a connection on a socket.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`.​
    
- **`send(int sockfd, const void *buf, size_t len, int flags)`**  
    Sends data on a connected socket.  
    **Returns:** On success, returns the number of bytes sent; on error, returns `-1` and sets `errno`
    
- **`recv(int sockfd, void *buf, size_t len, int flags)`**  
    Receives data from a connected socket.  
    **Returns:** On success, returns the number of bytes received; on error, returns `-1` and sets `errno`.​
    
- **`setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)`**  
    Sets options on sockets.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`.
    
- **`getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)`**  
    Retrieves the current address to which the socket is bound.  
    **Returns:** On success, returns `0`; on error, returns `-1` and sets `errno`.​
    

---

### Network Byte Order Conversion

- **`htons(uint16_t hostshort)`**  
    Converts a 16-bit number from host to network byte order.  
    **Returns:** The converted value.
    
- **`htonl(uint32_t hostlong)`**  
    Converts a 32-bit number from host to network byte order.  
    **Returns:** The converted value.​
    
- **`ntohs(uint16_t netshort)`**  
    Converts a 16-bit number from network to host byte order.  
    **Returns:** The converted value.​
    
- **`ntohl(uint32_t netlong)`**  
    Converts a 32-bit number from network to host byte order.  
    **Returns:** The converted value.


### Multiplexing I/O
- **`select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)`**  
    Monitors multiple file descriptors for readiness to perform I/O.
    
    - `nfds`: the highest-numbered file descriptor in any set + 1
        
    - `readfds`: set of fds to check for readability (can be `NULL`)
        
    - `writefds`: set of fds to check for writability (can be `NULL`)
        
    - `exceptfds`: set of fds to check for exceptional conditions (can be `NULL`)
        
    - `timeout`: how long to wait (`NULL` = wait forever, `0` = don't wait)  
        **Returns:** On success, number of ready file descriptors; `0` on timeout; `-1` on error with `errno` set.
        

---

- **`poll(struct pollfd *fds, nfds_t nfds, int timeout)`**  
    The `poll` system call is used to **monitor multiple file descriptors** (like sockets or files) to see if they’re ready for I/O (read/write), without blocking indefinitely.
    
    - `fds`: array of `struct pollfd`, each containing `.fd` (fd to monitor) and `.events` (what to watch for, e.g., `POLLIN`, `POLLOUT`)
        
    - `nfds`: number of items in the `fds` array
        
    - `timeout`: time in milliseconds to wait (`-1` = infinite)  
        **Returns:** On success, number of fds with events; `0` on timeout; `-1` on error with `errno` set.
    
    struct pollfd {
    int   fd;        // File descriptor to monitor
    short events;    // Events to watch for (input)
    short revents;   // Events that occurred (output)
	};
### Common `events` and `revents` flags:

| Flag       | Meaning                                                       |
| ---------- | ------------------------------------------------------------- |
| `POLLIN`   | Data is ready to be **read** (e.g. data available on socket). |
| `POLLOUT`  | Ready for **writing** (e.g. socket has space in the buffer).  |
| `POLLERR`  | Error condition.                                              |
| `POLLHUP`  | Hang up (e.g. other end of socket closed).                    |
| `POLLNVAL` | Invalid file descriptor (e.g. `fd` is not open).\|            |



- **`epoll_create(int size)`**  
    Creates a new epoll instance.
    
    - `size`: obsolete, must be > 0 but ignored on modern Linux  
        **Returns:** On success, returns a file descriptor referring to the new epoll instance; `-1` on error with `errno` set.
        

---

- **`epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)`**  
    Controls which file descriptors are monitored by an epoll instance.
    
    - `epfd`: the epoll instance file descriptor
        
    - `op`: operation (`EPOLL_CTL_ADD`, `EPOLL_CTL_MOD`, or `EPOLL_CTL_DEL`)
        
    - `fd`: the target file descriptor to add/modify/remove
        
    - `event`: pointer to `epoll_event` struct describing what to monitor (ignored for `DEL`)  
        **Returns:** `0` on success; `-1` on error with `errno` set.
        

---

- **`epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)`**  
    Waits for events on fds monitored by the epoll instance.
    
    - `epfd`: the epoll instance file descriptor
        
    - `events`: array to be filled with triggered events
        
    - `maxevents`: maximum number of events to return (must be ≥ 1)
        
    - `timeout`: wait time in milliseconds (`-1` = infinite, `0` = non-blocking)  
        **Returns:** Number of triggered events on success; `-1` on error with `errno` set.


- **`kqueue(void)`**  
    Creates a new kqueue instance for event notification.  
    **Returns:** On success, returns a file descriptor for the new kqueue instance; `-1` on error with `errno` set.
    


- **`kevent(int kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, struct timespec *timeout)`**  
    Changes the set of events being monitored and retrieves events.
    
    - `kq`: the kqueue file descriptor
        
    - `changelist`: list of `kevent` structures describing the events to add, modify, or delete
        
    - `nchanges`: number of events in the `changelist`
        
    - `eventlist`: array to store the events that were triggered
        
    - `nevents`: maximum number of events that can be returned
        
    - `timeout`: time to wait for events (`NULL` = block indefinitely)  
        **Returns:** On success, returns the number of events returned in `eventlist`; `-1` on error with `errno` set.