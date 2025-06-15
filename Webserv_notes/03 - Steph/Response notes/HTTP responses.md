An HTTP response is made by a server to a client. The aim of the response is to provide the client with the resource it requested, or inform the client that the action it requested has been carried out; or else to inform the client that an error occurred in processing its request.

An HTTP response contains:

1. A status line.
2. A series of HTTP headers, or header fields.
3. A message body, which is usually needed.

As in a request message, each HTTP header is followed by a carriage return line feed (CRLF). After the last of the HTTP headers, an additional CRLF is used (to give an empty line), and then the message body begins.

## Status line[](https://www.ibm.com/docs/en/cics-ts/6.x?topic=protocol-http-responses#dfhtl22__title__2 "Copy to clipboard")

[](https://github.com/Spanxx/Webserv/blob/stephVibes/response_for_get_request/Webserv_notes/03%20-%20Steph/Response/HTTP%20responses.md#status-line)

The status line is the first line in the response message. It consists of three items:

1. The HTTP version number, showing the HTTP specification to which the server has tried to make the message comply.
2. A **status code**, which is a three-digit number indicating the result of the request.
3. A **reason phrase**, also known as status text, which is human-readable text that summarizes the meaning of the status code.

An example of a response line is:

```
HTTP/1.1 200 OK
```

In this example:

- the HTTP version is `HTTP/1.1`
- the status code is `200`
- the reason phrase is `OK`

[Status codes and reason phrases](https://www.ibm.com/docs/en/SSJL4D_6.x/fundamentals/web/dfhtl_httpstatus.html#dfhtl_httpstatus "In the HTTP response that is sent to a client, the status code, which is a three-digit number, is accompanied by a reason phrase (also known as status text) that summarizes the meaning of the code. Along with the HTTP version of the response, these items are placed in the first line of the response, which is therefore known as the status line.") explains more about these elements of the status line.

## HTTP headers[](https://www.ibm.com/docs/en/cics-ts/6.x?topic=protocol-http-responses#dfhtl22__title__3 "Copy to clipboard")

[](https://github.com/Spanxx/Webserv/blob/stephVibes/response_for_get_request/Webserv_notes/03%20-%20Steph/Response/HTTP%20responses.md#http-headers)

The HTTP headers for a server's response contain information that a client can use to find out more about the response, and about the server that sent it. This information can assist the client with displaying the response to a user, with storing (or caching) the response for future use, and with making further requests to the server now or in the future. In the case of an unsuccessful request, headers can be used to tell the client what it must do to complete its request successfully.

An empty line (that is, a CRLF alone) is placed in the response message after the series of HTTP headers, to divide the headers from the message body.

## Message body[](https://www.ibm.com/docs/en/cics-ts/6.x?topic=protocol-http-responses#dfhtl22__title__4 "Copy to clipboard")

[](https://github.com/Spanxx/Webserv/blob/stephVibes/response_for_get_request/Webserv_notes/03%20-%20Steph/Response/HTTP%20responses.md#message-body)

The message body of a response may be referred to for convenience as a response body.

Message bodies are used for most responses. The exceptions are where a server is responding to a client request that used the HEAD method (which asks for the headers but not the body of the response), and where a server is using certain status codes.

For a response to a successful request, the message body contains either the resource requested by the client, or some information about the status of the action requested by the client. For a response to an unsuccessful request, the message body might provide further information about the reasons for the error, or about some action the client needs to take to complete the request successfully.

responsibility as the response generator is to:

1. **Structure internal data for the response** (headers, body, etc.).
    
2. **Format it into a valid HTTP response string**.
    
3. **Serve files, generate error messages, handle CGI output, redirects, etc., based on config and request**.