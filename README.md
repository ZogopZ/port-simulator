# port-simulator

A multi process application that simulates a commercial port.

# Makefiles - How to compile

Open a terminal in the parent directory and type:

```sh
$ make
```

This will call all three makefiles in directories myport, port-master and vessel.

You can also use:

```sh
$ make clean
```

This removes all executable and object files.

# How to run

Open a terminal in the parent directory and type:

```sh
$./myport/myport -l myport/configfile
```

# myport process

This process is the "anchor" of the port and is responsible for the 
following tasks:

1.	Checks given arguments from the tty and reads data within the given configuration file. Three types of vessels (LARGE, MEDIUM, SMALL) **must** be present in the file. 
2.	Opens the logFile and records a part of its implementation.
3.	Allocates memory for the port corresponding to the data presented within the configuration file and initializes the ```sharedMemory_t``` type struct.
4.	Creates the shared memory segment and loads it with the ```sharedMemory_t``` type struct.
5.	```forks``` the child process port-master and hands over the shared memory key.
6.	Awaits the termination of port-master process.
7.	Destroys the created semaphores and detaches the shared memory from its address space.
8.	Marks the shared memory segment for destruction. The segment will actually get destroyed when the last child process detaches the shared memory from its address space.
9.	Frees any allocated memory.

# port-master process