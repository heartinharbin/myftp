OBJS:=$(wildcard *.c)
server:$(OBJS)
	gcc $^ -o $@ -I ../include  -lmysqlclient -lpthread
clean:
	rm server

