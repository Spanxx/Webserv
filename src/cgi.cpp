#include "../incl/Response.hpp"
#include <sys/wait.h>

void Response::cgiExecuter(const std::string &path, const std::string &query)
{
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
	{
		std::cerr << "Pipe fail\n";
		setCode(500);
		handleERROR();
		return;
	}
	pid_t pid = fork();
	if (pid < 0)
	{
		std::cerr << "Fork fail\n";
		setCode(500);
		handleERROR();
		return;
	}
	if (pid == 0)	//child
	{
		std::string methodSTR = "REQUEST_METHOD=" + _request->getMethod();
		std::string querySTR = "QUERY_STRING=" + query;
		char *env[] = {
			const_cast<char *>(methodSTR.c_str()),
			const_cast<char *>(querySTR.c_str()),
			NULL
		};
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		char resolved_path[PATH_MAX]; //check the directories
		if (realpath(path.c_str(), resolved_path) == NULL)
		{
			perror("realpath");
			setCode(404); //check if this is the right code
			handleERROR();
			return;
		}
		char *av[] = {resolved_path, NULL};
		execve(resolved_path, av, env);
		perror("execve");
		setCode(500); //check if this is the right code
		handleERROR();
		return;
	}
	else	//parent
	{
		close(pipe_fd[1]);
		char buffer[1024];
		std::string output;
		ssize_t n;

		while ((n = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
			output.append(buffer, n);
		close(pipe_fd[0]);
		waitpid(pid, NULL, 0);
		parseCGIOutput(output);
		setCode(200);
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
}
