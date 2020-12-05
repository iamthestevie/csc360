.phony all:
all: fat

fat:
	gcc -Wall p3help.c diskinfo.c -o diskinfo
	gcc -Wall p3help.c disklist.c -o disklist
	gcc -Wall p3help.c diskget.c -o diskget
	gcc -Wall p3help.c diskput.c -o diskput

.PHONY clean:
clean:
	-rm -rf *.o *.exe
