#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

typedef struct FileData{
	int count;
	char prev;
	char curr;
	int* n;
	int* l;
	int nlSize;
}FileData;

int numDigits(int n);

void printChar(int num, char letter){
	fwrite(&num, sizeof(num), 1, stdout);
	fwrite(&letter, sizeof(letter), 1, stdout);
}

FileData readFile(char *f, int size, int charCount, char prevChar, char currChar, int j, int argc){
	if(j == 1){
		prevChar = f[0];
	}

	int arrSize = 8;
	int k = 0; // nums and letters index
	int* nums = (int*)malloc(arrSize * sizeof(int));
	char* letters = (char*)malloc(arrSize * sizeof(char));
	if (nums == NULL || letters == NULL) {
		printf("Error: Memory not allocated.\n");
		exit(0);
	}

	for (int i = 0; i < size; i++) {
		currChar = f[i];

		// Skip \0, do not reset char count
		if (currChar == 00) continue;

		//printf("charCount %i, currchar: %c prevchar: %c char in file %i: %i\n", charCount, currChar, prevChar, j, i);

		// Main zip function
		if (currChar == prevChar && (i <  size - 1 || j < argc - 1)) {
			charCount++;
		} else {
			if(i == size - 1 && currChar == prevChar){
				charCount++;
			}

			if (prevChar == 10) {

				//printf("%i\\n\n", charCount);

				// uncomment below for testing
				char newlinechar = {'\n'};

				printChar(charCount, newlinechar);
				nums[k] = charCount;
				letters[k] = newlinechar;
			}else {
				printChar(charCount, prevChar);
				nums[k] = charCount;
				letters[k] = prevChar;

				//printf("%d%c",charCount, prevChar);

				if (prevChar != currChar && j == argc - 1 && i == size - 1){
					//printf("1%c", currChar);

					printChar(1, currChar);
					nums[k] = 1;
					letters[k] = currChar;
				}
			}
			prevChar = currChar;
			charCount = 1;
			k++;
			if (k > arrSize) {
				arrSize = arrSize * 2;
				nums = realloc(nums, arrSize * sizeof(int));
				letters = realloc(letters, arrSize * sizeof(char)); 
			}
		}
	}

	//Working through making print statement.
	for (int i = 0; i <= k; i++) {
		//printf("%i%c\n",nums[i], letters[i]);
		printChar(nums[i], letters[i]);
		//printf(" %d\n", tempint);
	}

	// Add extra space incase array is completely full
	// TODO: put print statement in the main method after running through the file. 

	struct FileData data;
	data.count = charCount;
	data.prev = prevChar;
	data.curr = currChar;
	data.n = nums;
	data.l = nums;
	free(nums);
	free(letters);
	return data;
}

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
	char currChar = 0;
	//printf("num files: %i\n", argc - 1);
	for(int j = 1; j < argc; j++){
		char *f;
		int size;
		struct stat s;
		
		int file = open(argv[j], O_RDONLY);
		
		if(file < 0) continue;


		fstat (file, & s);
		size = s.st_size;
		//printf("file %i size: %i\n", j, size);
		f = (char *) mmap (0, size, PROT_READ, MAP_PRIVATE, file, 0);
		
		if(j == 1){
			prevChar = f[0];
		}

		struct FileData data = readFile(f, size, charCount, prevChar, currChar, j, argc);

		charCount = data.count;
		prevChar = data.prev;
		currChar = data.curr;
	}
	return 0;
}

// gets the number of digits in a number
int numDigits (int n) {
    if (n < 0) return numDigits ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numDigits (n / 10);
}

