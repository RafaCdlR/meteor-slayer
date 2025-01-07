CC:=gcc

LDFLAGS += -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

EXES:=game
HEADERS:=resource_dir.h
COMP:=comprimido.zip

.PHONY: clean zip all

all: ${EXES}

${EXES}: %: *.c
	${CC} $^ ${LDFLAGS} -o $@


clean:
	${RM} ${EXES} ${OBJS}

clean-all: clean
	${RM} ${COMP}

zip: ${EXES}
	zip ${COMP} ${EXES:=.c} ${HEADERS} makefile ${EXES}
