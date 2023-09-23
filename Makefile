NAME = webserv
FLAGS = -Wall -Wextra -Werror -std=c++98
SRC_FOL = sources/
SRC_FIL = $(addprefix sources/,main.cpp Cluster.cpp Server.cpp Config.cpp \
			ConfigSetters.cpp ConfigGetters.cpp Request.cpp Response.cpp Error.cpp)
#OBJS = $(SRC_FIL:.cpp=.o)
OBJS = *.o
$(NAME): $(SRC_FIL)
		c++ $(FLAGS) -c $(SRC_FIL)
		c++ $(OBJS) -o $(NAME)

all: $(NAME)

clean:
		rm -rf $(OBJS)

fclean:	clean
		rm -rf $(NAME)
		rm -rf fake_site/file_should_exist_after

re:		fclean all

vai: all
	@./$(NAME)

vaii: all
	@./$(NAME)

dre:	fclean debug

.PHONY:	all clean fclean re vai vaii debug dre