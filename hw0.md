# Homework 0

## Chapter 1

### 1.
This function can be written by:
```
#include <unistd.h>
int main() {
    write(1, "Hi! My name is Avinda.", 22);
    return 0;
}
```

### 2. 
A write_triangle function that will write to standard error with a height of n:
```
void write_line(int n) {
    int i;
    for (i = 0; i <= n; i++) {
        write(2, "*", 1);
    }
    write(2, "\n", 1);
}

void write_triangle(int n) {
    int i;
    for (i = 0; i < n; i++) {
        write_line(i);
    }
}

int main() {
    int n = 5; // Can be any number
    write_triangle(n);
    return 0;
}
```
### 3.
"Hello, World!" can be written into a file by:
```
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    mode_t mode = S_IRUSR | S_IWUSR;
    int filedes = open("hello_world.txt", O_CREAT | O_TRUNC | O_RDWR, mode);
    write(filedes, "Hello, World!", 13);
    close(filedes);
    return 0;
}
```

### 4.
"Hello, World!" can also be written to a file using `printf` by:
```
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    mode_t mode = S_IRUSR | S_IWUSR;
    close(1);
    int write_file = open("hello_world.txt", O_CREAT | O_TRUNC | O_RDWR, mode);
    printf("Hello, World!\n");
    close(write_file);
    return 0;
}
```
### 5.
One difference between `printf()` and system call `write()` is the fact that `printf()` uses a buffer when it is called. This means that it will only print once this buffer is full or if explicitly stated, such as `\n`. Additionally, `printf()` has the capability to print out a wider range of characters including integers since it has standard C library calls that can print that certain statement.  `write()` is unable to do this because it is a low level operative call that is written in a point in system memory and requires the numbers of bits before being printed. 

## Chapter 2

### 1.
The number of bits required for a byte can differ depending on the machine that a person is using. However most modern machines use 8 bits to represent one byte because this is the minimum required of bits needed to represent a byte. This was used in early machines because the processors had a limited number of bits.

### 2.
There is one byte in a `char`

### 3.
On my machine there are:
* 4 bytes in an `int` 
* 8 bytes in a `double`
* 4 bytes in a `float`
* 4 bytes in a `long`
* 8 bytes in a `long long`

### 4.
If the address of `int data[8]` starts at `0x7fbd9d40` then the address location of `data+2` will be `0x7fbd9d50`.

### 5.
Before `data[3]` is dereferenced to an integer. The C compiler converts it to an address pointer in memory that stores the information of the data, which is the integer. An address pointer is usually a size of 4 bytes.

### 6.
Because the char pointer `ptr` is a read-only constant string. The compiler is able to realize that it is immutable and returns an error when a constant pointer is trying to be changed.

### 7.
The value of the variable `str_size` is 12 bytes.

### 8.
The value of the variable `str_len` is 5 bytes.

### 9.
If X was a literal string constant of any 3 characters such as "abc" or "xyx" the `sizeof(X)` would be 3 bytes.

### 10.
An example of Y that would either be 4 or 8 bytes depending on the machine would be any single integer between 0-9.

## Chapter 3

### 1.
One way of doing this is creating a while loop that checks when `argv[i]` is equal to `NULL` and having a count variable that increments with every iteration of the while loop. Another way is to print the `argc` number at the end of the main function. This number is equivalent to the number of arguments that have been passed.

### 2.
`argv[0]` represents the execution name of the program that is being run.

### 3. 
The pointer to environment variables are stored in an external location in the user's shell. These variables can be accessed with the keyword `extern`.

### 4.
The value of `sizeof(ptr)` is equal to 4 bytes and the value of `sizeof(array)` is equal to 6 bytes. These two sizes are different because `char* ptr` holds the number of bytes needed to represent the char pointer. On the other hand `char array[]` represents a new char array that has been created and holds the size of the entire array, which in this case is 6 bytes.

### 5.
The user created function that the automatic variable is in manages the lifetime of the automatic variable. Once the function is done executing, the automatic variable is cleared from memory.

## Chapter 4

### 1.
One can use `malloc` in order to access data after the lifetime of the function. `malloc` allows the program to take a required number of bytes and keep it on the heap. If this variable is returned in that function, a pointer can be used to access that data if the function is called in main. 

### 2.
Some main differences between stack and heap memory is that the stack is used to store local variables while heap memory is used to store objects. Additionally, allocation and deallocation is done automatically on a stack while a user must manually do that for the heap. 

### 3.
When it comes to a C-compiler process, only stack and heap memory is used.

### 4.
"In a good C program, for every malloc, there is a `free()` call to deallocate the memory".

### 5.
`malloc()` can fail if there is not enough heap memory to fulfill the requested number of bytes. If this occurs, the pointer will point to NULL.

### 6.
The function `time()` is used to get the number of seconds that has past since January 1st, 1970 or a different date can be used as the start. However it is hard for a person to tell what the date is just from the number of seconds since 1970. The function `ctime()` uses the value returned from `time()` in order to create a string with date and time in a way a person would understand.

### 7.
The problem with that code snippet is that once a pointer is freed by `free(ptr)`, the computer keeps track and makes sure that the memory block is free. However, if `free(ptr)` is called again, it will confuse the processor because it believes that address is currently free which can cause multiple problems.

### 8.
The problem with this code snippet is that the program will stop with an error since it is trying to access a pointer that has already been freed by memory.

### 9.
In order to avoid the two mistakes mentioned above, it is good practice to set `ptr = NULL;` after freeing it from memory.

### 10 & 11.
```
#include <stdio.h>
#include <unistd.h>
struct Person {
	char* name;
	int age;
	struct Person** friends;
};

typedef struct Person person;

int main() {
	person* person1 = (person*) malloc(sizeof(person));
	person* person2 = (person*) malloc(sizeof(person));
	person1->name = "Agent Smith";
	person1->age = 128;
	person2->name = "Sonny Moore";
	person2->age = 256;
	person1->friends = &person2;
	person2->friends = &person1;
	free(person1);
	free(person2);
	return 0;
}
```
### 12.
The function to create a person uses same libraries and structs as questions 10-11:
```
person* create(int n_age, char* n_name) {
	person* new_person = (person*) malloc(person_size);
	new_person->name = strdup(n_name);
	new_person->age = n_age;
	return new_person;
	
}
```
### 13.
The function to destroy a person uses same libraries and structs as questions 10-11:
```
void destroy(person* p) {
	free(p->name);
	memset(p, 0, person_size);
	free(p);
}
```

## Chapter 5

### 1.
The three functions that can be used to get characters from `stdin` and write them to `stdout` are `getchar()`, `gets()` and `getline()`.

### 2.
The problem with `gets()` is that you must specify how many characters will be recieved from `stdin`. If there are more characters than space provided it will start overwriting memory that is designated for other parts of the program and this can cause errors.

### 3.
Code that parses the string into 3 variables:
```
#include <stdio.h>

int main() {
	char* data = "Hello 5 World";
	char first[10];
	char second[10];
	int num;
	sscanf(data, "%s %d %s", first, & num, second);
	printf("%s %d %s\n", first, num, second);
	return 0;
}
```
### 4.
Before using the function `getline` one must `#define _GNU_SOURCE` at the top of the file in order for `getline` to function properly.

### 5.
The snippet of code can print out a file line by line:
```
#include <stdio.h>

int main() {
	char* buffer = NULL;
	size_t num = 1;
	ssize_t result = 0;
	FILE *file = fopen("filename.txt", "r");
	while (result != -1) {
		result = getline(&buffer, &num, file);
		printf("%s\n", buffer);
		
	}
	return 0;
}
```

## C Development

### 1.
The compiler flag used to debug a build is `-g`

### 2.
When just using `make` to generate a build, this only creates a new build if there is a change to source files. The other libraries and built in functions are never built again. It is important to remake everything with the command `make clean` because it flushes out the old build completely and rebuilds everything which can get rid of small unwanted errors.

### 3. 
Tabs are used to indent commands of Makefile. If spaces are used, the makefile would not be able to understand the spacing and will put out an error.

### 4.
The command `git commit` is used to save the changes that are made to files that you want to change. Once these changes have been committed, it can then be uploaded to gitHub so other users can see the changes that have been made. In the context of git, a `sha` is a Unique ID that is created when something is committed which tracks the changes that have been made in the commit. This is useful because each `sha` can be accessed and see all the changes that have been made.

### 5.
The command `git log` shows all the commits that have been made in a certain folder.

### 6.
The command `git status` gives a track of all the files that are in the certain directory. It gives information about which files have commited changes or which files have been untracked/ unchanged.

### 7.
The command `git push` is important because without this command, the changes that have been made in a certain commit, will not be uploaded onto gitHub. So the files that are graded or accessed to a large group, will not be changed without the command `git push`.

### 8.
When this error occurs, it usually happens due to another user trying to push changes to the same branch. This problem can be resolved, pulling both branches at once or if the remote branch is merged to the source. This can also happen if certain files are trying to be destroyed or deleted.