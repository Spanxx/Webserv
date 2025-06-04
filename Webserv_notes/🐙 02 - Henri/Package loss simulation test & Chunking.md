
#### NOTES ON CHUNKING

  
The introduction of chunked encoding provided various benefits:

- Chunked transfer encoding allows a server to maintain an [HTTP persistent connection](https://en.wikipedia.org/wiki/HTTP_persistent_connection "HTTP persistent connection") for dynamically generated content. In this case, the HTTP Content-Length header cannot be used to delimit the content and the next HTTP request/response, as the content size is not yet known. Chunked encoding has the benefit that it is not necessary to generate the full content before writing the header, as it allows streaming of content as chunks and explicitly signaling the end of the content, making the connection available for the next HTTP request/response.
- Chunked encoding allows the sender to send additional header fields after the message body. This is important in cases where values of a field cannot be known until the content has been produced, such as when the content of the message must be digitally signed. Without chunked encoding, the sender would have to buffer the content until it was complete in order to calculate a field value and send it before the content.

**Handling Chunked Requests (Transfer-Encoding: chunked)**

🔍 What is `Transfer-Encoding: chunked`?

It's an HTTP/1.1 mechanism where the body is **sent in chunks**, and each chunk is **preceded by its size** in hex.

Example:

makefile

CopyEdit

`POST /upload HTTP/1.1 Host: example.com Transfer-Encoding: chunked  
7\r\n 
Mozilla\r\n 
9\r\n 
Developer\r\n 
7\r\n 
Network\r\n 
0\r\n 
\r\n`

Each chunk:

- Starts with a line that contains its length in hex (e.g., `7\r\n`)
    
- Followed by that many bytes
    
- Ends with `0\r\n\r\n` (signals end of body)
    

✅ To support it, your server must:

- Detect `Transfer-Encoding: chunked` header
    
- Read and parse each chunk in a loop
    
- Assemble the full body
    
- Stop at the zero-length chunk
#### **NOTES ON REDIRECTING TO INDEX WHEN REQUEST IS DIRECTORY**

**Yes, redirecting to `/index.html` when the URI is `/` is standard behavior.**  
What matters is **how** you do it:  
You should **serve** `index.html` (status 200), not redirect (status 301/302), unless explicitly configured to do so.


🔍 What's Happening in Your Case?

- Client requests:
    
    sql
    
    CopyEdit
    
    `GET / HTTP/1.1`
    
- Your server detects that `/` is a directory and responds with a **redirect to `/index.html`**.
    

While that's not **technically wrong**, it's **not what most servers do** by default.

---

✅ What Should You Do?

Instead of responding with:

pgsql

CopyEdit

`HTTP/1.1 302 Found Location: /index.html`

You should:

1. Recognize that `/` is a directory.
    
2. Check for a default file like `index.html` inside that directory.
    
3. **Serve it directly**, returning:
    
    less
    
    CopyEdit
    
    `HTTP/1.1 200 OK Content-Type: text/html [index.html content here...]`
    

---

🌐 What Do Apache/Nginx/Other Servers Do?

- If a client requests `/`:
    
    - They **look for an index file** in that directory (`index.html`, `index.php`, etc.)
        
    - They **serve it directly**, not as a redirect.
        

Unless there's an explicit `DirectoryIndex` redirect rule or a `rewrite`, no redirect happens.

---

🧠 When Would You Redirect?

You might send a redirect (302/301) to `/index.html` **only if**:

- You’re serving a static site where `/index.html` is meant to be a full route.
    
- You want the **URL to show `/index.html`** explicitly in the browser bar.
    

But most modern servers and apps avoid that to keep URLs clean.

---

✅ Conclusion

|Case|Recommended Behavior|
|---|---|
|Client requests `/`|Look up root directory, find `index.html`, and **serve it directly**|
|Client requests directory `/blog/`|Check `/blog/index.html`, and serve it if found|
|No `index.html` found|Either show directory listing (if allowed), or return 403/404|



#### NOTES ON MULTIPLE PIPELINED REQUESTS IN ONE TCP STREAM
**In `handle_request`, after processing a full request, you delete the Request object immediately**

cpp

CopyEdit

`delete request; _requestCollector.erase(_socketArray[i].fd);`

This is fine **if** you handle exactly one request per connection at a time. But:

- What if the client sends **multiple pipelined requests** in one TCP stream? You have leftover data in `_socketBuffers` after consuming one request.
    
- Your `_socketBuffers` is erased completely here:
    
    cpp
    
    CopyEdit
    
    `_socketBuffers.erase(_socketArray[i].fd);`
    
    This means if there's extra data (e.g., next pipelined request), it will be lost.

**You don’t consume processed data from the buffer**

You do this:

cpp

CopyEdit

`std::string &data = _socketBuffers[_socketArray[i].fd]; // ... after handling full request ... _socketBuffers.erase(_socketArray[i].fd);`

But you never remove the processed request bytes (headers + body) from the buffer. This means leftover data won't be processed properly if multiple requests come quickly on the same connection.

---

4. **You need to loop inside `read_from_connection` or `handle_request` to process multiple requests**

If the buffer contains multiple requests pipelined back-to-back, you want to process all complete requests before returning.

---