📆 2025-04-26 🕓 14:31
Tags: #testing #telnet #nginx #tcp #ip #http 

---

Start Shell:
```shell
telnet example.com 80
```

## ✅ Good Request
```shell
GET / HTTP/1.1
Host: example.com
					// the first newline could be used for more information
					// the second newline tells the server to proceed
```

## ❌ Bad Request (missing header)
```shell
GET /index.html HTTP/1.1


```

**The Host header is needed for HTTP/1.1.
It is not nessessary in HTTP/1.0**
# 🧠 Does telnet testing also check NGINX _configuration_?

✅ **Yes, partially.**
When you send a request with **telnet** to **NGINX**, you are testing **both**:
- How **NGINX itself** behaves (default HTTP/1.1 compliance)  
    **and**
- How your **specific NGINX configuration** is set up (e.g., what files, paths, timeouts, headers it uses).

👉 So: **telnet talks to the NGINX server process**, and **the server process uses its config**.

That means:
- If your NGINX is misconfigured (wrong root directory, no index.html, wrong ports, bad server block...), you will **see the effect immediately** during your telnet session.
- If you change NGINX settings (like enabling/disabling keep-alive, modifying error pages, etc.), **your telnet tests will reflect that**.

# 📜 Example:

If your NGINX config has:
```nginx
server {
    listen 80;
    server_name localhost;

    root /var/www/html;
    index index.html;
}
```

When you telnet and request `/index.html`, **NGINX**:
- Knows to serve `/var/www/html/index.html`
- Answers based on its `root` and `index` config.

If you misconfigure it (like wrong `root`), your telnet test would give you a `404 Not Found` even if you typed everything correctly.

# Example Requests from Telnet

```html title:Timeout
HTTP/1.1 408 Request Time-out
Content-length: 110
Cache-Control: no-cache
Connection: close
Content-Type: text/html

<html><body><h1>408 Request Time-out</h1>
Your browser didn't send a complete request in time.
</body></html>
Connection closed by foreign host.
```

```html title:"content of index.html"
GET /index.html HTTP/1.1
Host: example.com

HTTP/1.1 200 OK
Content-Type: text/html
ETag: "84238dfc8092e5d9c0dac8ef93371a07:1736799080.121134"
Last-Modified: Mon, 13 Jan 2025 20:11:20 GMT
Cache-Control: max-age=2990
Date: Sat, 26 Apr 2025 12:45:54 GMT
Content-Length: 1256
Connection: keep-alive

<!doctype html>
<html>
<head>
    <title>Example Domain</title>

    <meta charset="utf-8" />
    <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style type="text/css">
    body {
        background-color: #f0f0f2;
        margin: 0;
        padding: 0;
        font-family: -apple-system, system-ui, BlinkMacSystemFont, "Segoe UI", "Open Sans", "Helvetica Neue", Helvetica, Arial, sans-serif;
        
    }
    div {
        width: 600px;
        margin: 5em auto;
        padding: 2em;
        background-color: #fdfdff;
        border-radius: 0.5em;
        box-shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);
    }
    a:link, a:visited {
        color: #38488f;
        text-decoration: none;
    }
    @media (max-width: 700px) {
        div {
            margin: 0 auto;
            width: auto;
        }
    }
    </style>    
</head>

<body>
<div>
    <h1>Example Domain</h1>
    <p>This domain is for use in illustrative examples in documents. You may use this
    domain in literature without prior coordination or asking for permission.</p>
    <p><a href="https://www.iana.org/domains/example">More information...</a></p>
</div>
</body>
</html>
```