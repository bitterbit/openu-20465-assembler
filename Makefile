CFLAGS=-Wall -ansi -pedantic -g
LDFLAGS=-lm

SRC_DIR=.
CFILES=main.c list.c symtab.c memory.c err.c instruction.c assembly_line.c str_utils.c line_queue.c bit_utils.c object_file.c first_pass.c second_pass.c
OBJS:=$(CFILES:%.c=%.o)


assembler_20465:$(OBJS)
	${CC} $(OBJS) ${LDFLAGS} -o $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<


clean:
	rm -f assembler_20465
	rm -f -- *.o

.PHONY: clean

check-style:
	@for src in $(CFILES) ; do \
		var=`clang-format "$(SRC_DIR)/$$src" | diff "$(SRC_DIR)/$$src" - | wc -l` ; \
		if [ $$var -ne 0 ] ; then \
			echo "$$src does not respect the coding style (diff: $$var lines)" ; \
			exit 1 ; \
		fi ; \
	done
	@echo "Style check passed"

style:
	@for src in $(CFILES) ; do \
		echo "Formatting $$src..." ; \
		clang-format -i "$(SRC_DIR)/$$src" -style=file; \
	done
	@echo "Done"
