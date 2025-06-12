## 🧪 How to Test the DELETE Method in HTML

To test the DELETE method, you can add a **form** to your HTML with a way to simulate a DELETE request. However, **HTML forms only support `GET` and `POST`** directly. So, to simulate `DELETE`, you can either:

### ✅ Option 1: Use JavaScript to Send a DELETE Request

This is the most realistic way to test `DELETE` from the browser:

html```
<h2>Delete a File</h2>
<form id="delete-form">
	<input type="text" id="filename" placeholder="Enter filename to delete">
	<button type="submit">Delete File</button>
</form>

<script>
document.getElementById('delete-form').addEventListener('submit', function(e) {
	e.preventDefault(); // prevent default form behavior

	const filename = document.getElementById('filename').value;

	fetch(`/uploads/${filename}`, {
		method: 'DELETE'
	})
	.then(response => {
		if (response.ok) {
			alert('File deleted successfully!');
		} else {
			alert('Error deleting file.');
		}
	})
	.catch(error => {
		console.error('Error:', error);
	});
});
</script> ```


This will send a `DELETE` request to `/uploads/filename`, which you can handle in your C++ server.

---

## ❓ How Do HTML Forms Work?

### Basic Structure:

html```
<form action="/target" method="POST"> 	<input type="text" name="username"> 	<button type="submit">Send</button> </form>```

### Browser Behavior:

1. **`<form>`** defines how to send data to the server.
    
2. `action="/target"` – the URL to which the request will be sent.
    
3. `method="POST"` – the HTTP method (`GET` or `POST`) used to send data.
    
4. When the user clicks "Submit", the browser:
    
    - Collects all input data from the form (`<input>`, `<textarea>`, `<select>`, etc.).
        
    - Encodes it (usually `application/x-www-form-urlencoded` or `multipart/form-data`).
        
    - Sends an HTTP request to the `action` URL with that data in the body (for POST).
        

### Example:

http```
POST /upload.py HTTP/1.1 Host: localhost:8080 Content-Type: multipart/form-data; boundary=... Content-Length: 12345  --boundary Content-Disposition: form-data; name="file"; filename="test.txt" Content-Type: text/plain  [file content] --boundary--```


### **Using External Tools (Not Native to HTML)**

If you’re not limited to browser UI, you can send real `DELETE` requests using tools like:

- `curl` in terminal:
    
    bash
    
    `curl -X DELETE http://localhost:8080/uploads/test.txt`
    
- Postman or Insomnia – for GUI-based testing of DELETE requests.


## 📦 Real World Use Cases in Webpages

### 🧹 1. **Delete a User Account**

html```
<button onclick="deleteAccount()">Delete My Account</button>

<script>
function deleteAccount() {
	fetch('/user/123', { method: 'DELETE' })
		.then(res => res.ok ? alert("Account deleted.") : alert("Failed."))
}
</script>```

---

### 🧼 2. **Delete a Blog Post**

 html ```
 <button onclick="deletePost(42)">Delete Post</button>

<script>
function deletePost(postId) {
	fetch(`/posts/${postId}`, { method: 'DELETE' })
		.then(res => {
			if (res.ok) window.location.reload();
			else alert("Error deleting post.");
		});
}
</script>```

---

### 🗂️ 3. **Delete an Uploaded File**

html```

<ul>
	<li>document.pdf <button onclick="deleteFile('document.pdf')">🗑️</button></li>
</ul>

<script>
function deleteFile(filename) {
	fetch(`/uploads/${filename}`, { method: 'DELETE' })
		.then(res => res.ok ? alert("Deleted") : alert("Not deleted"));
}
</script>```

