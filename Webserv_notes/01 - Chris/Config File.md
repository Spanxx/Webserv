📆 2025-05-01 🕓 16:11
Tags: #config #parsing 

---

# What to check?
- [x] spaces not allowed
- [ ] contains all needed values
	- [ ] if not, default value or abort program?
- [ ] 

# File Format for Config file?

## ❌ Json
There is no JSON handling support within C++98
External Library would be: 🥇 **[nlohmann/json](https://github.com/nlohmann/json)** (a.k.a. _JSON for Modern C++_)
But, not allowed
**Solution**: 
- Build own Json handler -> maybe to much?
## ✅ Simpler config fomat like INI or custom one
- Read line by line
- Handle `{` and `}` to track nesting
- Store key-value pairs in a `std::map<std::string, std::string>` or custom struct
- --> thats like the nginx config file

`std::map` -> then we can access it like json

```cpp title:"map example"
std::map<std::string, std::string> config; // first argument is key, second is value --> like JSON

config["port"] = "8080";
config["index"] = index.html
...
std::cout << "Server Port is: " << config["port"] << std::endl;
```

OR we build a config class for more different datatypes?
# 📗 Rules for our config file

```
# No spaces (except in comments)
# Hashtag "#" marks a line as comment --> will be ignored
# Empty lines will be ignored
# Each section starts with a keyword and {}, containing key=value pairs
# Example:
#	server{
#	port=80
#	root=/www/
#	...
# }
# Directories in the "dir" section will be created if they don't exist
# Subdirectories inside the dir{} block must be relative to the root directory defined in the server{} block (e.g., images/ → www/images/)
# Allowed top-level keywords: server, dir, files
```

### 🛠️ How it might work conceptually:

1. **Stage 1 – Parse Phase:**
    - You walk through the config file.
    - For each `{}` block, you parse its key-value lines into a `std::map<std::string, std::string>`.
    - You associate that map with a keyword like `server`, `dir`, or `files`.

2. **Stage 2 – Validation & Normalization:**
    - You check that the maps have all required keys.
    - Normalize paths, check types (e.g., convert `"8080"` to int), validate ranges, etc.

3. **Stage 3 – Construction Phase:**
    - You use the values in the maps to populate a `Config` class (or `ServerConfig`, `DirConfig`, etc.).
    - Now your config is type-safe and ready to use by the server logic.

# Finding duplicates for keys

Instead of using `map.insert(...)` we use:
```cpp title:"check for key duplicates"
if (map.find(key) != map.end()) {
    // Duplicate key found — throw error or log warning
} else {
    map[key] = value;
}
```

---
# NGINX config example

🔗 [NGINX config example](https://www.plesk.com/blog/various/nginx-configuration-guide/)

```nginx title:"NGINX example"
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
