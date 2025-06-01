📆 2025-05-22 🕓 15:54
Tags: #todo 

---

- [ ] location blocks

- [ ] adding cgi stuff
	more scripts
	- timeout inside the script, child should be killed?

- [ ] Chunking for post
	needed for get?
		only for response

sscanf???
check headers function in request


check method is needed

Timeout if child takes to long
	kill child

Chunking?

adding exceptions

Check Subject to handle edgecases.

Look up command for simulating packet loss in POST (TCP is handling it???)


```cpp
  

void Response::cgiExecuter(std::string path, const std::string &query)
{
int inPipe[2];
int outPipe[2];

std::string method = this->_request->getMethod();
std::ifstream file(path.c_str());
if (!file)
{
	std::cerr << "Requested file open error!\n";
	handleERROR(404);
	return;
}

if (pipe(inPipe) == -1 || pipe(outPipe) == -1)
{
std::cerr << "ERROR: Pipe fail\n";

this->handleERROR(500);

return;

}

  

pid_t pid = fork();

if (pid < 0)

{

std::cerr << "ERROR: Fork fail\n";

this->handleERROR(500);

return;

}

  

if (pid == 0) //child

{

std::string methodSTR = "REQUEST_METHOD=" + method;

std::string querySTR = "QUERY_STRING=" + query;

std::string contentType = "CONTENT_TYPE=" + _request->getHeader("Content-Type");

std::string contentLength = "CONTENT_LENGTH=" + _request->getHeader("Content-Length");

char *env[] = {

const_cast<char *>(methodSTR.c_str()),
const_cast<char *>(querySTR.c_str()),
const_cast<char *>(contentType.c_str()),
const_cast<char *>(contentLength.c_str()),
NULL
};
dup2(inPipe[0], STDIN_FILENO);
dup2(outPipe[1], STDOUT_FILENO);

close(inPipe[0]);
close(inPipe[1]);
close(outPipe[0]);
close(outPipe[1]);

char resolved_path[PATH_MAX]; //check the directories
if (realpath(path.c_str(), resolved_path) == NULL)
{
	// perror("ERROR: realpath (child) fail!");
	std::cerr << "ERROR: realpath (child) fail!\n";
	// this->handleERROR(404);
	exit(EXIT_FAILURE); // to close the child
	}
	
	char *av[] = {resolved_path, NULL};
	execve(resolved_path, av, env);
	std::cerr << "ERROR: execve fail!\n";
	// this->handleERROR(500);
	exit(EXIT_FAILURE);
}

  

close(inPipe[0]);

while (1)

{

time_t start = time(NULL);

int status;

int result = waitpid(pid, &status, WNOHANG);

  

if (result == 0)

{

if (time(NULL) - start > TIMEOUT_SEC)

{

kill(pid, SIGKILL);

waitpid(pid, &status, 0);

std::cerr << "CGI Timeout\n";

this->handleERROR(408);

}

usleep(10000);

continue;

}

else if (result == pid)

{

if (method == "POST")

{

std::string body = this->_request->getBody();

std::cerr << "DEBUG: POST body length: " << body.size() << std::endl;

if (body.empty())

std::cerr << "WARNING: POST body is empty!" << std::endl;

ssize_t written = write(inPipe[1], body.c_str(), body.size());

std::cerr << "DEBUG: Written bytes to CGI stdin: " << written << std::endl;

// write content from the post request to the input for read

//write(inPipe[1], this->_request->getBody().c_str(), this->_request->getBody().length());

}

  

close(inPipe[1]);

close(outPipe[1]);

  

char buffer[1024];

std::string output;

ssize_t n;

  

while ((n = read(outPipe[0], buffer, sizeof(buffer))) > 0)

output.append(buffer, n);

close(outPipe[0]);

  

// int status;

// waitpid(pid, &status, 0);

if (WIFEXITED(status))

{

int exit_status = WEXITSTATUS(status);

if (exit_status == 0)

{

parseCGIOutput(output);

setCode(200);

}

else

{

std::cerr << "CGI script exited with status: " << exit_status << std::endl;

this->handleERROR(500);

}

}

}

}

}
```