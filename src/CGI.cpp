#include "../incl/Response.hpp"
#include "../incl/Server.hpp"
#include <sys/wait.h>

// void Response::cgiExecuter(std::string path, const std::string &query)
// {
// 	int inPipe[2];
// 	int outPipe[2];

// 	std::string method = this->_request->getMethod();
// 	std::ifstream file(path.c_str());
// 	if (!file)
// 	{
// 		std::cerr << "Requested file open error!\n";
// 		handleERROR(404);
// 		return;
// 	}
// 	if (pipe(inPipe) == -1 || pipe(outPipe) == -1)
// 	{
// 		std::cerr << "ERROR: Pipe fail\n";
// 		this->handleERROR(500);
// 		return;
// 	}

// 	pid_t pid = fork();
// 	if (pid < 0)
// 	{
// 		std::cerr << "ERROR: Fork fail\n";
// 		this->handleERROR(500);
// 		return;
// 	}

// 	if (pid == 0)	//child
// 	{
// 		std::string methodSTR = "REQUEST_METHOD=" + method;
// 		std::string querySTR = "QUERY_STRING=" + query;
// 		std::string contentType = "CONTENT_TYPE=" + _request->getHeader("Content-Type");
// 		std::string contentLength = "CONTENT_LENGTH=" + _request->getHeader("Content-Length");
		
// 		char *env[] = {
// 			const_cast<char *>(methodSTR.c_str()),
// 			const_cast<char *>(querySTR.c_str()),
// 			const_cast<char *>(contentType.c_str()),
// 			const_cast<char *>(contentLength.c_str()),
// 			NULL
// 		};

// 		dup2(inPipe[0], STDIN_FILENO);	
// 		dup2(outPipe[1], STDOUT_FILENO);

// 		close(inPipe[0]);
// 		close(inPipe[1]);
// 		close(outPipe[0]);
// 		close(outPipe[1]);

// 		char resolved_path[PATH_MAX]; //check the directories
// 		if (realpath(path.c_str(), resolved_path) == NULL)
// 		{
// 			// perror("ERROR: realpath (child) fail!");
// 			std::cerr << "ERROR: realpath (child) fail!\n";
// 			// this->handleERROR(404);
// 			exit(EXIT_FAILURE);	// to close the child
// 		}

// 		char *av[] = {resolved_path, NULL};
// 		execve(resolved_path, av, env);
// 		// perror("execve");
// 		std::cerr << "ERROR: execve fail!\n";
// 		// this->handleERROR(500);
// 		exit(EXIT_FAILURE);
// 	}
		
// 	close(inPipe[0]);
// 	if (method == "POST")
// 	{
// 		std::string body = this->_request->getBody();
// 			std::cerr << "DEBUG: POST body length: " << body.size() << std::endl;
// 		if (body.empty())
//    				std::cerr << "WARNING: POST body is empty!" << std::endl;
// 			ssize_t written = write(inPipe[1], body.c_str(), body.size());
// 			std::cerr << "DEBUG: Written bytes to CGI stdin: " << written << std::endl;
// 		// write content from the post request to the input for read
// 		//write(inPipe[1], this->_request->getBody().c_str(), this->_request->getBody().length());
// 	}
// 	close(inPipe[1]);
// 	close(outPipe[1]);
// 	char buffer[1024];
// 	std::string output;
// 	ssize_t n;
// 	while ((n = read(outPipe[0], buffer, sizeof(buffer))) > 0)
// 		output.append(buffer, n);
// 	close(outPipe[0]);

// 	int status;
// 	waitpid(pid, &status, 0);
// 	if (WIFEXITED(status))
// 	{
// 		int exit_status = WEXITSTATUS(status);
// 		if (exit_status == 0)
// 		{
// 			parseCGIOutput(output);
// 			setCode(200);
// 		}
// 		else
// 		{
// 			std::cerr << "CGI script exited with status: " << exit_status << std::endl;
// 			this->handleERROR(500);
// 		}
// 	}
// }


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
		std::cerr << "ERROR: Pipe creation failed\n";
		handleERROR(500);
		return;
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		std::cerr << "ERROR: Fork failed\n";
		handleERROR(500);
		return;
	}

	if (pid == 0)	//child
	{
		std::string methodSTR      = "REQUEST_METHOD=" + method;
		std::string querySTR       = "QUERY_STRING=" + query;
		std::string contentTypeSTR = "CONTENT_TYPE=" + _request->getHeader("Content-Type");
		std::string contentLenSTR  = "CONTENT_LENGTH=" + _request->getHeader("Content-Length");

		char *env[] = {
			const_cast<char *>(methodSTR.c_str()),
			const_cast<char *>(querySTR.c_str()),
			const_cast<char *>(contentTypeSTR.c_str()),
			const_cast<char *>(contentLenSTR.c_str()),
			NULL
		};

		dup2(inPipe[0], STDIN_FILENO);
		dup2(outPipe[1], STDOUT_FILENO);

		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);

		char resolved_path[PATH_MAX];
		if (realpath(path.c_str(), resolved_path) == NULL)
		{
			std::cerr << "ERROR: realpath failed!\n";
			exit(EXIT_FAILURE);
		}

		char *argv[] = {resolved_path, NULL};
		execve(resolved_path, argv, env);

		// execve failed
		std::cerr << "ERROR: execve failed!\n";
		exit(EXIT_FAILURE);
	}

	// parent
	close(inPipe[0]);   // We only write to inPipe
	close(outPipe[1]);  // We only read from outPipe

	// Send POST data to CGI script (if needed)
	if (method == "POST")
	{
		std::string body = this->_request->getBody();
		if (!body.empty())
		{
			ssize_t written = write(inPipe[1], body.c_str(), body.size());
			std::cerr << "DEBUG: Written to CGI stdin: " << written << " bytes\n";
		}
	}
	close(inPipe[1]); // EOF to child stdin

	// wait for child with timeout
	time_t start = time(NULL);
	int status = 0;

	while (true)
	{
		int ret = waitpid(pid, &status, WNOHANG);

		if (ret == 0)
		{
			if (time(NULL) - start > TIMEOUT_SEC)	// defined in Server.hpp
			{
				std::cerr << "CGI Timeout. Killing child...\n";
				kill(pid, SIGKILL);
				waitpid(pid, &status, 0); // reap
				handleERROR(408);
				close(outPipe[0]);
				return;
			}
			// time_t currentTime = time(NULL);
			// std::cerr << "Time: " << currentTime << '\n'; 
			usleep(1000); // 1ms
			continue;
		}
		else if (ret == -1)
		{
			std::cerr << "waitpid() failed\n";
			handleERROR(500);
			close(outPipe[0]);
			return;
		}
		else	// Child exited
			break;
	}

	ssize_t n;
	char buffer[1024];
	std::string output;

	while ((n = read(outPipe[0], buffer, sizeof(buffer))) > 0)
		output.append(buffer, n);
	close(outPipe[0]);

	// Handle child exit status
	if (WIFEXITED(status))
	{
		int exitStatus = WEXITSTATUS(status);
		if (exitStatus == 0)
		{
			parseCGIOutput(output);
			setCode(200);
		}
		else
		{
			std::cerr << "CGI script exited with status: " << exitStatus << "\n";
			handleERROR(500);
		}
	}
	else
	{
		std::cerr << "CGI script did not exit cleanly\n";
		handleERROR(500);
	}

}


void Response::parseCGIOutput(const std::string &output)
{
	std::istringstream stream(output);
	std::string line;

	while (std::getline(stream, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r') 		// CGI often uses \r\n so remove \r if present
			line.erase(line.size() - 1);
		if (line.empty())
			break;

		size_t sep = line.find(": ");
		if (sep != std::string::npos)
		{
			std::string key = line.substr(0, sep);
			std::string value = line.substr(sep + 2);
			_headers[key] = value;
		}
	}
	std::string body((std::istreambuf_iterator<char>(stream)),
						std::istreambuf_iterator<char>());  //using fancy iterator to read the rest of the body
	std::cout << "CGI body: " << body << std::endl;
	_body = body;
	int bodyLen = _body.size();
	this->_headers["Content-Length"] = intToString(bodyLen);
}
