## Webserv project at 42 school
Contributors: Chris (@Spanxx), Henriette (@Henrizz) and Stephanie (@StephVibes)
<br>
<br>
## Project description
The objective of this project is to learn the rules and basics of HTTP and client-server communication. We are asked to build a server that can host various websites with individual databases and specifications determined in a configuration file (similar to NGINX). A special focus is on keeping the server non-blocking at all times, which we tested with siege, to ensure 100% availability even with many simultaneous requests. 
<br>
We handle the following aspects: 
- regular and chunked requests (a feature often used for streaming)
- GET, POST and DELETE methods
- login feature with user and cookie management 
- CGI scripts (using python and php)
- upload and deletion of files 
- html and css
- automatically updated image gallery with javascript
- custom error pages with the adequate error codes
- routing per specificied location blocks
- directory listing for directory listing (when no index page is specified in config file and autoindex is on)
<br>

## Usage
To run our program, first download the code, open a terminal, go inside the project directory and type: 'make'
Then type './webserv server.conf' in the terminal to launch the server with our default configuration. You can change the configuration by adding a different file as program argument or change the server.conf file directly.
You can access our default websites in the browser, by either requesting 'localhost:8080' or 'localhost:9090' (or the alternate host:port combination you would put in your own configuration) and then discover all features.
<br>

**! Plase note that the 'SAY HI' button on our websites is a php script configured for linux !**
<br>
<br>
If you want to use this script on mac, you have to change the php shebang of the hello.php script inside the cgi-bin folder to the location your php-cgi is at (to find out where this is, type 'which php-cgi' in the terminal) 


