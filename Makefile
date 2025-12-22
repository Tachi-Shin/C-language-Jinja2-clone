CC = gcc
CFLAGS = -Wall -Wextra

all: server

server: Report5_html_server.o server.o router.o template.o session.o utils.o
	$(CC) $(CFLAGS) -o server Report5_html_server.o server.o router.o template.o session.o utils.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o server