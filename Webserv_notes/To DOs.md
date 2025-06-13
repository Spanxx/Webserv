
- [ ] make logic work with many servers
 - [ ] adjust config file --> Nginx reference
- [ ] inside config file:
	- [ ] set up server names
	- [ ] set up routes with different configurations
	- [ ] set up default error pages (what does this mean?)
	- [ ] define location blocks + implement autoindex on/off
	- [ ] list available directories or not (is this autoindex?)
	- [ ] configure where uploaded files should be saved (+adjust makefile rule to delete that specific file, if possible?)
	- [x] set max size of request body (+ implement inside read request function)
		- [ ] double check if correct
- [x] handle chunked requests
- [ ] handle chunked responses ?
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
- [ ] upload dir dynamic
- [ ] index dynamic? 
- [x] maxbodysize range check
- [ ] default 404 error page
- [ ] query string? 
- [ ] script types check 
- [ ] method check 

Steph:




QUESTIONS COLLECTION: 
- should we support folder style requests like "localhost:8080/files" and then serve an index? or is this already related to autoindex
- should we accept files that have no extension (so no dot)? or is "checkRequestedFiletype()" already commented out for that reason 