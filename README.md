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
The port-master process is responsible for the following tasks:
1. Checks its given argument which is the key to the shared memory.
2. Initiliazes the signal handler for ```SIGUSR1 (10)``` and ```SIGUSR2 (12)```.
3. Opens logFile for appending.
4. Attaches the shared memory to its address space.
5. Initializes the semaphores to be used.
6. Creates the child process vessel-creator.
7. Awaits for ships using ```semA``` and serves them according to the following occurences:
        - Ships that just reached the approach lane.
        - Ships that are already in the port and want to leave.
        - Ships that tried to port but there wasn't any free space at the time.

    Ships that want to leave are given priority. The ship departure is achieved using ```semA```, ```semD``` and ```semE```. The port-master commands the exit maneuver and deletes the ship's data from the shared memory. Ships that are waiting at the approach lane are checked next. The semaphores used are ```semA``` and ```semC```. If the approaching ship can port (namely there is a free space) the port-master allows it to proceed. If there isn't any free space the port-master calculates how much time the ship must wait in the approach lane until it can ask to port again.

    ##### How to send ```SIGUSR1 (10)``` and ```SIGUSR2 (12)``` signals
    Open a terminal and type:
    ```sh
    $kill -12 port-master-pid
    $kill -10 port-master-pid vessel-creator-pid
    ```
    The ```port-master-pid``` and ```vessel-creator-pid``` can be found in the tty while the application is running. When the first command is executed the port-master process changes "speed" using the ```sleep``` system calls. Speeds of 0.5, 1, 2, 3 seconds are supported. The second command is used to flag the port-master and the vessel-creator processes for termination. When the port-master receives the ```SIGUSR1``` it completes the last entering maneuver and prevents other ships from entering. Then it waits for ships to complete their tasks.
8. When the last vessel exits the port and after the vessel-creator process terminates the port-master detaches the shared memory and frees any allocated memory.

# vessel, vessel-creator process
The vessel-creator process is port-master's child and is responsible for random vessel creation at regular intervals (2 seconds).

The vessel process:
1. Checks its arguments and attaches the shared memory at its address space.
2. Using the ```semB``` awaits for either docking or departing from the port. Each time a vessel locks the ```semB``` semaphore it writes its data (type of vessel, mooring position to pay, mooring duration and maneuver duration) in the shared memory.
3. Waits for port-master's "OK" to start its entering maneuver or goes back to the approach lane and waits for a specified time.
4. When a vessel finishes its tasks within the port it exits the port by using semaphores ```semA```, ```semD``` and ```semE``` and detaches the shared memory from its address space.
5. Frees any allocated memory and terminates.

The vessel-creator process stops generating vessels when the ```SIGUSR1``` signal is received. At this point it waits for its children (vessels) to terminate and detaches the shared memory. Lastly it frees any allocated memory and terminates.

# Valgrind - Check, Shared Memory - Check
The application was checked for memory leaks using the tool **Valgrind** and is completely free from such errors. To run the application using Valgrind open a terminal in the parent directory and type:
```sh
$valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes ./myport/./myport -l myport/configfile
```
To stop the application use:
```sh
$kill -10 port-master-pid vessel-creator-pid
```
After termination the application was also checked for correct shared memory deallocation using:
```sh
$ipcs -mi 5668888
```
Where 5668888 is the shared memory id. The command's result was:
```sh
ipcs: id 5668888 not found.
```
