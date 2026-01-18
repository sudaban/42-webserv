CPP = c++
SRC = src/main.cpp src/Utils.cpp src/Document.cpp src/Server.cpp src/ConfigParser.cpp
CFLAGS = -Wall -Werror -Wextra -std=c++98

NAME = webserv

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CPP) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re