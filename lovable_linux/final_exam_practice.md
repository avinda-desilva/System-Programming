# Angrave's 2019 Acme CS 241 Exam Prep		
## A.K.A. Preparing for the Final Exam & Beyond CS 241... 
#### avindad2
#### ajariw2
#### janaks2 
Some of the questions require research (wikibook; websearch; wikipedia). 
It is accepted to work together and discuss answers, but please make an honest attempt first! 
Be ready to discuss and clear confusions & misconceptions in your last discussion section.
The final will also include pthreads, fork-exec-wait questions and virtual memory address translation. 
Be awesome. Angrave.

## 1. C 


1.	What are the differences between a library call and a system call? Include an example of each.

System Calls are functions provided by the kernal. Example: open().

Library calls are functions within program libraries. Example: fopen().

2.	What is the `*` operator in C? What is the `&` operator? Give an example of each.

'*' is to dereference. So if you have a pointer where you are give a pointer to an integer: int* hello. To find the integer value of hello you must dereference it. *hello = 3 (in example).

'&' gives a reference to something. For example if you have an integer: int hello. &hello will give you a pointer to where hello is in the memory. 


3.	When is `strlen(s)` != `1+strlen(s+1)` ?

Example: 
if my first character is a null character. 
char* hello = "\0hahahhah\0";
This will give you a different result. 

4.	How are C strings represented in memory? What is the wrong with `malloc(strlen(s))` when copying strings?

C strings are represented in memory using consecutive bytes and terminating with a null character. It is wrong because every string needs to account for the null character so you must do malloc(strlen(s) + 1) instead. 

5.	Implement a truncation function `void trunc(char*s,size_t max)` to ensure strings are not too long with the following edge cases.
```
if (length < max)
    return;
    strcmp(trunc(s, max), s) == 0
else if (s is NULL)
    return;
    trunc(s, max) == NULL
else
    if(max == strlen(s)) {
        return;
    } else {
        s[max + 1] = '\0';
        return;
    }
    strlen(trunc(s, max)) <= max
    // i.e. char s[]="abcdefgh; trunc(s,3); s == "abc".
     
```


6.	Complete the following function to create a deep-copy on the heap of the argv array. Set the result pointer to point to your array. The only library calls you may use are malloc and memcpy. You may not use strdup.

    `void duplicate(char **argv, char ***result);` 

    void duplicate(char** argv, char***result) {
        int i = 0;

        while(argv + i != NULL) {
            i++;
        }

        char** argv2 = malloc((i-1)* sizeof(char*));
        int j = 0;
        while(j < i) {
            argv2[j] = malloc(strlen(argv[j] + 1));
            memcpy(argv2[j], argv[j], strlen(argv[j]) + 1);
            j++;
        }
        
        result = &argv2;
    }


7.	Write a program that reads a series of lines from `stdin` and prints them to `stdout` using `fgets` or `getline`. Your program should stop if a read error or end of file occurs. The last text line may not have a newline char.

while(1) {
    char* buffer = NULL;
    size_t cap = 0;
    ssize_t result = getline(&buffer, &cap, input);
    if (result == -1) {
        free(buffer);
        return "END_OF_FILE";
    }
    
    if(result > 0 && buffer[result-1] == '\n') { 
        buffer[result-1] = '\0';
    }
    printf("%s", buffer);
    if(result < 0 || result == 0) {
        break;
    }
    
}

## 2. Memory 

1.	Explain how a virtual address is converted into a physical address using a multi-level page table. You may use a concrete example e.g. a 64bit machine with 4KB pages. 

Find Index of top table and then get the value associated with the index of the root table. You go to the table associated with the value and then take the index of subtable to get the frame. 

2.	Explain Knuth's and the Buddy allocation scheme. Discuss internal & external Fragmentation.

Buddy Allocator: Segregated Allocator  = divides heap and handled by sub-allocators. Splits allocation into blocks of size 2^n times some base unit of bytes. If there is no free blocks of size 2^n then go to the next one so size 2^n+1 and keep going till you find space then take that block and split it. If adjacent are freed then coalesce the 2 blocks together.

Knuth Allocator: These are allocating systems that utilize boundary tags, which store the block's at the end of a block. So one can decide to free it or calesce after reading the boundary tags. 

Internal Fragmentation: Given block is larger than allocation size. 

External Fragmentation: May be that though we have enough total space in the heap, but it may be divided up in a way that continuous blocks of a certain size are not available. 

3.	What is the difference between the MMU and TLB? What is the purpose of each?

The MMU takes the virtual memory address and translates that into the physical memory address. On the other hand, the TLB is a cache of page table entries and its main purpose is to bypass the process of accessing the main memory every single time a virtual address is mapped.

4.	Assuming 4KB page tables what is the page number and offset for virtual address 0x12345678  ?

Binary form of Virtual Address = 00010010001101000101011001111000

Page Number = 00010010001101000101 --> includes page directory and page  table which then makes up the page number.

offset = 011001111000

5.	What is a page fault? When is it an error? When is it not an error?

Page Fault is when a running program tries t access some vertual memory in its adress space that is not mapped to physical memory.

Not an error: If there is no mapping yet for the page, but it is a valid address. 

Major: If mapping to the page is not in memory but on disk. (If it happens enough it becomes an error).

Error: When you write to a non-writable memory address or read to a non-readable memory address. 


6.	What is Spatial and Temporal Locality? Swapping? Swap file? Demand Paging?

Spatial Locality: Items referenced with nearby adresses referenced close together in time. 

Temporal Locality: Items referenced before will be referenced quickly again.

Demand paging: Only bring in page when needed. 

Swapping: Swapping a page into memory and taking another page out.

Swap File: A space on disk used as an extension of the virtual memory.

## 3. Processes and Threads 

1.	What resources are shared between threads in the same process?

Heap, stack, code, data, and files are all resources that are shared between threads in the same process.

2.	Explain the operating system actions required to perform a process context switch

Very Expensive. Save PC and User stack pointer of current process. Saving rest of registers, and other machine state. State of the next process from. 

3.	Explain the actions required to perform a thread context switch to a thread in the same process

Storing context of thread, Saving value of CPU registers of thread being switched out and restoring the CPU registers of new thread. 

4.	How can a process be orphaned? What does the process do about it?

Parent process has finished though it remains running. Special init process quickly adopts the orphan. 

5.	How do you create a process zombie?

A process zombie is created when a child process has finished its execution and then takes up a spot in the kernel process table for our process.

6.	Under what conditions will a multi-threaded process exit? (List at least 4)

Process calls exit directly.

One of threads calls exit. 

Returns from main().

Receives a termination signal.

## 4. Scheduling 
1.	Define arrival time, pre-emption, turnaround time, waiting time and response time in the context of scheduling algorithms. What is starvation?  Which scheduling policies have the possibility of resulting in starvation?

Arrival time is the time at which a process first arrvies at the ready and is ready to start executing. 

Preemption brings process back to the ready state. 

Turnaround time is total time from when the process arrives to when it ends

Wait time is the total time that a process is on the ready queue. 

Response time: is total time it takes from when the process arrives to when the CPU actually starts working on it. 

Starvation is when the low priority program is requesting for a system resource but are not able to execute because a higher priortiy program is utilizing that resource for an extended period. 

Scheduling policies resulting in starvation: Priority Scheduling, Shortest-job first 

2.	Which scheduling algorithm results the smallest average wait time?

Shortest Job First

3.	What scheduling algorithm has the longest average response time? shortest total wait time?

Longest Average Response time: First come first served.

Shortest Total Wait: Shortest Job First.

4.	Describe Round-Robin scheduling and its performance advantages and disadvantages.

Round-Robin schedules in order of their arrival in the ready queue. Moreover, after a small time step, a running process will be removed from the running state and then it will be placed back into the ready queue.

Advantages- Round-Robin ensures some notion of fairness.
Disadvantages - Round-Robin has large number of processes which results in a lot of switching.

5.	Describe the First Come First Serve (FCFS) scheduling algorithm. Explain how it leads to the convoy effect. 

First Come First Serve (FCFS) schedules processes in the order of their arrival.

Advantages - First Come First Serve (FCFS) is super simple and the ready queue is a FIFO (first in, first out) queue. No stravation if all processes guaranteed to terminate. Context switches infrequent when there are long-running processes.

Disadvantages - Simple algorithm and implementation. Context switches infrequent when there are long-running processes.

First Come First Serve (FCFS) leads to the convoy effect because the whole operating system slows down due to slow processes. Hence, we must wait for one to be done completely before moving on to the next which slows down our OS if we have slow processes.

6.	Describe the Pre-emptive and Non-preemptive SJF scheduling algorithms. 

Non-preemptive SJF - the processes all arrive at the start and the scheduler schedules the job with the shortest total CPU time.

Pre-emptive SJF - is like the Non-preemptive SJF but if a new job comes in with a shorter runtime than the total runtime of the current job, it is run instead. If it is equal, the algorithm can choose.

7.	How does the length of the time quantum affect Round-Robin scheduling? What is the problem if the quantum is too small? In the limit of large time slices Round Robin is identical to _____?

Quantum is how long each process runs before it switches to another process. So a small quantum means a lot more process switching. If too small then a lot of expensive process switching. Large time slices Round Robin will be equivalent to FCFS. 

8.	What reasons might cause a scheduler switch a process from the running to the ready state?

When there is an interrupt or if the algorithm swaps out the current running process. 

## 5. Synchronization and Deadlock

1.	Define circular wait, mutual exclusion, hold and wait, and no-preemption. How are these related to deadlock?

Circular Wait: Exists a cycle in Resource allocation graph. P1 waits for P2 who waits for p3 and p3 waits for p1. Constant Deadlock, since no process can move because they do not have the proper resource.

Hold and Wait: Once resource obtained, process keeps resource locked. If any other process needs that resource then deadlock will occur.

No Pre-emption: Nothing can force the process to give up a resource. If other processes need that resource then deadlock will occur. 

2.	What problem does the Banker's Algorithm solve?

It prevents livelock.

3.	What is the difference between Deadlock Prevention, Deadlock Detection and Deadlock Avoidance?

Deadlock prevention is that ensures that at least one at least one of deadlock necessary conditions will never occur. Deadlock Avoidance ensures system will not enter an unsafe state. Deadlock detection determines deadlock exists and identifies process and resources involved in deadlock.

4.	Sketch how to use condition-variable based barrier to ensure your main game loop does not start until the audio and graphic threads have initialized the hardware and are ready.

Use a barrier. Threads tdo the audio and graphics first. Once these threads finish a task they sleep. Then the last thread to finish wakes the rest up rest of them when they finish. So once all threads are finished then the rest are woken up by cond variable to do the main game loop.

5.	Implement a producer-consumer fixed sized array using condition variables and mutex lock.

Implementation: 

For Producer: Using Mutex lock until number of items is less than max. Then wakes up using cond variable and inserts in array when possible,

For Consumer: Using Consumer lock until number of items is greater than 0. Then wakes up using cond variable and removes in array when possible.

6.	Create an incorrect solution to the CSP for 2 processes that breaks: i) Mutual exclusion. ii) Bounded wait.

raise my flag
if your flag is raised, wait until my turn
// Do Critical Section stuff
turn = yourid
lower my flag


7.	Create a reader-writer implementation that suffers from a subtle problem. Explain your subtle bug.

void read() {
  while(writing) {/*spin*/}
  reading = 1
  // do read stuff
  reading = 0
}

void write() {
  while(reading || writing) {/*spin*/}
  writing = 1
  // do write stuff
  writing = 0
}

There is a race condition.

## 6. IPC and signals

1.	Write brief code to redirect future standard output to a file.

int fd = open("test.txt", O_WRONLY);
dup2(fd, 1);

2.	Write a brief code example that uses dup2 and fork to redirect a child process output to a pipe

pid_t child = fork(); 

if(child == 0) {
    dup2(pipe_d[0], 0);
    close(pipe_d[1]);
}

3.	Give an example of kernel generated signal. List 2 calls that can a process can use to generate a SIGUSR1.

SIGSEGV WILL GENERATE A kernel signal

kill -10 <pid>
kill(pid, SIGUSR1);
raise(SIGUSR1);

4.	What signals can be caught or ignored?

SIGINT, SIGQUIT - are signals that can be caught

SIGSTOP, SIGKILL - are signals that cannot be caught

IGN - is a signal that can be ignored

5.	What signals cannot be caught? What is signal disposition?

SIGSTOP, SIGKILL - are signals that cannot be caught

Signal disposition - is a per-process attribute that determines how a signal is handled after it is delivered.

6.	Write code that uses sigaction and a signal set to create a SIGALRM handler.

struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = close_server;
    if (sigaction(SIGALRM, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

7.	Why is it unsafe to call printf, and malloc inside a signal handler?

If printf was running and a signal was raised with printf would be called and the calls would be intertwined, outputting something undesirable. Same with malloc, with the memory to be allocated. 

## 7. Networking 

1.	Explain the purpose of `socket`, `bind`, `listen`, and `accept` functions

Socket - the purpose of the socket function is that socket creates a network socket and returns a descriptor that can be used with read and write. It is the network analog of open that opens a file stream for us to use that can communicate with the outisde world/network.

Bind - the purpose of the bind function is that bind associates an abstract socket with an actual network interface and port.

Listen - the purpose of the listen function is that listen specifies the queue size for the number of incoming, unhandled connection.

Accept - the purpose of the accept function is that accept waits for new connections.

Socket, Bind, Listen, and accept are the four system calls required to create a minimal TCP server.

2.	Write brief (single-threaded) code using `getaddrinfo` to create a UDP IPv4 server. Your server should print the contents of the packet or stream to standard out until an exclamation point "!" is read.

    int s;

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // INET for IPv4
    hints.ai_socktype =  SOCK_DGRAM;
    hints.ai_flags =  AI_PASSIVE;

    getaddrinfo(NULL, "300", &hints, &res);

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
        perror("bind()");
        exit(1);
    }
    struct sockaddr_storage addr;
    int addrlen = sizeof(addr);

    while(1){
        char buf[1];
        ssize_t byte_count = recvfrom(sockfd, buf, sizeof(buf), 0, &addr, &addrlen);
        
        if(buf[0] == '!') {
            break;
        }
        printf("%s", buf);
    }

3.	Write brief (single-threaded) code using `getaddrinfo` to create a TCP IPv4 server. Your server should print the contents of the packet or stream to standard out until an exclamation point "!" is read.

    int s;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, "1234", &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }

    if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("bind()");
        exit(1);
    }

    if (listen(sock_fd, 10) != 0) {
        perror("listen()");
        exit(1);
    }
    
    struct sockaddr_in *result_addr = (struct sockaddr_in *) result->ai_addr;
    printf("Listening on file descriptor %d, port %d\n", sock_fd, ntohs(result_addr->sin_port));

    printf("Waiting for connection...\n");
    int client_fd = accept(sock_fd, NULL, NULL);
    printf("Connection made: client_fd=%d\n", client_fd);

    while(1) {
        char buffer[2];
        int len = read(client_fd, buffer, sizeof (buffer) - 1);
        buffer[len] = '\0';
        if(buffer[0] == '!') {
            break;
        }
        printf("%s", buffer);
    }


4.	Explain the main differences between using `select` and `epoll`. What are edge- and level-triggered epoll modes?

Select returns total number of file descriptors that are ready. If none are ready by timeout it will return -. After select returns. The caller will beed to loop through the filedescriptors in readfds/writefds to see which are ready.

Epoll is more efficient. It will tell you which file descriptors are ready. It gives you a way to store a small amount of data with each descriptor. 

Epoll only returns when an edge returns.

Level triggered will keep returning while the state stays the same. 


5.	Describe the services provided by TCP but not UDP. 

Ports with IP, retransmission Packets can get dropped due to network errors or congestion, out of order packets, duplicate packets, error correction, flow control, congestion control, connection-Oriented/life cycle oriented are all the services that are provided by TCP but not by UDP.


6.	How does TCP connection establishment work? And how does it affect latency in HTTP1.0 vs HTTP1.1?

A TCP connection establishment works by establishing and creating a socket, binding to and listening at a port, and then accepting new connection. Moreover, HTTP1.0 does not allow you to send multiple requests while HTTP1.1 does.

7.	Wrap a version of read in a loop to read up to 16KB into a buffer from a pipe or socket. Handle restarts (`EINTR`), and socket-closed events (return 0).

    size_t bytes_read = 0;
    size_t counter = count;
    while(bytes_read < 16*1024) {
        ssize_t return_code = read(socket, buffer + bytes_read, counter);
        if(return_code == 0) {
            return bytes_read;
        } else if(return_code > 0) {
            bytes_read += return_code;
            counter -= return_code;
        } else if(return_code == -1 && errno == EINTR) {
            
        } else {
            return return_code;
        }
    }

8.	How is Domain Name System (DNS) related to IP and UDP? When does host resolution not cause traffic?

A URL gets converted to an IP address. If a machine does not hold answer locally then it sends a UDP pack to local DNS server. This server in turn may query other upstream DNS servers. 

9.	What is NAT and where and why is it used? 

NAT is Network Address Translation. It is used because NAT translate the IP addresses of computers in a local network to a single IP address. It is often used by the router that connects the computers to the internet.


## 8. Files 

1.	Write code that uses `fseek`, `ftell`, `read` and `write` to copy the second half of the contents of a file to a `pipe`.

char* theFile;
FILE* fileContents = fopen(theFile, "r");
fseek(fileContents, 0, SEEK_END);
size_t fileSize = ftell(fileContents);
fileSize = fileSize / 2;
fseek(fileContents, 0, fileSize);

char* fileBuffer;
fileBuffer = malloc(fileSize);

fread(fileBuffer, 1, fileSize, fileContents);

int pipe_fd;
write(pipe_fd[0], fileBuffer, fileSize);

2.	Write code that uses `open`, `fstat`, `mmap` to print in reverse the contents of a file to `stderr`.

int fd = open("filename", O_RDONLY);
struct stat file_stat; 
fstat(fd, &file_stat);
char* mapper = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

int HOLO = file_stat.st_size - 1;
while(HOLO >= 0) {
    fprintf(stderr, "%c", mapper[HOLO]);
    HOLO--;
}

3.	Write brief code to create a symbolic link and hard link to the file /etc/password

Symbol Link: 
symlink(/etc/password, linker_file);

Hard Link: 
link(/etc/password, linker_file)

4.	"Creating a symlink in my home directory to the file /secret.txt succeeds but creating a hard link fails" Why? 

You can create infinite symbolic links, but if there are many links you cannot create a hard link. 

5.	Briefly explain permission bits (including sticky and setuid bits) for files and directories.

Permission bits are the bits that are used to set the basic access permissions. There are a total of 9 of them. The first 3 set the permissions for the owner of the file. The next 3 set the permissions for the members of the file's group. The last 3 set the permissions for everyone else on the system.

A sticky bit is a permission bit that is set on a file or a directory that lets only the owner of the file/directory or the root user to delete or rename the file. 

The setuid bits indicate that when running the executable, it will set its permissions to that of the user who created it, instead of setting it to the user who launched it.

6.	Write brief code to create a function that returns true (1) only if a path is a directory.

struct stat stat-buff;

return S_ISDIR(stat-buff.st_mode);

7.	Write brief code to recursive search user's home directory and sub-directories (use `getenv`) for a file named "xkcd-functional.png' If the file is found, print the full path to stdout.

char* envo = getenv("xkcd-functional.png");
if(envo) {
    printf("%s", envo);
}

8.	The file 'installmeplz' can't be run (it's owned by root and is not executable). Explain how to use sudo, chown and chmod shell commands, to change the ownership to you and ensure that it is executable.

When the file 'installmeplz' cannot be run, you can call the sudo command in your terminal which allows a permitted user to execute a command as the superuser or another user.

When the file 'installmeplz' cannot be run, you can call the chown shell commmand which allows you to change the owner and group of files, directories, and links which will allow you to run the file.

When the file 'installmeplz' cannot be run, you can call the chmod shell command which allows you give the members of the group permission to read the file, but not to write to it.

## 9. File system 
Assume 10 direct blocks, a pointer to an indirect block, double-indirect, and triple indirect block, and block size 4KB.

1.	A file uses 10 direct blocks, a completely full indirect block and one double-indirect block. The latter has just one entry to a half-full indirect block. How many disk blocks does the file use, including its content, and all indirect, double-indirect blocks, but not the inode itself? A sketch would be useful.

10*4*1024 + 1024*4*1024 + 1024*1024*4*1024 + 1024*2*1024 = 1050122 

2.	How many i-node reads are required to fetch the file access time at /var/log/dmesg ? Assume the inode of (/) is cached in memory. Would your answer change if the file was created as a symbolic link? Hard link?

Symbolic link could be many. Hard link could also be 2 maximum I believe. 

1 Inode Read if normal file. 

3.	What information is stored in an i-node?  What file system information is not? 

The inode contains all the administrative data that is needed to read a file.
The only thing that is not stored in an inode is the name of the inode which is always stored in the directory above the inode (not directory data).

4.	Using a version of stat, write code to determine a file's size and return -1 if the file does not exist, return -2 if the file is a directory or -3 if it is a symbolic link.

struct stat buf;

if buf == NULL return -1

switch (buf.st_mode & S_IFMT) {

     case S_IFDIR:  return -3;
     case S_IFLNK:  return -2;
     case S_IFREG:  break;
}

return buf.st_size; 

5.	If an i-node based file uses 10 direct and n single-indirect blocks (1 <= n <= 1024), what is the smallest and largest that the file contents can be in bytes? You can leave your answer as an expression.

10*4KB + n*4KB

6.	When would `fstat(open(path,O_RDONLY),&s)` return different information in s than `lstat(path,&s)`?

if file is a symbolic link. 

## 10. "I know the answer to one exam question because I helped write it"

Create a hard but fair 'spot the lie/mistake' multiple choice or short-answer question. Ideally, 50% can get it correct.

char* buffer = NULL;
size_t n = 0;

while (1) {
    size_t EOF = getline(&buffer, &n, stdin);

    if (EOF == -1) {
        break;
    }

    if (EOF == 0) {
        break;
    }

    if( buffer[EOF - 1] == '\n' ) {
        buffer[EOF - 1] = 0; 
    }   

    printf("%s", buffer);
}

ANSWER: YOU MUST FREE THE BUFFER. NOT FREEING IS THE MISTAKE.