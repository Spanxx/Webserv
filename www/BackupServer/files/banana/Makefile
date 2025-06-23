CXX = c++
CXXFLAGS = -Wall -Wextra -Werror --std=c++98 -g
SRCS = main.cpp
OBJS = $(addprefix .obj/, $(SRCS:.cpp=.o))
OBJDIR = .obj
NAME = Iter

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

.obj/%.o : %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)
clean:
	$(RM) -r $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re

