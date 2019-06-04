// Andy Ninh
// CSC3350 - Operating Systems Programming
// Lab 3 - Recursive File Search
 // Extra Credit

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>

//takes a file/dir as argument, recurses,
// prints name if empty dir or not a dir (leaves)
void recur_file_search(char *file);


// This is a function declaration for the threaded file seearch.
// Parameters: a void pointer to a given argument variable.
// Returns a pointer to void type 
void* threaded_file_search(void* arg);

//share search term globally (rather than passing recursively)
const char *search_term;

//A character pointer array for the path of strings given
//which is set to a given size of 1000
char* stringPath[1000];

// A counter variable called i which is initially set to 0
size_t i = 0;

//The four threads that will be used as a global variable
pthread_t thread[4];

//A struct of the directory given which has a range from the given
//startingPoint and an endPoint 
struct directoryPath 
{
	size_t startingPoint;	//The starting point in the directory path
	size_t endPoint;	//The end point in the directory path
};

// Main which will run the program
// Rturns a type in value
// Paramters: the int argc value and a charcater ** value
// called argv
int main(int argc, char **argv)
{
	// Checks to see if input is valid argument.
	// If the argument given doesn't have 3 values
	if(argc != 3)
	{
		printf("Usage: my_file_search <search_term> <dir>\n");
		printf("Performs recursive file search for files/dirs matching\
				<search_term> starting at <dir>\n");
		exit(1);
	}

	//don't need to bother checking if term/directory are swapped, since we can't
	// know for sure which is which anyway
	search_term = argv[1];

	//open the top-level directory
	DIR *dir = opendir(argv[2]);

	//make sure top-level dir is openable (i.e., exists and is a directory)
	if(dir == NULL)
	{
		perror("opendir failed");
		exit(1);
	}
	
	//start timer for recursive search
	struct timeval start, end;
	gettimeofday(&start, NULL);

	// This calls the recursive file search method.
	recur_file_search(argv[2]);


	int counter;	//A counter variable which will keep count
	if(i >= 4)	//Checks to see if counter i greater than or equal to 4
	{
		counter = i/4;	//Sets counter variable accordingly
	}
	else	//Other cases for when counter i is less than 4
	{
		counter = i%4;	//sets counter variable accordingly
	}

	//Structs of the directory path ranges which specify ranges within
	//the provided array for each thread
	struct directoryPath firstThread;	//Struct of first thread
	firstThread.startingPoint = 0;	//Starting point of first thread
	firstThread.endPoint = counter;	//Ending point of first thread

	struct directoryPath secondThread;	//Struct of second thread
	//Starting point of second thread
	secondThread.startingPoint = firstThread.endPoint + counter;
	//Ending point of second thread
	secondThread.endPoint = secondThread.startingPoint + counter;

	//Struct of third thread
	struct directoryPath thirdThread;
	//Staring point of third thread
	thirdThread.startingPoint = secondThread.endPoint + counter;
	//Ending point of third thread
	thirdThread.endPoint = thirdThread.startingPoint + counter;
	
	//Struct of forth thread
	struct directoryPath fourthThread;
	//Starting point of fourth thread
	fourthThread.startingPoint = thirdThread.endPoint + counter;
	//Ending point of fourth thread
	fourthThread.endPoint = fourthThread.startingPoint + counter;

	//Pointers that allow for passing the structs in to pthread_create 
	//function (all four threads).
	struct directoryPath* firstPassVal = &firstThread;
	struct directoryPath* secondPassVal = &secondThread;
	struct directoryPath* thirdPassVal = &thirdThread;
	struct directoryPath* fourthPassVal = &fourthThread;

	//New threads called pthread_create which uses address of threads,
	//their attributes, the function pointer being called, and 
	//given parameters of functions.
	//This is for each of the 4 threads provided (0 to 3).
	pthread_create(&thread[0], NULL, threaded_file_search, firstPassVal);
	pthread_create(&thread[1], NULL, threaded_file_search, secondPassVal);
	pthread_create(&thread[2], NULL, threaded_file_search, thirdPassVal);
	pthread_create(&thread[3], NULL, threaded_file_search, fourthPassVal);
	
	int j = 0;	//A counter variable 
	while(j < 4)	//Loops through this when count is less than 4
	{
		//checks to see if the thread at given counter index can't
		//be joined.
		if(pthread_join(thread[j], NULL))
		{	//Prints statement when threads don't work.
			printf("Sorry, but the threads couldn't be joined.\n");
		}
		j++;	//Increments counter variable 
	}

	gettimeofday(&end, NULL);
	printf("Time: %ld\n", (end.tv_sec * 1000000 + end.tv_usec)
			- (start.tv_sec * 1000000 + start.tv_usec));

	//Exits thread
	pthread_exit(NULL);

	int k = 0;	// A counter variable k that is set to 0
	while(k < i)	// Checks to see if counter k is less than i
	{
		free(stringPath[k]);	//Deallocation of memory
		k++;	//Increments counter variable k by 1 each time
	}

	return 0;
}


//This function takes a path to recurse on, searching for mathes to the
// (global) search_term.  The base case for recursion is when *file is
// not a directory.
//Parameters: the starting path for recursion (char *), which could be a
// directory or a regular file (or something else, but we don't need to
// worry about anything else for this assignment).
//Returns: nothing
//Effects: prints the filename if the base case is reached *and* search_term
// is found in the filename; otherwise, prints the directory name if the directory
// matches search_term.
void recur_file_search(char *file)
{
	//check if directory is actually a file
	DIR *d = opendir(file);

	//NULL means not a directory (or another, unlikely error)
	if(d == NULL)
	{
		//opendir SHOULD error with ENOTDIR, but if it did something else,
		// we have a problem (e.g., forgot to close open files, got
		// EMFILE or ENFILE)
		if(errno != ENOTDIR)
		{	
			perror("Something weird happened!");
			fprintf(stderr, "While looking at: %s\n", file);
			exit(1);
		}

		//nothing weird happened, check if the file contains the search term
		// and if so print the file to the screen (with full path)
		if(strstr(file, search_term) != NULL)
			printf("%s\n", file);

		//no need to close d (we can't, it is NULL!)
		return;
	}

	//we have a directory, not a file, so check if its name
	// matches the search term
	if(strstr(file, search_term) != NULL)
		printf("%s/\n", file);

	//call recur_file_search for each file in d
	//readdir "discovers" all the files in d, one by one and we
	// recurse on those until we run out (readdir will return NULL)
	struct dirent *cur_file;
	while((cur_file = readdir(d)) != NULL)
	{
		//make sure we don't recurse on . or ..
		if(strcmp(cur_file->d_name, "..") != 0 &&\
				strcmp(cur_file->d_name, ".") != 0)
		{
			//we need to pass a full path to the recursive function, 
			// so here we append the discovered filename (cur_file->d_name)
			// to the current path (file -- we know file is a directory at
			// this point)
			char *next_file_str = malloc(sizeof(char) * \
					strlen(cur_file->d_name) + \
					strlen(file) + 2);

			strncpy(next_file_str, file, strlen(file));
			strncpy(next_file_str + strlen(file), \
					"/", 1);
			strncpy(next_file_str + strlen(file) + 1, \
					cur_file->d_name, \
					strlen(cur_file->d_name) + 1);

			//recurse on the file
			recur_file_search(next_file_str);

			//free the dynamically-allocated string
			free(next_file_str);
		}
	}

	//close the directory, or we will have too many files opened (bad times)
	closedir(d);
}

// Threaded file search method
// Parameters: a void pointer argument which  casts arguments accordingly.
// return: type is a void pointer 
// Method will be called and actions as appropriate when a thread is created.
void* threaded_file_search(void* arg)
{
	//Sets the directory Path range to a struct argument variable
	struct directoryPath* ran = (struct directoryPath*)arg;

	// Checks to see if range is valid, being less than value of i
	if(ran->startingPoint < i)
	{
		// An integer variable a which is set to the starting
		// point of selected range 
		int a = ran->startingPoint;
		
		//Loops through here while value of a hasn't reached
		//the end point of given range 
		while(a < ran->endPoint)
		{
			// Checks to see if given point is less than value of i
			if(a < i)
			{	//Checks to see if given stringpath
				//contains the given search term given
				if(strstr(stringPath[a], search_term))
				{	//Prints a statement of path
					printf("%s\n", stringPath[a]);
				}
			}
			a++;	//Increments counter of current point
		}
	}
}
