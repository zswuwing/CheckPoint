#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
	int num = 1;
	while(1) {
		printf("I am wokking: %d\n",num);
		num++;
		sleep(2);

	}
	return 0;
}