#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("File not specified\n");
		exit(1);
	}
	
	// read all the files given to us
	for(int i = 1; i < argc; i++){
		
		char *f;
		int size;
		struct stat s;

		int file = open(argv[i], O_RDONLY);
		fstat (file, & s);
		size = s.st_size;
		
		f = (char *) mmap (0, size, PROT_READ, MAP_PRIVATE, file, 0);

		for (int i = 0; i < size; i++) {
			char c;

			c = f[i];
			putchar(c);
		}
		return 0;
	}
}

void readLine(){

}
