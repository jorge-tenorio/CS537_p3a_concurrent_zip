#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>

int offset = 0;

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("Specify a file to see output\n");
		exit(1);
	}

	int size;
    struct stat s;
    int file = open(argv[1], O_RDONLY);
    fstat (file, & s);
    size = s.st_size;

	
	for(int i = 0; i < size; i ++){    
		int rc = fork();

		if (rc == 0){
			//hexdump -s 0 -n 4 -e '1/4 " %3d"' out
			char *args[8];
			args[0] = strdup("hexdump");
			args[1] = strdup("-s");
			char str[256];
			sprintf(str, "%d", offset);
			if (i % 2 == 0){
				args[2] = strdup(str);
				args[3] = strdup("-n");
				args[4] = strdup("4");
				args[5] = strdup("-e");
				args[6] = strdup("1/4 \" %3d\"");
				args[7] = strdup(argv[1]);
			}else{
				//hexdump -s 4 -n 1 -e '1/1 " %_c\n"' out
				args[2] = strdup(str);
				args[3] = strdup("-n");
				args[4] = strdup("1");
				args[5] = strdup("-e");
				args[6] = strdup("1/1 \" %_c\n\"");
				args[7] = strdup(argv[1]);
			}
			args[8] = NULL;
			//(void) close(STDOUT_FILENO);
			//open("unzipout.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
			execvp(args[0], args);
			exit(1);
		}else if (rc > 0){
			(void)wait(NULL);
			if(i % 2 == 0){
				offset = offset + 4;
			}else{
				offset++;
				printf("\n");
			}
		}else{
			exit(1);
		}
	}
}
