NAME = webserv
SERVER = webserv-server

SRC = src/HttpRequestsParser/ParseRequestLine.cpp \
	  src/HttpRequestsParser/ParseHost.cpp \
	  src/HttpRequestsParser/ParseContentLength.cpp \
	  src/HttpRequestsParser/ParseTransferEncoding.cpp \
	  src/HttpRequestsParser/ParseHeader.cpp \
	  src/HttpRequestsParser/ParseBody.cpp \
	  src/HttpRequestsParser/Parser.cpp \
	  src/HttpRequestsParser/ParseUtils.cpp \
	  src/HttpRequestsParser/Exceptions.cpp \
	  src/Test/HttpRequestsTest.cpp \
	  src/Test/TestScript.cpp \
	  src/StringUtils.cpp \
	  src/UrlDecode.cpp

OBJS = ${SRC:.cpp=.o}
SERVER_SRC = src/main.cpp src/listener.cpp src/event_loop.cpp \
             src/client_events.cpp src/client.cpp src/HttpRequestsParser/ParserMock.cpp \
			 src/HttpRequestsParser/ParseRequestLine.cpp src/HttpRequestsParser/ParseHost.cpp \
			 src/StringUtils.cpp \
	  src/UrlDecode.cpp
SERVER_OBJS = ${SERVER_SRC:.cpp=.o}
ALL_OBJS = $(sort ${OBJS} ${SERVER_OBJS})
DEPS = ${ALL_OBJS:.o=.d}

INCLUDE = -I inc

CC = c++
RM = rm -f
CPPFLAGS = -Wall -Wextra -Werror -pedantic -std=c++98 -g

.cpp.o:
		${CC} ${CPPFLAGS} ${INCLUDE} -MMD -MP -c $< -o ${<:.cpp=.o}

$(NAME): ${OBJS}
		${CC} ${CPPFLAGS} ${INCLUDE} ${OBJS} -o ${NAME}

all:	${NAME}

server: ${SERVER}

${SERVER}: ${SERVER_OBJS}
		${CC} ${CPPFLAGS} ${SERVER_OBJS} -o ${SERVER}

-include ${DEPS}

clean:
		${RM} ${ALL_OBJS} ${DEPS}

fclean:	clean
		${RM} ${NAME} ${SERVER}

re: clean all

.PHONY: all server clean fclean re
