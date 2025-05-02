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

![[Procecss_drawing| 800]]
