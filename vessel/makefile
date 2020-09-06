#############################################################################
# Makefile for building: vessel                                             #
#############################################################################

CC            = gcc
CFLAGS        = -g -Wall -W -pthread
DEL_FILE      = rm -f

OBJECTS       = main.o \
		checker.o \
		cli_utils.o \
		port.o \
		printer.o \
		tools.o

default: vessel

vessel: $(OBJECTS)
	$(CC) $(CFLAGS) -o vessel $(OBJECTS)

main.o: main.c ../myport/checker.h \
		../myport/port.h \
		../myport/cli_utils.h \
		../port-master/printer.h \
		tools.h
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

printer.o: ../port-master/printer.c ../myport/checker.h \
		../myport/port.h \
		../port-master/printer.h
	$(CC) -c $(CFLAGS) -o printer.o ../port-master/printer.c

tools.o: tools.c ../myport/port.h \
		../myport/cli_utils.h \
		tools.h
	$(CC) -c $(CFLAGS) -o tools.o tools.c

clean:
	-$(DEL_FILE) vessel $(OBJECTS)
