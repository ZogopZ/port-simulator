#############################################################################
# Makefile for building: myport                                             #
#############################################################################

CC            = gcc
CFLAGS        = -g -Wall -W -pthread
DEL_FILE      = rm -f

OBJECTS       = main.o \
		cli_utils.o \
		checker.o \
		tools.o \
		port.o \
		printer.o

default: myport

myport: $(OBJECTS)
	$(CC) $(CFLAGS) -o myport $(OBJECTS)

main.o: main.c checker.h \
		port.h \
		cli_utils.h \
		tools.h \
		../port-master/printer.h
	$(CC) -c $(CFLAGS) -o main.o main.c

cli_utils.o: cli_utils.c cli_utils.h \
		checker.h \
		port.h
	$(CC) -c $(CFLAGS) -o cli_utils.o cli_utils.c

checker.o: checker.c port.h \
		checker.h
	$(CC) -c $(CFLAGS) -o checker.o checker.c

tools.o: tools.c tools.h \
		port.h \
		checker.h \
		cli_utils.h \
		../port-master/printer.h
	$(CC) -c $(CFLAGS) -o tools.o tools.c

port.o: port.c port.h \
		tools.h \
		checker.h
	$(CC) -c $(CFLAGS) -o port.o port.c

printer.o: ../port-master/printer.c checker.h \
		port.h \
		../port-master/printer.h
	$(CC) -c $(CFLAGS) -o printer.o ../port-master/printer.c

clean:
	-$(DEL_FILE) myport $(OBJECTS)
