/* (C) Max Schrimpf
 *
 * Author: Max Schrimpf
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


 /*
 * Uebung 2 - Systemprogrammierung
 * Max Schrimpf 
 */

#include <stdio.h>
// for strerror
#include <errno.h>
#include <string.h>
// for exit()
#include <stdlib.h>
// for lseek
#include <sys/types.h>
// for lockf
#include <unistd.h>

int open_lock_sleep_close( char *path, int from, int to, int sleepTime) {
	FILE *file = fopen(path, "r+");

	if (file == NULL) {
		printf("File can't be opened. Errno: %s\n", strerror(errno));
                exit(1);
	}

	printf("Locking ... \n");
	lseek(fileno(file), (off_t) from, SEEK_SET);
	int offset = to - from; 
	lockf(fileno(file), F_LOCK, offset);
	int i; 
	for(i = sleepTime; i> 0; i--) {
		sleep(1);
		printf("Sleeping %d remaining \n", i);
	}
	return fclose(file);
} 
 
int main ( int argc, char *argv[] ) {
	int from, to, sleepTime;
        if(argc < 4) {
                printf("Not enough parameters specified\n");
// 1 = false // 0 true
                exit(1);
        } 
	if(argc < 5) {
	 sleepTime = 10; 
	} else {
		sscanf(argv[4], "%d", &sleepTime); 
	}
        printf("Working in dir: %s\n", argv[1]);
	sscanf(argv[2], "%d", &from); 
	sscanf(argv[3], "%d", &to); 

        printf("Locking from Byte: %d\n", from, sleepTime);
        printf("Locking to Byte: %d\n", to);
        return open_lock_sleep_close(argv[1], from, to, sleepTime);
}

