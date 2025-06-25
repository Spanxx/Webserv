MULTIPART/DATA_FORM:

The `multipart/form-data` format was designed to let browsers and tools send **multiple form fields (text inputs, file uploads, etc.)** as one single request body.

Each _part_ includes:

- Its own headers (e.g. `Content-Disposition`, possibly `Content-Type`)
    
- A blank line
    
- The actual content




so far only to upload images, to be expanded later? 

if request content type = multipart/form-data
that means it's an image: 

Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryR0rw0WxIQYFm8Bu3

--> the boundary is at the beginning and the end of the request body 


inside the body there is metadata about the file:
(content dispostion etc. ) and below is the content (after a line of CRLF)

------WebKitFormBoundaryXyZ123
Content-Disposition: form-data; name="file"; filename="example.txt"
Content-Type: text/plain

Hello, this is the file content.
------WebKitFormBoundaryXyZ123--