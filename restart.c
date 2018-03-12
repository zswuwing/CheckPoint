#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <ucontext.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>


#define addressOfStack 0x5300000



#define PointerOfStack 0x5400000


#define SIZE 0x100000



//discussed with TA and Ding Li
char image_File[1000];
ucontext_t uct;

typedef struct MemoryRegion{
    void *startAddr;
    void *endAddr;
    unsigned long size; 
    int isReadable;
    int isWriteable;
    int isExecutable;
    int isContext;
}MemoryRegion;


void unMap() {
    FILE* file = fopen("/proc/self/maps", "r");
    
    char stackWord[] = "stack";
    char str[300];
    
    while (fgets(str,300,file) != NULL) {
        char* ouputArray[6];
        //printf("%s\n",str);
        bool getStackOrnot = false;
        if (strstr(str,"stack") != NULL) {
            printf("Stack Found\n");
            getStackOrnot = true;
        }

        char* p;
        char* start;
        char* end;
        start = strtok(str,"-");
        ouputArray[0] = start;
        unsigned long s =  strtoul(start,NULL,16);
        
        void* startAddr2 = (void*) s;
        //printf("%lu\n",*a);
        //printf("%s\n",(char*) m1.startAddr);
        //printf("%s\n",ouputArray[0]);
        end = strtok(NULL," ");
        ouputArray[1] = end;
        unsigned long e =  strtoul(end,NULL,16);
        void* endAddr2 = (void*) e;
        unsigned long length = (void*)e - (void*) s;
        if(getStackOrnot == true) {
        	int condition = -1;
           	condition = munmap(startAddr2, length);
           	if (condition < 0) {
               		printf("Wrong in unMap.\n");
           	}

           		printf("unMap done.\n");
           	break;
        }

    }

}







void restort() {
    int rd = open(image_File, O_RDONLY);
    if(rd < 0) {
        printf("Cannot Open image_File\n");
    }

    MemoryRegion m1;
    
    while(read(rd, &m1, 40) > 0) {

    	printf("start:%p, end:%p,size: %ld, context: %d difference:%ld \n",m1.startAddr, 	m1.endAddr,m1.size,m1.isContext,m1.endAddr - m1.startAddr);

        
	if (m1.isContext == 1) {
        	if(read(rd, &uct, sizeof(ucontext_t)) < 0) {
        		printf("Cannot Found Context\n");
        	}
            	printf("Found Context\n");
		
        }
    



        int prot = PROT_WRITE;
        if (m1.isReadable == 1) {
            prot |= PROT_READ;
        }
        if (m1.isWriteable == 1) {
            prot |= PROT_WRITE;
        }
        if (m1.isExecutable == 1) {
            prot |= PROT_EXEC;
        }

        mmap(m1.startAddr, m1.endAddr - m1.startAddr, prot, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);

        if (read(rd, m1.startAddr, m1.size) < 0) {
            printf("Error when restore memory\n");
	    
        }
    }
    close(rd);

    printf("begin restore context.\n");
    int ret = -1;
    if ((ret = setcontext(&uct)) < 0) {
	exit(1);
        printf("Error in set context.\n");
    }
    printf("set context ret:%d\n", ret);
    
}



int main(int argc, char **argv){
	if(argc < 2) {
		printf("Please input more elements\n");
	}
	strcpy(image_File, argv[1]);

	long startaddr = addressOfStack;
    	void *stackstart = (void *)startaddr;
    	size_t stacksize = SIZE;
	void *stackptr = (void *)PointerOfStack;
	void *map = mmap(stackstart, stacksize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
	if (map == MAP_FAILED) {
      		perror("mmap");
      		exit(1);
    	}
	asm volatile ("mov %0,%%rsp" : : "g" (stackptr) : "memory");

    	unMap();
    	restort();
    	return 0;
    	

}


