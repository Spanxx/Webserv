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


# 2. [[Config File]]

🔗 [NGINX config example](https://www.plesk.com/blog/various/nginx-configuration-guide/)

```
`server {`
`listen 80 default_server;`
`listen [::]:80 default_server;`
`server_name example.com www.example.com;`
`root /var/www/example.com;`
`index index.html;`
`try_files $uri /index.html;`
`}`
```

### `server {`

- **Starts a server block**, which defines configuration for one virtual host (e.g., a domain like `example.com`).
- Inside this block, you define ports, hostnames, document root, etc.
---
### 🔹 `listen 80 default_server;`

- Tells Nginx to listen on **port 80** (standard HTTP).
- `default_server` marks this as the **default virtual host** for port 80, meaning if no domain matches, this one is used.
---
### 🔹 `listen [::]:80 default_server;`

- Same as above, but for **IPv6**.
- `[::]` is shorthand for “any IPv6 address.”
---
### 🔹 `server_name example.com www.example.com;`

- Defines which **domain names** this server block responds to.
- Requests for `example.com` and `www.example.com` will be handled here.
---
### 🔹 `root /var/www/example.com;`

- Defines the **document root** — the folder that contains the site files.
- When a request is made (e.g. `/about.html`), it maps to `/var/www/example.com/about.html`.
---
### 🔹 `index index.html;`

- Sets the **default file** to serve if a directory is requested.
- Example: Requesting `/blog/` → serve `/var/www/example.com/blog/index.html`.
---
### 🔹 `try_files $uri /index.html;`

- This is a **fallback mechanism**:
    - First, try to serve the exact file requested by URI (e.g., `/about.html`).
    - If it doesn't exist, **serve `/index.html`** instead.
    - Useful for single-page applications like React or Vue apps.


