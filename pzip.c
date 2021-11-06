#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
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
		char prevChar = f[0];
		int charCount = 1;
		for (int i = 0; i < size; i++) {
			char currChar;
			currChar = f[i];

			// Skip \0, do not reset char count
			if (currChar == 00) continue;

			// Address compare
			//if (i > 0) {
			//	printf("\naddress jump: [%d]: %p > [%d]: %p\n,", i-1, &f[i-1], i, &f[i]);
			//}

			// Main zip function 
			if (currChar == prevChar) {
				charCount++;
			} else {
				if (prevChar == 10) {
					printf("%d/n",charCount);
				} else {
					printf("%d%c",charCount, prevChar);
				}
				prevChar = currChar;
				charCount = 1;
			}
			//putchar(c);
		}
		printf("\n");
		return 0;
	}
}

void readLine(){

}
