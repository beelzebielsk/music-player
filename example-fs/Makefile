headers := $(wildcard *.h)
sources := $(wildcard *.c)
all : interface filesystem-test tags

interface : disk-interface.h disk-interface.c interface-test.c
	gcc $(CC) disk-interface.c interface-test.c -o $@

# filesystem.h requires m library. Not sure what that is, but I
# imagine it's a math library.
filesystem-test : filesystem.h filesystem.c filesystem-test.c disk-interface.c disk-interface.h
	gcc $(CC) -lm filesystem.c filesystem-test.c disk-interface.c -o $@

tags : $(headers) $(sources)
	ctags $^
