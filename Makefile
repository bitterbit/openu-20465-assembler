CFLAGS=-Wall -ansi -pedantic
LDFLAGS=

CFILES=main.c list.c symtab.c memory.c
OBJS:=$(CFILES:%.c=%.o)


a.out:$(OBJS)
	${CC} $(OBJS) ${LDFLAGS} -o $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<


clean:
	rm -f a.out
	rm -f -- *.o

.PHONY: clean
