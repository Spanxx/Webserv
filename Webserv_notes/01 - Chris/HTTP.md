📆 2025-04-26 🕓 12:56
Tags: #http #socket  

---
# HTTP = Hypertext Transfer Protocol

1. HTTP Client (e.g. webbrowser) sends a HTTP Request to the HTTP Server
2. Server processes the request and sends HTTP Response to the HTTP Client

# HTTP Client (Web Browser)

- we type some URL/Adress of the webiste in the browser
	- `http://www.example.com` --> the server reconctructs the URL/Adress as `http://www.example.com:80`
- this will be send to the server
- If the server has a default page --> we visit the folder on the server and load the page (e.g. index.html)
## Structure of a HTTP Request

![[example http request.png | 500]]

## HTTP Methods

### There are 9 HTTP Methods
1. GET - Fetch a URL (Default method used by HTTP)
2. HEAD - Fetch information about a URL
3. POST - Send form data to a URL and get a response back
4. PUT - Store to an URL
5. DELETE - Deletes the targets resource
6. CONNECT - The intermediary establish a [TCP/IP tunnel](https://en.wikipedia.org/wiki/Tunneling_protocol "Tunneling protocol") to the origin server identified by the request target
	--> is used to secure connections through one or more HTTP Proxies with [TLS](https://en.wikipedia.org/wiki/Transport_Layer_Security)
7. OPTIONS - Transfers the the HTTP methods that it supports (Can check the functionality of webserver by requesting '\*'  instead of a specific resource)
8. TRACE - The target resource transfer the received request in the response body
9. PATCH - The target resource modify its state according to the partial update defined in the enclosed reqeuest
	--> can be used to update a part of a file/document without having to transfer it enterily
	--> this saves bandwitdth

```ad-tldr
**Rest APIs use**
1. GET
2. PUT
3. POST
4. DELETE
```
# HTTP Server

## Structure of a HTTP Response

![[example http response.png| 500]]

**HEADER**
These 3 Header informations are minimum required.

1. `HTTP/1.1 200 OK` → This mentions what version of HTTP we are using, Status code and Status message.
2. `Content-Type: text/plain` → This says that I’m (server) sending a plain text. There are many Content-Types. For example, for images we use [this](https://msdn.microsoft.com/en-us/library/ms527550\(v=exchg.10\).aspx).
3. `Content-Length: 12` → It mentions how many bytes the server is sending to the client. The web-browser only reads how much we mention here.

**Content-Type: image**
The image content type allows standardized image files to be included in messages.

**image/g3fax** [RFC1494]
**image/gif** [RFC1521]
**image/ief** (Image Exchange Format) [RFC1314]
**image/jpeg** [RFC1521]
**image/tiff** (Tag Image File Format) [RFC2301]

**BODY**
Here we send out Data.
1. calculate how many bytes we are sending --> for `Content-Length` in the header
2. set `Content-Type` according to the data we are sending

**STATUS CODE**
Status codes are issued by a server in response to a request.
The first digit specifies on of five standard classes of responses.

1xx informational response
2xx success
3xx redirection
4xx client errors
5xx server errors

Detailed list on wikipedia:
[List of HTTP status codes](https://en.wikipedia.org/wiki/List_of_HTTP_status_codes)

### Last steps

- If the Client is allowed to access the data/page, we read the file (e.g. index.html) and store everything in a variable.
- We count the bytes, during reading or afterwards for `Content-Length` (could be done with return value from `read())
- We construct the Response Header, add a newline and append the data which we read from the file.
- Send the response to the client.