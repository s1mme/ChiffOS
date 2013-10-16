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
	FILE *ptr_file;

    char buf[18];

   /* ptr_file =fopen("znark.txt","r");
	int i;
    fgets(buf,7, ptr_file);
    printf("%s", buf);
 */
    ptr_file =fopen("saturn.txt","r");
	fgets(buf,100, ptr_file);
    printf("%s", buf);
        
    ptr_file = fopen("file", "w"); 
    
    FILE * fp;

    fp = fopen ("kosmisk", "w");
    fprintf(fp, "%s %s %s %d", "We", "are", "in", 2013);
  
   
    fclose(fp);
    int num = 1234;
    printf("number : %d", num);   
    
    ptr_file =fopen("text.txt","r");
	char ch;
 
    while( ( ch = fgetc(ptr_file) ) != EOF )
		printf("%c",ch);

   for(;;);
}


