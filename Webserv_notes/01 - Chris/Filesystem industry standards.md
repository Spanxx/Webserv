📆 2025-05-23 🕓 15:14
Tags: #server #files 

---

## 🗂️ General Linux Filesystem Hierarchy (FHS)

The Filesystem Hierarchy Standard (FHS) defines the directory structure and directory contents in Linux distributions.
### Common Directories:

|Directory|Purpose|
|---|---|
|`/`|Root directory of the filesystem.|
|`/bin`, `/sbin`|Essential command binaries and system binaries.|
|`/etc`|Configuration files.|
|`/var`|Variable data like logs, mail, databases.|
|`/usr`|User-installed software and libraries.|
|`/opt`|Optional software packages, especially third-party apps.|
|`/home`|User home directories.|
|`/tmp`|Temporary files.|
|`/srv`|Data for services provided by the server (e.g., `/srv/www` for web data).|

---
## 🌐 Web Server Directory Structure (e.g., Nginx, Apache)

### Example:

```php
/var/www/
├── html/                # Default site content
├── yoursite.com/
│   ├── public/          # Publicly accessible files (HTML, JS, images)
│   ├── logs/            # Site-specific logs
│   ├── config/          # Virtual host or app config
│   └── private/         # Non-public files (e.g., PHP includes)
```

Other common locations:
- Logs: `/var/log/nginx/`, `/var/log/apache2/`
- Configs: `/etc/nginx/`, `/etc/apache2/`

## 🧱 Application Server Structure (e.g., Python, Node.js apps)

### Node.js Example:

```
/srv/app/
├── src/                 # Source code
├── config/              # Environment configs
├── logs/                # App-specific logs
├── public/              # Static files
├── node_modules/
├── .env                 # Environment variables
└── package.json
```

---

## 📦 Containerized Applications (e.g., Docker)

Inside a Docker container or orchestrated service, structure matters less for consistency across servers, but is still structured clearly:

```swift
/app/
├── main.py
├── requirements.txt
├── Dockerfile
└── static/
```

