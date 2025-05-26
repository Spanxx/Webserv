#include "../incl/Response.hpp"
#include <sys/wait.h>

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
	if ((method == "POST" && pipe(inPipe) == -1) || pipe(outPipe) == -1)
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

	if (pid == 0)	//child
	{
		std::string methodSTR = "REQUEST_METHOD=" + method;
		std::string querySTR = "QUERY_STRING=" + query;
		
		char *env[] = {
			const_cast<char *>(methodSTR.c_str()),
			const_cast<char *>(querySTR.c_str()),
			NULL
		};

		if (method == "POST")
		{
			dup2(inPipe[0], STDIN_FILENO);	// child reads from inPipe
			close(inPipe[0]);
			close(inPipe[1]);
		}
		dup2(outPipe[1], STDOUT_FILENO);	// child reads from outPipe

		// close(inPipe[0]);
		// close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);

		char resolved_path[PATH_MAX]; //check the directories
		if (realpath(path.c_str(), resolved_path) == NULL)
		{
			// perror("ERROR: realpath (child) fail!");
			std::cerr << "ERROR: realpath (child) fail!\n";
			// this->handleERROR(404);
			exit(EXIT_FAILURE);	// to close the child
		}

		char *av[] = {resolved_path, NULL};
		execve(resolved_path, av, env);
		// perror("execve");
		std::cerr << "ERROR: execve fail!\n";
		// this->handleERROR(500);
		exit(EXIT_FAILURE);
	}
	else	//parent
	{
		if (method == "POST")
		{
			close(inPipe[0]);
			// write content from the post request to the input for read
			write(inPipe[1], this->_request->getBody().c_str(), this->_request->getBody().length());
			close(inPipe[1]);
		}
		// else
		// {
		// 	close(inPipe[0]);
		// 	close(inPipe[1]);
		// }

		close(outPipe[1]);

		char buffer[1024];
		std::string output;
		ssize_t n;

		while ((n = read(outPipe[0], buffer, sizeof(buffer))) > 0)
			output.append(buffer, n);

		close(outPipe[0]);
		int status;
		waitpid(pid, &status, 0);
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
