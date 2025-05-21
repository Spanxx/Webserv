#include "../incl/Response.hpp"
#include <sys/wait.h>

std::string Response::cgiExecuter(const std::string &path, const std::string &query)
{

	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
	{
		std::cerr << "Pipe fail\n";
		return make_status_page_string(042);
	}
	pid_t pid = fork();
	if (pid < 0)
	{
		std::cerr << "Fork fail\n";
		return make_status_page_string(042);
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
		char *av[] = {(char *)path.c_str(), NULL};
		execve(path.c_str(), av, env);
		perror("execve");
		exit(1);
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
		return output;
	}
}
