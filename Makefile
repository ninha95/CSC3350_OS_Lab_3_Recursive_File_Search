all:
	gcc my_file_search.c -o my_file_search -pthread
	gcc file_search.c -o file_search -pthread

file_search:
	gcc file_search.c -o file_search -pthread

my_file_search:
	gcc my_file_search.c -o my_file_search -pthread

clean:
	rm -f *.o my_file_search file_search 
