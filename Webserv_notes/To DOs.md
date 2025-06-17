
- [x] make logic work with many servers
 - [x] adjust config file --> Nginx reference
 - [ ] make routing to index dynamic, according to location block indeces
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

Chris:
- [x] creating the mandatory folders if there is a new server
- [x] create top map for the dir maps. with path as key and map as value == Should work, needs recheck tommorow
- [ ] check autoindex, if on, provide a list with the files inside requested directory
	- [ ] response with file links and code 301 for redirecting

Henri:
- [x] change host to iphost for config logic
- [x] check for range of host
- [x] port range
- [x] upload dir dynamic
- [x] maxbodysize range check
- [x] default 404 error page
- [ ] query string? 
- [x] separate css stylesheet test
- [ ] script types check 
- [x] method check 

Steph:


(EDGE) CASES TO FIX:
- [ ] curl command in terminal: curl -X POST http://localhost:8080/html -F "file=@wetter.html" ouputs send error on fd x



