#############################################################################
# Makefile for building: port-master                                        #
#############################################################################


CC            = gcc
CFLAGS        = -g -Wall -W -pthread
DEL_FILE      = rm -f

OBJECTS       = main.o \
		checker.o \
		cli_utils.o \
		port.o \
		tools.o \
		printer.o

default: port-master

port-master: $(OBJECTS)
	$(CC) $(CFLAGS) -o port-master $(OBJECTS)

main.o: main.c tools.h \
		../myport/port.h \
		../myport/checker.h \
		../myport/cli_utils.h
	$(CC) -c $(CFLAGS) -o main.o main.c

checker.o: ../myport/checker.c ../myport/port.h \
		../myport/checker.h
	$(CC) -c $(CFLAGS) -o checker.o ../myport/checker.c

cli_utils.o: ../myport/cli_utils.c ../myport/cli_utils.h \
		../myport/checker.h \
		../myport/port.h
	$(CC) -c $(CFLAGS) -o cli_utils.o ../myport/cli_utils.c

port.o: ../myport/port.c ../myport/port.h \
		tools.h \
		../myport/checker.h
	$(CC) -c $(CFLAGS) -o port.o ../myport/port.c

tools.o: tools.c tools.h \
		../myport/port.h \
		../myport/checker.h \
		../myport/cli_utils.h \
		printer.h
	$(CC) -c $(CFLAGS) -o tools.o tools.c

printer.o: printer.c ../myport/checker.h \
		../myport/port.h \
		printer.h
	$(CC) -c $(CFLAGS) -o printer.o printer.c

clean:
	-$(DEL_FILE) port-master $(OBJECTS)
