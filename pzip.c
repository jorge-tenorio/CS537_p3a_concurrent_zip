#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>

typedef struct ThreadData{
	char *f;
	int size;
	int charCount;
	char prevChar;
	char currChar;
	int j;
	int argc;
	int threadNum;
	char *letters;
	int *nums;
	int k;
	int startSize;
}ThreadData;

//pthread_mutex_t lock;
//pthread_cond_t *cv;
//volatile int turn;

int numDigits(int n);

void printChar(int num, char letter){
	fwrite(&num, sizeof(num), 1, stdout);
	fwrite(&letter, sizeof(letter), 1, stdout);
}

void *readFile(void *threadData){

	struct ThreadData *data = threadData;
	int j = data->j;
	char *f = data->f;
	int size = data->size;
	int charCount = data->charCount;
	char prevChar = data->prevChar;
	char currChar = data->currChar;
	int argc = data->argc;
	//int threadTurn = data->threadTurn;

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

	//printf("start value for thread %i: %i\n", data->threadNum, data->startSize);

	for (int i = data->startSize; i < size; i++) {
		currChar = f[i];

		// Skip \0, do not reset char count
		if (currChar == 00) continue;

		//printf("thread num: %i, charCount %i, currchar: %c prevchar: %c char in file %i: %i\n", data->threadNum, charCount, currChar, prevChar, j, i);
	
		// Main zip function
		if (currChar == prevChar && (i < size - 1 || j < argc - 1)) {
			charCount++;
		} else {
			if(i == size - 1 && currChar == prevChar){
				charCount++;
			}

			if (prevChar == 10) {

				//printf("%i\\n\n", charCount);

				// uncomment below for testing
				char newlinechar = {'\n'};

				//printChar(charCount, newlinechar);
				nums[k] = charCount;
				letters[k] = newlinechar;
			}else {
				//printChar(charCount, prevChar);
				nums[k] = charCount;
				letters[k] = prevChar;

				//printf("%d%c",charCount, prevChar);

				if (prevChar != currChar && j == argc - 1 && i == size - 1){
					//printf("1%c", currChar);

					//printChar(1, currChar);
					k++;
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

	//for(int i = 0; i < k; i++){
		//printf("thread: %i, nums[%i]: %i, letters[%i]: %c\n", data->threadNum, i, nums[i], i, letters[i]);
	//}
	
	data->k = k;
	data->nums = nums;
	data->letters = letters;
	data->charCount = charCount;
	data->prevChar = prevChar;
	data->currChar = currChar;
	return data;
}

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("pzip: file1 [file2 ...]\n");
		exit(1);
	}
	
	long int num_threads = sysconf(_SC_NPROCESSORS_ONLN);;
	int charCount = 0;
	char prevChar = 0;
	char currChar = 0;
	//turn = 1;
	
	pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
	//cv = (pthread_cond_t*)malloc(sizeof(pthread_cond_t) * num_threads);

	if (threads == NULL){
		printf("Cannot malloc\n");
		exit(1);
	}

	//for(int i = 0; i < num_threads; i++){
		//if (pthread_cond_init(&cv[i], NULL) != 0) {
			//printf("pthread_cond_init() error");
			//exit(1);
		//}
	//}

	//pthread_mutex_init(&lock, NULL);

	ThreadData **threaddata;
    threaddata = malloc(sizeof(ThreadData*) * num_threads * (argc - 1));

	if (threaddata == NULL){
		printf("Cannot malloc\n");
		exit(1);
	}

	int lastIndex = 0;
	
	//printf("num files: %i\n", argc - 1);
	for(int j = 1; j < argc; j++){
		char *f;
		int size;
		struct stat s;
		
		int file = open(argv[j], O_RDONLY);
		int threadSize = 0;
		int currSize = 0;	
		if(file < 0) continue;

		fstat (file, & s);
		size = s.st_size;
		threadSize = size / num_threads;
		printf("file %i size: %i\n", j, size);
		f = (char *) mmap (0, size, PROT_READ, MAP_PRIVATE, file, 0);
		
		if(j == 1){
			prevChar = f[0];
		}

		ThreadData *totalData = malloc(sizeof(ThreadData) * num_threads);
		int buff = 0;
		for (int uu = 0; uu < num_threads; uu++){			
			ThreadData newThreadData;
			
			int tempSize = currSize + threadSize;

			if(size - tempSize < threadSize){
				buff = (size - tempSize);
			}else{
				buff = 0;
			}

			newThreadData.f = f;
			newThreadData.size = tempSize + buff;
			newThreadData.charCount = charCount;
			newThreadData.prevChar = prevChar;
			newThreadData.currChar = currChar;
			newThreadData.j = j;
			newThreadData.argc = argc;
			newThreadData.threadNum = uu + 1;
			newThreadData.startSize = currSize;
			totalData[uu] = newThreadData;
			printf("thread %i created. From %i to %i\n", uu + 1, totalData[uu].startSize, totalData[uu].size);
			pthread_create(&threads[uu], NULL, readFile, &totalData[uu]);

			currSize = currSize + threadSize;
		}

		free(totalData);
		
		for(int uu = 0; uu < num_threads; uu++){
			void *returnData;
			struct ThreadData *tempData;
			pthread_join(threads[uu], &returnData);
			printf("thread %i returned\n", uu + 1);
			tempData = returnData;
			//printf("length of nums for thread %i: %i\n", uu + 1, tempData->k); 
			*(threaddata + lastIndex) = tempData;
			lastIndex++;
		}
	}

	//pthread_mutex_lock(&lock);

	//while(turn != data->threadTurn)
		//pthread_cond_wait(&cv[data->threadTurn], &lock);
	
	//printf("Now starting to print %i\n", threaddata[0]->nums[0]);

	//Working through making print statement.
	char threadPrevChar = threaddata[0]->letters[0];
	char threadCurrChar = threaddata[0]->letters[0];
	int totalCharCount = 0;
	for (int i = 0; i < (num_threads * (argc - 1)); i++) {
		printf("thread %i data below:\n", i + 1);
		for (int n = 0; n < threaddata[i]->k; n++){
			printf("prevChar: '%c', currChar: '%c', totalCharCount: %i\n", threadPrevChar, threadCurrChar, totalCharCount);
			//printf("%d, %d\n", i != (num_threads * (argc - 1)) - 1, n != threaddata[i]->k - 1);
			if(threadPrevChar == threadCurrChar){
				printf("1totalcount incremented by %i\n", threaddata[i]->nums[n]);
				totalCharCount = totalCharCount + threaddata[i]->nums[n];
			}else{
				if (threadPrevChar == threadCurrChar && n == threaddata[i]->k - 1){
					printf("2totalcount incremented by %i. i: %i, n: %i\n", threaddata[i]->nums[n], i, n);
					totalCharCount = totalCharCount + threaddata[i]->nums[n];
				}

				if(threadPrevChar == 10){
					totalCharCount = threaddata[i]->nums[n];
					printf("%i\\n\n", totalCharCount);
				}else{
					printf("%i%c", totalCharCount, threadPrevChar);
				}
				totalCharCount = 0;
				//printChar(nums[i], letters[i]);
				//printf(" %d\n", tempint);
			}
			threadPrevChar = threadCurrChar;
			threadCurrChar = threaddata[i]->letters[n + 1];
		}
	}

	//turn++;
	//pthread_cond_signal(&cv[turn]);
	//pthread_mutex_unlock(&lock);
	
	//free(cv);
	free(threads);
	free(threaddata);

	return 0;
}

// gets the number of digits in a number
int numDigits (int n) {
    if (n < 0) return numDigits ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numDigits (n / 10);
}

