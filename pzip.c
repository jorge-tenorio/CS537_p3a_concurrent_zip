#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

int numDigits(int n);

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("File not specified\n");
		exit(1);
	}
	setmode(fileno(stdout), O_BINARY);
	//int num_threads = 0;
	//int page_size = getpagesize();
	//printf("page size: %i\n", page_size);	
	// read all the files given to us
	for(int i = 1; i < argc; i++){
		
		char *f;
		int size;
		struct stat s;
		
		int file = open(argv[i], O_RDONLY);
		fstat (file, & s);
		size = s.st_size;
		//printf("file size: %i\n", size);
		f = (char *) mmap (0, size, PROT_READ, MAP_PRIVATE, file, 0);
		char prevChar = f[0];
		int charCount = 1;
		for (int i = 0; i < size; i++) {
			char currChar;
			currChar = f[i];
			//printf("currchar: %c prevchar: %c i: %i\n", currChar, prevChar, i);
			// Skip \0, do not reset char count
			if (currChar == 00) continue;

			// Address compare
			if (i > 0) {
				//printf("\naddress jump: [%d]: %p > [%d]: %p\n", i-1, &f[i-1], i, &f[i]);
			}

			// Main zip function 
			if (currChar == prevChar && i < (size - 1)) {
				charCount++;
			} else {
				
				// convert charCount to string
				char charCountString[numDigits(charCount)];
				sprintf(charCountString, "%d", charCount);

				// convert prevChar to String
				char prevCharString[2] = {prevChar, '\0'};
				
				if (prevChar == 10) {
					//printf("%d/n",charCount);
				} else {
					printf("%s%c",charCount, prevChar);
					//fwrite(charCountString, sizeof(charCountString), 1, stdout);
					//fwrite(prevCharString, sizeof(prevCharString), 1, stdout);
					
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

// gets the number of digits in a number
int numDigits (int n) {
    if (n < 0) return numDigits ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numDigits (n / 10);
}

void readLine(){

}
