#Program name
NAME = webserv

#Compiler
CC = c++

#Flags
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

#Object files folder
OBJDIR = obj

#Cleanup
RM = rm -rf

#Source files and objects
SRCS = main.cpp \
		src/ConfigFile.cpp \
		src/Router.cpp \
		src/ServerDirStruct.cpp \
		src/Server.cpp src/ServerSocket.cpp \
		src/ServerLoop.cpp src/ServerUtils.cpp \
		src/Request.cpp src/RequestChecks.cpp src/RequestParsing.cpp\
		src/Response.cpp \
		src/CGI.cpp \
		src/Utils.cpp \
		src/Cluster.cpp

OBJ = $(patsubst %.cpp, $(OBJDIR)/%.o, $(SRCS))

#Targets
all: $(NAME)

#Link object files into target executable
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

#Object file generation
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

#Clean up
clean:
	$(RM) $(OBJ)
	$(RM) www/files/uploads

fclean: clean
	$(RM) $(NAME)

re: fclean all

#Phony targets
.PHONY: all clean fclean re
