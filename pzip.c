#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *args[]){
	if(argc < 1){
		printf("File not specified\n");
		exit(1);
	}
	
	// read all the files given to us
	for(int i = 0; i < argc; i++){
		FILE *file = fopen(args[i], "r");
		
				

		fclose(file);
	}
}
