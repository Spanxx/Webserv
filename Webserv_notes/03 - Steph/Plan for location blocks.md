See [[#Final Structure to use for configuration file]] 
### General Configuration Structure

1. **Introduce `location` blocks:** Your current `dir`, `pages`, `files`, `filetypes`, `config`, `cgi-bin`, and `redir` blocks are global but many of the requirements are path-specific. You need to define `location` blocks _inside_ your `server` block to apply rules to specific URL paths (e.g., `/`, `/uploads`, `/cgi-bin`).
    
    - **Example Structure (conceptual):**
        
        ```
        server {
            listen=8080
            host=127.0.0.1
            server_name example.com; # See point 2
        
            # Global settings for this server
            root www/;
            index index.html; # Default index file for this server
            max_body_size 5M; # Your limitRequestLine is covered by client_max_body_size or similar
            error_page 404 /errors/404.html; # See point 3
        
            location / { # Root path
                methods GET POST; # See point 4
                autoindex off;    # See point 6
                # root www/html; # If you want a different root for /
            }
        
            location /uploads { # For file uploads
                methods POST;
                upload_store www/files/uploads; # See point 9
            }
        
            location /cgi-bin/ { # For CGI scripts
                methods GET POST;
                cgi_pass /usr/bin/python3; # Or /usr/bin/php-cgi etc. See point 10
                # cgi_root www/cgi-bin; # If your CGI scripts are in a specific sub-root
            }
        
            location /old-path { # For redirects
                return 301 /new-path; # See point 8
            }
        
            # More specific locations as needed
        }
        ```
        

### Specific Requirements & To-Do List

1. **Choose the port and host of each ’server’.**
    
    - **Status:** Partially met. `ports=8080,5555` and `host=127.0.0.1` are present.
    - **To Do:** If you intend to have _multiple distinct servers_ (e.g., `server_A` on 8080, `server_B` on 5555), you need separate `server { ... }` blocks for each. If `ports=8080,5555` means one server listens on both, that's fine.
2. **Set up the `server_names` or not.**
    
    - **Status:** Missing.
    - **To Do:** Add a `server_name` directive within your `server` block (e.g., `server_name localhost example.com;`). This is crucial for demonstrating virtual hosting.
3. **The first server for a host:port will be the default for this host:port.**
    
    - **Status:** This is an implicit rule, not a config setting.
    - **To Do:** Ensure your configuration file's order reflects which server you want to be the default for a given `host:port` combination if you have multiple `server` blocks listening on the same `host:port`.
4. **Set up default error pages.**
    
    - **Status:** `pages{ error=www/data/status_page.html }` is present but is global.
    - **To Do:** Implement specific `error_page` directives within the `server` block, mapping different error codes to potentially different HTML files (e.g., `error_page 404 /errors/404.html; error_page 500 /errors/500.html;`). Ensure the paths are correct relative to the server's root or define them as absolute paths.
5. **Set the maximum allowed size for client request bodies.**
    
    - **Status:** `maxbodysize=5242880` is present.
    - **To Do:** This seems correctly placed at the server level. No changes needed here. (Note: `limitRequestLine` is also good, but not explicitly requested for body size).
6. **Set up routes with one or multiple of the following rules/configurations:**
    
    - **Define a list of accepted HTTP methods for the route.**
        
        - **Status:** `methods=GET/POST/DELETE` is global.
        - **To Do:** Define `methods` (e.g., `allow_methods GET POST;` or similar syntax) within _each `location` block_ to control allowed methods per path.
    - **Define an HTTP redirect.**
        
        - **Status:** `redir` and `config` blocks hint at redirects but are not properly structured for per-route redirects.
        - **To Do:** Add a `location` block for the path you want to redirect, and use a `return` or `redirect` directive (e.g., `location /old-url { return 301 /new-url; }`).
    - **Define a directory or file where the requested file should be located.**
        
        - **Status:** `root=www/` is global. `dir` block attempts to define sub-roots, but not linked to URL paths.
        - **To Do:** Use the `root` or `alias` directive within `location` blocks to map specific URL paths to specific file system directories (e.g., `location /files/ { root www/files; }`). This ensures `/files/image.png` maps to `www/files/image.png`.
    - **Enable or disable directory listing.**
        
        - **Status:** Missing.
        - **To Do:** Add an `autoindex` directive (e.g., `autoindex on;` or `autoindex off;`) within `location` blocks where you want to control directory listing.
    - **Set a default file to serve when the request is for a directory.**
        
        - **Status:** `index=www/html/index.html` is global.
        - **To Do:** Use the `index` directive within `location` blocks to specify the default file(s) for a directory (e.g., `location / { index index.html index.htm; }`). This can be different for different directories.
    - **Execute CGI based on certain file extension (for example .php).**
        
        - **Status:** `filetypes` and `cgi` hints are present.
        - **To Do:**
            - Create a `location` block specifically for CGI (e.g., `location /cgi-bin/ { ... }` or `location ~ \.php$ { ... }`).
            - Inside this location, specify the path to the CGI interpreter (e.g., `cgi_pass /usr/bin/php-cgi;` or `cgi_pass /usr/bin/python3;`).
            - Ensure `methods GET POST;` are allowed in this location.
    - **Make it work with POST and GET methods.**
        
        - **Status:** Covered by `methods` but needs to be applied per route.
        - **To Do:** As mentioned, specify `methods` within relevant `location` blocks, especially for CGI and upload locations.
    - **Allow the route to accept uploaded files and configure where they should be saved.**
        
        - **Status:** `uploads=www/files/uploads` is present, but not linked to a specific route.
        - **To Do:** Create a `location` block for uploads (e.g., `location /upload_data { ... }`). Inside this block, define an `upload_store` directive pointing to `www/files/uploads` and ensure `POST` method is allowed.

# Final Structure to use for configuration file:

server {
    listen=8080,5555
    host=127.0.0.1
    server_name=webserv42
    root=www/webserv42/
    index=www/html/index.html
    maxbodysize=5242880
    limitRequestLine=4096
    error_page_404=/www/private/status_page.html
    location = / {  #root
        methods=GET/POST
    }
    location = /html {
        methods=GET
    }
    location = /files {
        root=www/html/webserv42/index.html
    }
    location = /files/uploads {
        methods=GET/POST/DELETE
    }
    location = /cgi-bin/ {
        methods=GET/POST
    }
}
server {
    listen=8080,9090
    host=127.0.0.2
    server_name=backupServer
    root=www/backupServer
    index=www/html/index.html
    maxbodysize=5242880
    limitRequestLine=2048
    error_page_404=/www/private/status_page.html
    location = / {  #root
        methods=GET/POST
    }
    location = /html {
        methods=GET
    }
    location = /files {
        root=www/html/backupServer/index.html
    }
    location = /files/uploads {
        methods=GET/POST/DELETE
    }
    location = /cgi-bin/ {
        methods=GET/POST
    }
}