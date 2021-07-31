CFLAGS=-Wall -ansi -pedantic -g
LDFLAGS=

CFILES=main.c list.c symtab.c memory.c err.c instruction.c assembly_line.c str_utils.c line_queue.c
OBJS:=$(CFILES:%.c=%.o)


a.out:$(OBJS)
	${CC} $(OBJS) ${LDFLAGS} -o $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<


clean:
	rm -f a.out
	rm -f -- *.o

.PHONY: clean
