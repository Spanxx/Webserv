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
		src/Server.cpp src/Config.cpp

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

fclean: clean
	$(RM) $(NAME)

re: fclean all

#Phony targets
.PHONY: all clean fclean re