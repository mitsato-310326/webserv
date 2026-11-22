NAME = webserv

SRC = src/HttpRequestsParseFirstLine.cpp \
	  src/HttpRequestsTest.cpp \
	  src/TestScript.cpp \
	  src/StringTest.cpp \
	  src/StringUtils.cpp

OBJS = ${SRC:.cpp=.o}

INCLUDE = -I include

CC = c++
RM = rm -f
CPPFLAGS = -Wall -Wextra -Werror -pedantic -std=c++98 -g

.cpp.o:
		${CC} ${CPPFLAGS} ${INCLUDE} -c $< -o ${<:.cpp=.o}

$(NAME): ${OBJS}
		${CC} ${CPPFLAGS} ${INCLUDE} ${OBJS} -o ${NAME}

all:	${NAME}

clean:
		${RM} ${OBJS}

fclean:	clean
		${RM} ${NAME}

re: clean all

.PHONY: all clean re
