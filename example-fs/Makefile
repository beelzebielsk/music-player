all : interface filesystem-test

interface : disk-interface.h disk-interface.c interface-test.c
	gcc disk-interface.c interface-test.c -o $@

filesystem-test : filesystem.h filesystem.c filesystem-test.c
	gcc filesystem.c filesystem-test.c -o $@

