#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<sys/stat.h>
#include<setjmp.h>
#include<ucontext.h>
#include<signal.h>
#include<unistd.h>
#include<fcntl.h>


//discussed with TA and Ding Li
typedef struct MemoryRegion {
    void *startAddr;
    void *endAddr;
    unsigned long size; 
    int isReadable;
    int isWriteable;
    int isExecutable;
    int isContext;
}MemoryRegion;

void writeContext(int output_File, ucontext_t* context) {
    MemoryRegion m2;
    m2.startAddr = NULL;
    m2.endAddr = NULL;
    m2.isReadable = 1;
    m2.isWriteable = 1;
    m2.isExecutable = 1;
    m2.isContext = 1;
    m2.size = sizeof(*context);
    //printf("context     start:%p, end:%p, size:%ld another: %ld\n",m2.startAddr,     
    //m2.endAddr,m2.size,sizeof(m2));
    printf("writing context\n");
    ssize_t len = write(output_File, &m2, sizeof(MemoryRegion)); 
    if(len <=0) {
	//printf("wrong one\n");
    }
    if((len = write(output_File,context, sizeof(*context))) <= 0){
        //printf("something happened to write output_File for context\n");
        
    }
}
	
void signal_handler(int signum){

    printf("I have got your SIGUSR2 >>>>>>>>> Running Store Checkpoint Image function\n");
    
    
    
    char Path[200];
    strcpy(Path,"/proc/self/maps");
    FILE *file = fopen(Path,"r");
    int output_File = open("./myckpt", O_CREAT|O_WRONLY|O_TRUNC, 0666);
    char str[300];
    if(output_File < 0) {
        printf("something happened to ./myckpt\n");
        exit(0);
    }

    
    

    if(file == NULL) 
    {
        printf("open maps failed!\n");
        exit(0);
    }
    else 
    {
        printf("open maps succeed!\n");
    }

    //ucontext_t context;
    //getcontext(&context);
    //writeContext(output_File,&context);
    struct MemoryRegion m1;
    while (fgets(str,300,file) != NULL)
    {
	if(strstr(str,"vdso") != NULL) {
		break;
	}
	printf("%s\n",str);
	
        char* ouputArray[6];
        char* p;
        char* start;
        char* end;
        start = strtok(str,"-");
        ouputArray[0] = start;
        unsigned long s =  strtoul(start,NULL,16);
        //printf("%ld-",s);
        m1.startAddr = (void*) s;
        //printf("%s-",ouputArray[0]);
        end = strtok(NULL," ");
        ouputArray[1] = end;
	//printf("%s\n",ouputArray[1]);
        unsigned long e =  strtoul(end,NULL,16);
	//printf("%ld\n",e);
        
        m1.endAddr = (void*) e;
        unsigned long length = (void*)e - (void*)s;
        //get size for m1
        //printf("%s\n",(char*) m1.startAddr);
        m1.size = length;
        int i = 2;
        while((p = strtok(NULL," "))) 
        {
            ouputArray[i] = p;
            i++;
        }

        if (strstr(ouputArray[2],"r") != NULL)
        {
            m1.isReadable = 1;
            
            if(strstr(ouputArray[2],"w") != NULL) 
            {
                
            }
        }
        else 
        {
            m1.isReadable = 0;
	    continue;
        }

        if (strstr(ouputArray[2],"w") != NULL)
        {
            m1.isWriteable = 1;
        }
        else 
        {
            m1.isWriteable = 0;
        }

        if (strstr(ouputArray[2],"x") != NULL)
        {
            m1.isExecutable = 1;
        }
        else 
        {
            m1.isExecutable = 0;
        }
        //printf("size of m :  %ld\n",length);
        m1.isContext = 0;
        printf("start:%p, end:%p, size:%ld\n",m1.startAddr, m1.endAddr,m1.size);
        write(output_File, &m1, sizeof(m1));
        write(output_File, m1.startAddr,m1.size);
    }
    
    ucontext_t context;
    if (getcontext(&context) < 0) {
		printf("Problem of getcontext.\n");
    }
    //MemoryRegion m2;
    //m2.startAddr = NULL;
    //m2.endAddr = NULL;
    //m2.isReadable = 0;
    //m2.isWriteable = 0;
    //m2.isExecutable = 0;
    //m2.isContext = 1;
    //m2.size = sizeof(context);            
    //printf("size of content :  %ld\n",sizeof(context));

    writeContext(output_File,&context);
    close(output_File);
    fclose(file);

}


__attribute__ ((constructor))
void myconstructor() {
  signal(SIGUSR2, signal_handler);
}

