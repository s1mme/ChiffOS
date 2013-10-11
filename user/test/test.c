#include <syscalls.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdio.h>
/*void puts(unsigned char *buf)
{
	int a = 1;
	int num = 0;
	unsigned char * test = "hello world!";
	__asm__ __volatile__("int $0x80" : "=a" (a) : "0" (num), "b" (buf));
}
*/
void main()
{
	/*FILE *f = fopen("file.txt", "w");
	
	const char *text = "Write this to the file";
	fprintf(f, "Some text: %s\n", text);
	/*printf("hejsan\n");*/
	/*write(1,"hejsan",6);*/

	for(;;);
}


