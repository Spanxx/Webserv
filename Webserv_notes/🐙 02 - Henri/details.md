
### **1. The Role of `config.conf`**

The `config.conf` file serves as the **configuration** for the web server. It contains static settings, such as:

- The **port** the server listens to (`listen` directive).
    
- The **root directory** where files are served from (`root` directive).
    
- The **error pages** for handling things like 404 errors or 500 errors.
    
- Server **locations** or **routes**, and their associated settings (e.g., paths for specific URLs).
    

### **Example of a basic config:**

conf

Copy code

`server {     listen 8080;  # Port the server listens on     root ./www;   # Directory where the server looks for files      location / {         # Serve files from the 'root' directory     }      location /upload {         # Handle upload requests (could be a different handler)     } }`

This file defines a server that listens on port 8080 and serves files from the `./www` directory. If a request is made to `/upload`, it could be routed differently (e.g., to handle file uploads).

### **2. Handling Multiple Requests:**

In your **server loop**, the server needs to handle **multiple requests** sequentially (or concurrently if you decide to add multithreading or multiprocessing).

**How it works:**

1. **Server listens for incoming connections:** When you start the server with `./webserv config.conf`, it begins by:
    
    - Binding to the configured port (`8080` in this case).
        
    - Listening for incoming connections from clients (e.g., browsers).
        
2. **Server accepts a connection:** Every time a new client connects to your server, the server:
    
    - Accepts the incoming connection.
        
    - Creates a new socket dedicated to that client.
        
3. **Server processes the request:** The server then processes the incoming HTTP request. It checks:
    
    - The HTTP **method** (GET, POST, etc.).
        
    - The **URI** requested (e.g., `/index.html`).
        
    - The **headers** (Content-Type, Authorization, etc.).
        
    - The **body** (if it's a POST request).
        
4. **Server responds:** Based on the request, the server sends back an appropriate HTTP response, which could be:
    
    - A **file** (like `index.html`).
        
    - An **error page** (if the requested resource doesn’t exist).
        
    - A **dynamic response** (if you've set up routing logic for things like `/upload`).
        
5. **Server repeats for each new connection:** After responding to the first client, the server goes back into its listening state and waits for the next incoming connection. It can handle many different clients, one at a time (synchronously) or in parallel (using threads or async).
    

### **3. Handling Multiple Requests:**

While **`config.conf`** provides the configuration details (like ports, routes, and settings), the **server itself** must continuously **loop** and handle multiple requests. Here’s how the flow might work with multiple requests:

1. **Incoming Request:**
    
    - A client (browser) sends an HTTP request to the server (e.g., `GET /index.html`).
        
    - The server accepts the request, processes it, and sends a response back.
        
2. **Subsequent Requests:**
    
    - The client might send additional requests, either for the same resource (e.g., multiple `GET` requests for `/index.html`) or for different resources (e.g., `GET /about.html` or `POST /upload`).
        
    - The server can handle all these requests **sequentially** or **concurrently**, depending on how it’s designed (using threads, async I/O, etc.).
        

### **4. Handling Multiple Clients:**

A basic setup with a **single-threaded server** will look like this:

1. The server listens on the configured port.
    
2. A client connects.
    
3. The server accepts the request and processes it.
    
4. The server sends the response and closes the connection.
    
5. The server goes back to listening for the next client.
    

You can extend this with features like:

- **Asynchronous I/O** (using `select` or `poll` to handle multiple connections).
    
- **Multithreading** or **multiprocessing** to handle each client request in its own thread or process, allowing the server to process multiple requests in parallel.



Once you start your `webserv` program using the configuration file, the server is actively listening for incoming HTTP requests. You don't need to manually "send" requests from within the server program itself—clients (such as a browser or command-line tools) will send requests to the server.


---

### **1. Start the Web Server:**

Once you have your `webserv` program and the `config.conf` file set up, you would run your server using a command like this:

bash

Copy code

`./webserv config.conf`

- The server will read the configuration file (`config.conf`).
    
- The server will bind to the port defined in the configuration file (e.g., port 8080).
    
- The server will begin **listening** for incoming requests.
    

### **2. Send HTTP Requests to the Server:**

After starting the server, you can send requests to it from **any HTTP client**, such as:

#### **Option 1: Web Browser (e.g., Chrome, Firefox)**

1. Open your web browser (Chrome, Firefox, Safari, etc.).
    
2. Type the **server's address** in the address bar. Assuming you are running the server locally and the port is `8080`, you would enter:
    
    arduino
    
    Copy code
    
    `http://localhost:8080`
    
3. Press **Enter**.
    

- The browser will send an HTTP **GET** request to your server, asking for the root resource (`/`).
    
- Your `webserv` server will receive the request, process it, and send back an HTTP response (as defined in your server's logic).
    
- The browser will display the content sent back by the server (such as HTML, images, etc.).
    

#### **Option 2: cURL (Command Line)**

If you want to send an HTTP request from the terminal, you can use a command-line tool like **cURL**. Here’s an example:

bash

Copy code

`curl http://localhost:8080`

- This sends a **GET** request to `http://localhost:8080`, where your server is running.
    
- The server will respond with the defined response (e.g., the HTML page or a 404 error if not found).
    
- You will see the response printed in the terminal.
    

#### **Option 3: Postman (for testing APIs)**

If you need more control over HTTP requests, including **POST** requests or complex headers, you can use **Postman**:

1. Open Postman.
    
2. Set the request type (e.g., **GET**, **POST**, etc.).
    
3. Enter the URL of your server (e.g., `http://localhost:8080`).
    
4. Click **Send**.
    

Postman is particularly useful for testing REST APIs, allowing you to configure headers, request bodies, and different HTTP methods.

#### **Option 4: HTTP Requests Programmatically (from your code)**

You can also send HTTP requests programmatically from your code, using libraries in different programming languages. For example, in Python, you can use the `requests` library:

python

Copy code

`import requests  response = requests.get('http://localhost:8080') print(response.text)  # Print the response from the server`

This sends an HTTP request to your server and prints the response in the console.

---

### **3. Server Response:**

After receiving the request, the `webserv` server will process it:

- If the request is for an existing resource (like a file on disk), the server will read the file and send it back in the HTTP response.
    
- If the request is invalid (e.g., a 404 error due to a missing page), the server will send an error response.
    
- For dynamic resources (like form submissions via **POST**), the server might perform additional logic and send back a response.
    

The server will then **close the connection** or keep it open (depending on whether `Connection: keep-alive` is set in the headers), ready to handle the next request.

---

### **4. What Happens After the Request is Sent:**

Once the request is sent and the server responds:

1. The **client** (browser, cURL, Postman, etc.) receives the HTTP response.
    
2. The client will display or use the data from the response.
    
    - In a browser, this could be an HTML page rendered for the user to see.
        
    - In a terminal with cURL, the raw HTML or response body will be printed.
        

---

### **5. Handling Multiple Requests:**

The server will continue listening for **multiple requests** in a loop. If you have more than one client (browser tab, multiple cURL commands, etc.), the server will handle each of them one by one unless it's implemented to handle requests concurrently (using threads, async, etc.).

### **Next Steps**:

If you're looking to test different **HTTP methods** or **custom routes**, you can modify your server code or config file to handle different types of requests.

For example, if you added a POST handler for a `/submit` route in your server, you would:

1. Set up the route in the config file.
    
2. Write logic on the server side to handle POST requests to `/submit`.
    
3. Use **Postman** or **cURL** to send a POST request to `http://localhost:8080/submit` with data.
    

---

### **Summary:**

- To interact with your `webserv` program, you need to **start the server** with the configuration file.
    
- **Clients** (e.g., browsers, cURL, Postman) will send requests to the server.
    
- The server processes each incoming request and sends an appropriate HTTP response.
    
- You can test the server by visiting `http://localhost:8080` in a browser or using tools like **cURL** or **Postman** for more complex scenarios.
    



You can send a variety of HTTP requests or commands to the `webserv` server, depending on the functionality you want to test or implement. The most common types of HTTP requests are **GET**, **POST**, **PUT**, **DELETE**, and others. These methods are part of the HTTP protocol and are used to perform different actions on resources hosted by the server.

Here’s a list of the most common HTTP requests/commands you might send to a server, along with brief descriptions:

### 1. **GET Request**

- **Purpose:** Retrieve a resource from the server (typically a webpage or a file).
    
- **Typical Use:** Requesting an HTML page, an image, or any resource on the server.
    
- **Example (Browser):**
    
    h
    
    Copy code
    
    `GET http://localhost:8080/index.html HTTP/1.1 Host: localhost:8080`
    
- **Example (cURL):**
    
    bash
    
    Copy code
    
    `curl http://localhost:8080/index.html`
    
- **Example (Postman):**
    
    - Method: **GET**
        
    - URL: `http://localhost:8080/index.html`
        
- **Response Example:**  
    An HTTP response with the content of the requested resource, typically HTML or other static content like images.
    

---

### 2. **POST Request**

- **Purpose:** Send data to the server (often to create or update a resource).
    
- **Typical Use:** Sending form data, uploading files, etc.
    
- **Example (Browser):** A form submission in HTML:
    
    html
    
    Copy code
    
    `<form action="http://localhost:8080/submit" method="POST">   <input type="text" name="username" />   <input type="password" name="password" />   <input type="submit" value="Submit" /> </form>`
    
- **Example (cURL):**
    
    bash
    
    Copy code
    
    `curl -X POST -d "username=admin&password=secret" http://localhost:8080/submit`
    
- **Example (Postman):**
    
    - Method: **POST**
        
    - URL: `http://localhost:8080/submit`
        
    - Body:
        
        ini
        
        Copy code
        
        `username=admin&password=secret`
        
- **Response Example:**  
    The server could respond with a success message or a confirmation, like `HTTP/1.1 200 OK`.
    

---

### 3. **PUT Request**

- **Purpose:** Update or replace a resource on the server.
    
- **Typical Use:** Updating an existing file or resource on the server (e.g., modifying an existing user profile).
    
- **Example (cURL):**
    
    bash
    
    Copy code
    
    `curl -X PUT -d "username=admin&password=newpassword" http://localhost:8080/user/profile`
    
- **Example (Postman):**
    
    - Method: **PUT**
        
    - URL: `http://localhost:8080/user/profile`
        
    - Body:
        
        ini
        
        Copy code
        
        `username=admin&password=newpassword`
        
- **Response Example:**  
    The server could respond with an updated version of the resource, or a success status like `HTTP/1.1 200 OK`.
    

---

### 4. **DELETE Request**

- **Purpose:** Delete a resource on the server.
    
- **Typical Use:** Deleting a file, a user account, or some other resource.
    
- **Example (cURL):**
    
    bash
    
    Copy code
    
    `curl -X DELETE http://localhost:8080/user/profile`
    
- **Example (Postman):**
    
    - Method: **DELETE**
        
    - URL: `http://localhost:8080/user/profile`
        
- **Response Example:**  
    The server could respond with a status like `HTTP/1.1 200 OK` or `HTTP/1.1 204 No Content` if the resource was successfully deleted.
    

---

### 5. **HEAD Request**

- **Purpose:** Similar to **GET**, but it only retrieves the headers of a resource, not the actual content.
    
- **Typical Use:** Checking metadata or file headers without downloading the full resource.
    
- **Example (cURL):**
    
    bash
    
    Copy code
    
    `curl -I http://localhost:8080/index.html`
    
- **Example (Postman):**
    
    - Method: **HEAD**
        
    - URL: `http://localhost:8080/index.html`
        
- **Response Example:**  
    The server will respond with just the headers, such as the content type, length, etc., but not the actual content of `index.html`.
    

---

### 6. **OPTIONS Request**

- **Purpose:** Retrieve allowed HTTP methods for a specific resource.
    
- **Typical Use:** Checking which methods (GET, POST, PUT, etc.) are allowed for a resource.
    
- **Example (cURL):**
    
    bash
    
    Copy code
    
    `curl -X OPTIONS http://localhost:8080/index.html`
    
- **Example (Postman):**
    
    - Method: **OPTIONS**
        
    - URL: `http://localhost:8080/index.html`
        
- **Response Example:**  
    The server will respond with the `Allow` header, listing the HTTP methods that are allowed for the resource (e.g., `GET, POST, PUT`).
    

---

### 7. **PATCH Request**

- **Purpose:** Apply partial modifications to a resource.
    
- **Typical Use:** Updating only specific fields of a resource (e.g., updating just the email of a user without changing the password).
    
- **Example (cURL):**
    
    bash
    
    Copy code
    
    `curl -X PATCH -d "email=newemail@example.com" http://localhost:8080/user/profile`
    
- **Example (Postman):**
    
    - Method: **PATCH**
        
    - URL: `http://localhost:8080/user/profile`
        
    - Body:
        
        ini
        
        Copy code
        
        `email=newemail@example.com`
        
- **Response Example:**  
    The server could respond with the updated resource or a success message.
    

---

### 8. **Custom Requests (depending on your server implementation)**

In some cases, you might create **custom routes** or commands for your server. For example, a **POST** request for file uploads, or a **GET** request to fetch a list of files from the server.

- **Example:**
    
    bash
    
    Copy code
    
    `curl -X GET http://localhost:8080/images`
    
- This could return a list of all image files on the server, depending on your server’s implementation.
    

---

### Summary of Common HTTP Methods:

- **GET**: Retrieve a resource from the server (e.g., a web page, an image).
    
- **POST**: Send data to the server (e.g., submit a form, upload a file).
    
- **PUT**: Replace or update a resource.
    
- **DELETE**: Remove a resource.
    
- **HEAD**: Get only the headers of a resource.
    
- **OPTIONS**: Find out what HTTP methods are supported for a resource.
    
- **PATCH**: Apply partial modifications to a resource.
    

---

### How to Use These Requests in the Context of `webserv`:

- When you send a request (like `GET`, `POST`, etc.), your server should handle each method differently, according to the resource requested and the logic defined in the configuration or server code.
    
- **GET** requests typically return HTML or files.
    
- **POST** requests are for submitting data, like form submissions or file uploads, and will typically trigger some processing on the server.
    
- For each request type, the server needs to determine how to handle it (which file to serve, what data to store, etc.), based on the logic written in the server code and the configuration file.