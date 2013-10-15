#include <syscalls.h>
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

void main()
{
	setvbuf(stdin, NULL, _IONBF, 0);		/* important!! */
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    
	
   FILE * fp;

   fp = fopen ("kosmisk", "w");
   fprintf(fp, "%s %s %s %d", "We", "are", "in", 2013);
   
   fclose(fp);
   int num = 1234;
   printf("number : %d", num);


   for(;;);
}


