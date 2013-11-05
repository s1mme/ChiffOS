
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdio.h>
void puts_(unsigned char *buf)
{
	int a = 1;
	int num = 5;
	unsigned char * test = "hello world!";
	__asm__ __volatile__("int $0x80" : "=a" (a) : "0" (num), "b" (buf));
}


#define BUFFERSIZE 60
unsigned char buf[20];
int x;
char usage[] = "Usage: [-hc] [--help] "; /*todo */
unsigned int max_args;

char *retarg(char *buffer)
{
	int i = 0;
	char *tmp = malloc(20);
	unsigned char buffer_[strlen(buffer)-strlen(buf)];
               
                     while ((x = *buffer++) != 0)
                     {
                          buffer_[i] = (unsigned char)x;
                          tmp[i] = buffer_[i];
						 
                          i++;    
                     }
                     buffer_[i] = '\0';
    return tmp;
}

int main(int argc, char *argv[])
{
    char *buffer = malloc(128);
	char c;
    
    while(fgets(buffer, BUFFERSIZE , stdin) != NULL)
    {		  
		//printf("root@chiffos $ ");
       
         if(strcmp(buffer, "cat\n") == 0)
		 {
			 char **envp = { NULL};
			 char *argv[] = {"cat", "", envp};
			 execve("cat",argv,envp);
		 } 
	    else {
			
			char *cmdbackup = buffer;
             
            int i = 0;
            int realcmd = 1;

            while ((x = *buffer++) != ' ' && x != 0)
                {
                    buf[i] = (unsigned char)x;
                    i++;
                } 
            if(x == 0)
				realcmd = 0; 
            buf[i] = 0;
            i = 0;
            
            if(realcmd){
				 if(strcmp(buf, "help") == 0)
				{
					printf ("%s\n", usage);
					char *tmp_;
                    tmp_ = retarg(buffer);
					puts_(tmp_);
			
					if(strcmp((unsigned char*)tmp_,"-h\n") == 0)
					{
                                printf("Chishell v0.1 help\n");
                                printf("Possible command line options:\n");
                                printf("-c command Execute a command\n");
                                printf("-h Display this help screen\n");
					}
					if(strcmp((unsigned char*)tmp_,"-c\n") == 0)
					{
								printf("cat - output a files content\n");
								printf("hello - open ported GNU hello!\n");
					}
					
			}
		
				if(strcmp(buf,"hello") == 0)
                {
					 char *tmp_;
                     tmp_ = retarg(buffer);
						
					 size_t len = strlen(cmdbackup); /*counting arg's, maybe necessary in future */
					 for (size_t i=0; i < len; i++) {
				 	 if (cmdbackup[i] == ' ') {
					 max_args++;
					 while (cmdbackup[i] == ' ' && i < len) i++; 
							}
					 }	
						
						
					 argv[0] = "hello";
				   	 argv[1] =  (unsigned char*)tmp_;
					 argv[2] = NULL;    
					
					
					
                     char **envp = { NULL};

		     execve("hello",argv,envp);
	     }
	     if(strcmp(buf, "glxgears") == 0)
		 {    
			char **envp = { NULL};
			char *argv[] = {"glxgears", "-h", NULL};
			execve("glxgears",argv,envp); 
			/* kill(getpid());*/
		 } 
		   if(strcmp(buf, "run") == 0)
		 {    
			char *argv[] = {"run", "-h", NULL};
			execve("run",argv,2); 
			/* kill(getpid());*/
		 } 
	 }
	}			  
	    
	 
    
}

    return 0;
}


