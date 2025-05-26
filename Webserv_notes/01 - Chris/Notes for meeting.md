📆 2025-05-22 🕓 10:28
Tags: #CGI 

---

- add exceptions?

Name: cgiExecuter -> executor?

- line 11: is this not set in henriz create error page function?
	- same for line 20
- line 28: for what is the env array?
- line 37: PATH_MAX is not defined (`#include <limits.h>` was missing)
	- 4096 chars
- Scirpt output is not shown on the webpage?
	- realPath is showing the path from my computer root...is that correct? (line 38 cgiExe)


Browser is waiting for something after recieving response?

serverLoop
	POLLERR, POLLHUP, POLNVAL
		- add erase for response collector?

Read from connection
- line 105? it is taking the last time the from the loop?

Response collector?


# Todo
Directory structre check?
data -> internal
industry standard wise

location blocks

adding cgi stuff
	more scripts
	how is post working
Chunking for post
	needed for get?
		only for response


Branch from stephs branch

content type in header
change handleerror (handleERROR(int code))


