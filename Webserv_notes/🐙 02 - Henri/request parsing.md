
error checks while parsing: 
- ~~request line must have 3 parts~~
- ~~Missing blank line between headers and body.
- Incorrectly formatted empty lines (e.g., missing `\r` or extra characters).
- Verify that the HTTP version follows the pattern `HTTP/1.x`.
- HTTP version not supported (--> otherwise 505 HTTP Version Not Supported)
- path name should not have illegal characters or wrong format [[#^ac3272]]  (--> otherwise send error: 400 Bad Request)
- Ensure that the path starts with `/` (unless it’s an absolute URL).
- ~~Headers are missing the colon (`:`) separator.~~
- ~~Headers have extra spaces before or after the colon.~~
- ~~Headers are empty (just a `\r` or empty line).~~
- ~~Strip any leading/trailing whitespace from both the key and value before storing.~~
- If a header line is empty (i.e., `\r`), stop parsing headers.
- Invalid value format for headers like `Content-Length`, `Host`, etc.
- Missing required headers, like `Host`.
- Check the validity of certain header values. For example, ensure `Content-Length` is a valid integer.
- If the request is too large, you should reject it.
- **Common errors**: Headers or body exceed a maximum size limit.
- **How to handle**: Set a maximum size limit for headers or body content and reject the request if the limit is exceeded.

- If the connection should be kept alive, but the client or server doesn't handle it correctly, this can cause issues with long-running requests.
- **Common errors**: Missing or incorrect `Connection` header.
- Misunderstood HTTP version handling (keep-alive with `HTTP/1.1`, but client sends `HTTP/1.0`).
- **How to handle**: Check the `Connection` header for values like `keep-alive`.
- Ensure proper handling of keep-alive and close connections.
    

error checks while processing:
- ~~is the method valid (--> otherwise send 405 error code)~~

###  **Invalid Body Content**
If the request contains a body (e.g., in a POST or PUT request), it should conform to the appropriate format, especially for form data or JSON.
**Common errors**:
- Body data is not properly formatted.
- Content-Type header doesn't match the actual body format.
**How to handle**:
- Validate the body format based on the `Content-Type` header.
- For JSON bodies, ensure that the body can be parsed as JSON, for example.



### **Illegal Characters in the Path**

^ac3272

An HTTP request path refers to the part of the URL after the domain name (or IP address), typically starting with a `/`. HTTP paths follow specific rules, and any characters not conforming to these rules are considered **illegal**. Here are some general guidelines for illegal characters:

- **Control Characters**: Any ASCII control characters (0-31) except for space, such as `\x00-\x1F`, are illegal.
    
- **Non-ASCII Characters**: In traditional URLs, only ASCII characters are allowed. This means characters like `é`, `ü`, `£`, or any other non-ASCII characters are technically illegal. However, modern implementations may allow URL encoding (percent-encoding) for non-ASCII characters.
    
- **Reserved Characters**: Some characters have special meanings in URLs and are reserved. These include `?`, `&`, `=`, `#`, `:`, `/`, `;`, and others. If these characters are not used correctly (e.g., as part of a query string or fragment identifier), they can make the path improperly formatted.
    
- **Spaces**: Spaces are not allowed in paths, although they can be percent-encoded (`%20`).
    

###  **Improper Formatting in the Path**

An improperly formatted path could refer to:

- **Unencoded Characters**: If characters like spaces, slashes (`/`), or non-ASCII characters are not percent-encoded (e.g., a space should be `%20`), then the path is improperly formatted.
    
- **Starting or Ending with a Slash**: While it's common for paths to start with a `/`, paths that end with a `/` may cause issues, depending on the server configuration. For example, `/folder/` might be treated differently than `/folder`.
    
- **Double Slashes**: Paths like `//folder/file` or `folder//file` can sometimes be treated differently. While some servers treat double slashes as a single slash, others might not interpret the path correctly.
    
- **Empty Path**: An empty path (i.e., a request like `GET / HTTP/1.1`) may be interpreted as a request for the root (`/`), which is valid, but if it’s unexpected in your context, it could be an issue.

[^1]: 
