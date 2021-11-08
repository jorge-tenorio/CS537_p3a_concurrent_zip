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
		printf("pzip: file1 [file2 ...]\n");
		exit(1);
	}
	
	//int num_threads = 0;
	//int page_size = getpagesize();
	//printf("page size: %i\n", page_size);	
	// read all the files given to us

	int charCount = 0;
	char prevChar = 0;
	char currChar;
	//printf("num files: %i\n", argc - 1);
	for(int j = 1; j < argc; j++){
		char *f;
		int size;
		struct stat s;
		
		int file = open(argv[j], O_RDONLY);
		
		if(file < 0){
			continue;
		}

		fstat (file, & s);
		size = s.st_size;
		//printf("file %i size: %i\n", j, size);
		f = (char *) mmap (0, size, PROT_READ, MAP_PRIVATE, file, 0);
		
		if(j == 1){
			prevChar = f[0];
		}
		
		for (int i = 0; i < size; i++) {
			currChar = f[i];
			//printf("charCount %i, currchar: %c prevchar: %c char in file %i: %i\n", charCount, currChar, prevChar, j, i);
			
			// Skip \0, do not reset char count
			if (currChar == 00) continue;

			// Address compare
			//if (i > 0) {
				//printf("\naddress jump: [%d]: %p > [%d]: %p\n", i-1, &f[i-1], i, &f[i]);
			//}

			//printf("charCount %i, currchar: %c prevchar: %c char in file %i: %i\n", charCount, currChar, prevChar, j, i);

			// Main zip function
			if (currChar == prevChar && (i <  size - 1 || j < argc - 1)) {
				charCount++;
			} else {

				if(i == size - 1 && currChar == prevChar){
					charCount++;
				}	
				
				// uncomment below to test	
				// convert prevChar to String
				char prevCharString[1] = {prevChar};
				
				if (prevChar == 10) {

					//printf("%i\\n\n", charCount);

					// uncomment below for testing
					char newlinechar = {'\n'};
					fwrite(&charCount, sizeof(charCount), 1, stdout);
					fwrite(&newlinechar, sizeof(newlinechar), 1, stdout);
				}else {

					// uncomment below to test
					// convert charCount to string
					char charCountString[numDigits(charCount)];
					sprintf(charCountString, "%d", charCount);
					
					fwrite(&charCount, sizeof(charCount), 1, stdout);
					fwrite(&prevCharString, sizeof(prevCharString), 1, stdout);

					//printf("%d%c",charCount, prevChar);

					if (prevChar != currChar && j == argc - 1 && i == size - 1){
						//printf("printing last char\n");
						//printf("1%c", currChar);
						int lastChar = 1;
						
						char currCharString[1] = {currChar};
						
						fwrite(&lastChar, sizeof(lastChar), 1, stdout);
						fwrite(&currCharString, sizeof(currCharString), 1, stdout);
					}
				}
				prevChar = currChar;
				charCount = 1;
			}
		}
	}
	return 0;
}

// gets the number of digits in a number
int numDigits (int n) {
    if (n < 0) return numDigits ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numDigits (n / 10);
}

void readLine(){

}
