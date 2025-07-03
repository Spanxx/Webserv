
- [x] make logic work with many servers
 - [x] adjust config file --> Nginx reference
 - [ ] make routing to index dynamic, according to location block indeces (+ root should route to server index?)
- [ ] inside config file:
	- [x] set up server names
	- [x] set up routes with different configurations
	- [x] set up default error pages (what does this mean?)
	- [ ] define location blocks + implement autoindex on/off
	- [ ] list available directories or not (is this autoindex?)
	- [x] configure where uploaded files should be saved
	- [x] set max size of request body (+ implement inside read request function)
		- [ ] double check if correct
- [x] handle chunked requests
- [ ] refactor logic with exceptions instead of return values
- [ ] check if we need to handle piped requests? --> adjust server loop
- [ ] check cgi with cgi tester --> does cgi routing work and miscellaneous scripts?
	- [ ] make cgi extension check seperate from other file type checks
- [ ] DELETE request make work with files other than images
- [ ] different style css / background color per server also for error files?

Chris:
- [x] creating the mandatory folders if there is a new server
- [x] create top map for the dir maps. with path as key and map as value == Should work, needs recheck tommorow
- [ ] check autoindex, if on, provide a list with the files inside requested directory
	- [ ] response with file links and code 301 for redirecting
- [ ] router umbauen: if path is directory + method is POST, then it's correct (check what case is for DELETE) 
- [ ] delete negative number check
- [ ] check timeout conditions for big files (how it works with child killing)
- [ ] make check for duplicate in host:port combination or port duplicates 

Henri:
- [x] change host to iphost for config logic
- [x] check for range of host
- [x] port range
- [x] upload dir dynamic
- [x] maxbodysize range check
- [x] query string use
- [x] separate css stylesheet test
- [ ] script types check 
- [x] method check 
- [x] upload directory assignment with upload_dir 
- [x] implement default error page variable (config file name)
- [x] make checks ob permission ok ist fuer cgi script 
- [ ] make index dynamic inside upload script (after index has been passed as variable by Chris)
- [ ] implement max height of uploaded images on index
- [ ] add number increment for filenames in POST request without CGI

Steph:






