#Program name
NAME = webserv

#Compiler
CC = c++

#Flags
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

#Cleanup
RM = rm -rf

#Source files and objects
SRCS = main.cpp src/Server.cpp

OBJS = $(patsubst %.cpp, %.o, $(SRCS))

#Targets
all: $(NAME)

#Link object files into target executable
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

#Object file generation
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

#Clean up
clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

#Phony targets
.PHONY: all clean fclean re