  /* note these headers are all provided by newlib - you don't need to provide them */
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <sys/fcntl.h>
  #include <sys/times.h>
  #include <sys/errno.h>
  #include <sys/time.h>
  #include <stdio.h>
  #include "syscalls.h"
/*test*/
  void _exit();
  int close(int file);
  char **environ; /* pointer to array of char * strings that define the current environment variables */
  int execve(char *name, char **argv, char **env);
  int fork();
  int fstat(int file, struct stat *st);
  int getpid();
  int isatty(int file);
  int kill(int pid, int sig);
  int link(char *old, char *new);
 
  int open(const char *name, int flags, ...);
  int read(int file, void *ptr, int len);
 
  caddr_t sbrk(int incr);
  int stat(const char *file, struct stat *st);
  clock_t times(struct tms *buf);
  int unlink(char *name);
  int wait(int *status);
  int write(int file, char *ptr, int len);
  
 
  /*int gettimeofday(struct timeval *p, struct timezone *z);*/
  
  DECL_SYSCALL1(sbrk, void *, ptrdiff_t); 
  DEFN_SYSCALL1(sbrk, void *, 0, ptrdiff_t);
  
  DECL_SYSCALL3(write, int, int, const char *, int);
  DEFN_SYSCALL3(write, int, 1, int, const char *, int);
  
  DECL_SYSCALL3(read, int, int, void *, int);
  DEFN_SYSCALL3(read, int, 2, int, void *, int);
  
  DECL_SYSCALL2(open, int, const char *, int);
  DEFN_SYSCALL2(open, int, 3, const char *, int);
  
  DECL_SYSCALL2(fstat, int, int, struct stat *);
  DEFN_SYSCALL2(fstat, int, 4, int, struct stat *);
 
  DECL_SYSCALL3(execve, int, char *, char **, char **);
  DEFN_SYSCALL3(execve, int, 6, char *, char **, char **);
  
  DECL_SYSCALL0(exit, int ); 
  DEFN_SYSCALL0(exit, int, 7);
  
  DECL_SYSCALL0(getpid, int ); 
  DEFN_SYSCALL0(getpid, int, 8);
  
  DECL_SYSCALL1(kill, int, int); 
  DEFN_SYSCALL1(kill, int, 9, int);
  
  DECL_SYSCALL0(getchar, unsigned char); 
  DEFN_SYSCALL0(getchar, unsigned char, 10);
  
  DECL_SYSCALL3(stdio_read, int, int , void *, int);
  DEFN_SYSCALL3(stdio_read, int, 11, int , void *, int);
  
  DECL_SYSCALL0(sti, int ); 
  DEFN_SYSCALL0(sti, int, 12);
   
caddr_t sbrk(int incr) {
	
	void *ret = (void *)sys_sbrk(incr);
	if ((signed long)ret < 0 && (signed long)ret > -200) {
		errno = - ((int)ret);
		return (void *)(-1);
	}
	else {
		return ret;
	}
}

int write(int file, char *ptr, int len) {

	if (file < 0) {
		errno = EBADF;
		return -1;
	}
	int ret;
	if ((ret = sys_write(file, ptr, len)) < 0) {
		errno = -ret;
		return -1;
	}
	else
		return ret;
}


int read(int file, void *ptr, int len) {

	if (file < 0) {
		errno = -EBADF;
		return -1;
	}

	/*if (file == 0 && isatty(1))
		fflush(stdout);
*/
	int ret;
	if ((ret = sys_read(file, ptr, len)) < 0) {

		errno = -ret;
		return -1;
	}
	else
		return ret;
}

int open(const char *name, int flags, ...) {
	int ret;
	if (flags != O_RDONLY) {
		
		ret = sys_open(name,flags);
	}

		
	if ((ret = sys_open(name, flags)) < 0) {
		errno = -ret;
		return -1;
	}
	else
		return ret;
}

int fstat(int file, struct stat *st) {

	if (st == NULL) {
		errno = EFAULT;
		return -1;
	}

	int ret = sys_fstat(file, st);
	if (ret == 0)
		return 0;
	else {
		errno = -ret;
		return -1;
	}
}

int close(int file) {
	return 1;
}


int lseek(int fildes, int offset, int whence)
{
	
	return 1;
}

int isatty(int file) {
	
		
	if (file < 0) {
		errno = EBADF;
		return 0; 
	}
	struct stat st;
	int ret;
	if ((ret = fstat(file, &st)) == 0)
		return (S_ISCHR(st.st_mode)) ? 1 : 0;
	else {
		errno = -ret;
		return 0;
	}
}

int execve(char *name, char **argv, char **env) {
        return sys_execve((char*)name,(char**)argv,(char**)env);
}

void _exit(int status)
{
	sys_exit();
}
int getpid()
{
	sys_getpid();
}

int kill(int pid, int sig)
{
	sys_kill(pid); /* fix signals */
}


