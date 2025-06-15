📆 2025-06-05 🕓 10:45
Tags: #nginx #testing #nginx 

---
# Sources:
[Install guide_DigitalOcean](https://www.digitalocean.com/community/tutorials/how-to-install-nginx-on-ubuntu-20-04-de)

# Installation

Nginx can be installed directly, in a VM or Docker Container

**following steps is for installing directly**
## Installing wit apt-packagemanager

```
sudo apt update
sudo apt install nginx
```

## Firewall config

Allowing Nginx in UFW (only for testing from outside, not needed for localhost)

Showing the services that can be allowed in the UFW
`sudo ufw app list`

``` title:Output
OutputAvailable applications:
  Nginx Full
  Nginx HTTP
  Nginx HTTPS
  OpenSSH
```


```
sudo ufw allow 'Nginx HTTP
sudo ufw status
```

``` title:Output
Status: active

To                         Action      From
--                         ------      ----
OpenSSH                    ALLOW       Anywhere                  
Nginx HTTP                 ALLOW       Anywhere                  
OpenSSH (v6)               ALLOW       Anywhere (v6)             
Nginx HTTP (v6)            ALLOW       Anywhere (v6)
```
## Test if Nginx is runnig

`systemctl status nginx`

```title: Output
nginx.service - A high performance web server and a reverse proxy server
   Loaded: loaded (/lib/systemd/system/nginx.service; enabled; vendor preset: enabled)
   Active: active (running) since Fri 2020-04-20 16:08:19 UTC; 3 days ago
     Docs: man:nginx(8)
 Main PID: 2369 (nginx)
    Tasks: 2 (limit: 1153)
   Memory: 3.5M
   CGroup: /system.slice/nginx.service
           ├─2369 nginx: master process /usr/sbin/nginx -g daemon on; master_process on;
           └─2380 nginx: worker process
```

## Manage Nginx-processes

| usecase                                                      | command                        |
| ------------------------------------------------------------ | ------------------------------ |
| Stop                                                         | `sudo systemctl stop nginx`    |
| Start                                                        | `sudo systemctl start nginx`   |
| Restart                                                      | `sudo systemctl restart nginx` |
| Reload (to use new config settings, without connection loss) | `sudo systemctl reload nginx`  |
| Disable autostart                                            | `sudo systemctl disable nginx` |
| Enable autostart                                             | `sudo systemctl enable nginx`  |
# Serverblocks & Config file

## Creating directory for domain

`sudo mkdir -p /var/www/your_domain/html`

`sudo chown -R $USER:$USER /var/www/your_domain/html`

`sudo chmod -R 755 /var/www/your_domain`

`nano /var/www/your_domain/html/index.html`

## Adding ServerBlock for serving content from domain

- instead changing the standard config file, we could create an own for each domain

`sudo nano /etc/nginx/sites-available/your_domain`

``` title:"example Serverblock"
server {
        listen 80;
        listen [::]:80;

        root /var/www/your_domain/html;
        index index.html index.htm index.nginx-debian.html;

        server_name your_domain www.your_domain;

        location / {
                try_files $uri $uri/ =404;
        }
}
```

## Activating the new file

- we connect the new page with the sites-enabled directory
	- nginx is reading this at start

`sudo ln -s /etc/nginx/sites-available/your_domain /etc/nginx/sites-enabled/`

To provide multiple domains and preventing a Hash-Bucket-Saveproblem we have to activate one line in the config file
- remove the # sign to activate line
`sudo ln -s /etc/nginx/sites-available/your_domain /etc/nginx/sites-enabled/`

``` title:"/etc/nginx/nginx.conf"
...
http {
    ...
    server_names_hash_bucket_size 64;
    ...
}
...
```

## Test config file

`sudo nginx -t`

``` title:Output
nginx: the configuration file /etc/nginx/nginx.conf syntax is ok
nginx: configuration file /etc/nginx/nginx.conf test is successful
```

--> When succesfull, restart nginx with `sudo systemctl restart nginx`


