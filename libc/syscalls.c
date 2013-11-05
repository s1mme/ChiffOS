#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>

#include <stdio.h>
#include <limits.h>
#include <reent.h>
#include <regex.h>
#include <sys/signal.h>


typedef signed   char  sint8;
typedef unsigned char  uint8;
typedef signed   short sint16;
typedef unsigned short uint16;
typedef signed   int   sint32;
typedef unsigned int   uint32;
typedef signed long long sint64;
typedef unsigned long long uint64;

#define DECL_SYSCALL0(fn, ret) ret sys_##fn(void);
#define DECL_SYSCALL1(fn, ret, p1) ret sys_##fn(p1);
#define DECL_SYSCALL2(fn, ret, p1,p2) ret sys_##fn(p1,p2);
#define DECL_SYSCALL3(fn, ret, p1,p2,p3) ret sys_##fn(p1,p2,p3);
#define DECL_SYSCALL4(fn, ret, p1,p2,p3,p4) ret sys_##fn(p1,p2,p3,p4);
#define DECL_SYSCALL5(fn, ret, p1,p2,p3,p4,p5) ret sys_##fn(p1,p2,p3,p4,p5);
#define DECL_SYSCALL6(fn, ret, p1,p2,p3,p4,p5,p6) ret sys_##fn(p1,p2,p3,p4,p5,p6);
#define DECL_SYSCALL7(fn, ret, p1,p2,p3,p4,p5,p6,p7) ret sys_##fn(p1,p2,p3,p4,p5,p6,p7);
#define DEFN_SYSCALL0(fn, ret, num) \
ret sys_##fn(void) \
{ \
  ret a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num)); \
  return a; \
}

#define DEFN_SYSCALL1(fn, ret, num, P1) \
ret sys_##fn(P1 p1) \
{ \
  ret a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1)); \
  return a; \
}

#define DEFN_SYSCALL2(fn, ret, num, P1, P2) \
ret sys_##fn(P1 p1, P2 p2) \
{ \
  ret a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1), "c" ((ret)p2)); \
  return a; \
}

#define DEFN_SYSCALL3(fn, ret, num, P1, P2, P3) \
ret sys_##fn(P1 p1, P2 p2, P3 p3) \
{ \
  ret a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1), "c" ((ret)p2), "d"((ret)p3)); \
  return a; \
}

#define DEFN_SYSCALL4(fn, ret, num, P1, P2, P3, P4) \
ret sys_##fn(P1 p1, P2 p2, P3 p3, P4 p4) \
{ \
  ret a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1), "c" ((ret)p2), "d" ((ret)p3), "S" ((ret)p4)); \
  return a; \
}

#define DEFN_SYSCALL5(fn, ret, num, P1, P2, P3, P4, P5) \
ret sysc_##fn(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) \
{ \
  ret a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1), "c" ((ret)p2), "d" ((ret)p3), "S" ((ret)p4), "D" ((ret)p5)); \
  return a; \
}

#define DEFN_SYSCALL6(fn, ret, num,P1, P2, P3, P4, P5, P6) \
ret sys_##fn(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) \
{ \
  ret a; \
  asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1), "c" ((ret)p2), "d" ((ret)p3), "S" ((ret)p4), "D" ((ret)p5), "e"((ret)p6)); \
  return a; \
}



DECL_SYSCALL2(physicaltovirt, int, uint32, uint32);
DECL_SYSCALL1(exit, void, int);
DECL_SYSCALL1(puts, int, const char *);
DECL_SYSCALL0(cli, int);
DECL_SYSCALL0(uptime, int);
DECL_SYSCALL0(getchar, int);
DECL_SYSCALL1(putchar, int, int);
DECL_SYSCALL2(open, int, const char *, int);
DECL_SYSCALL3(read, int, int, void *, size_t);
DECL_SYSCALL0(clear, int);
DECL_SYSCALL0(reboot, int);
DECL_SYSCALL0(swap, int);
DECL_SYSCALL0(giveSCREEN, uint16 *);
DECL_SYSCALL1(setx, int,int);
DECL_SYSCALL1(put_dec, int,int);
DECL_SYSCALL2(stat, int, const char *, struct stat *);
DECL_SYSCALL1(chdir, int, const char *);
DECL_SYSCALL3(write, int, int, const char *, int);
DECL_SYSCALL2(fstat, int, int, struct stat *);
DECL_SYSCALL0(getpid, int);
DECL_SYSCALL0(VBEstart, int);
DECL_SYSCALL1(sbrk, void *, ptrdiff_t); // TODO: return type caddr_t
DECL_SYSCALL0(__getreent, struct _reent *);
DECL_SYSCALL2(gettimeofday, int, struct timeval *, void *);
DECL_SYSCALL2(nanosleep, int, const struct timespec *, struct timespec *);
DECL_SYSCALL1(wait, int, int *);
DECL_SYSCALL0(getppid, int);
DECL_SYSCALL3(waitpid, int, int, int *, int);
DECL_SYSCALL2(getcwd, char *, char *, size_t);
DECL_SYSCALL1(dup, int, int);
DECL_SYSCALL2(dup2, int, int, int);
DECL_SYSCALL2(setxy, int, int, int);
DECL_SYSCALL1(pipe, int, int *);
DECL_SYSCALL2(access,int,char *,int );
DECL_SYSCALL3(ioctl, int,int, int,void * );
DECL_SYSCALL0(sti, int);
DECL_SYSCALL3(execve, int, char *, char **, char **);


DEFN_SYSCALL1(puts, int, 1, const char *);
DEFN_SYSCALL0(cli, int, 2);
DEFN_SYSCALL0(getchar, int, 3);
DEFN_SYSCALL1(putchar, int, 4, int);
DEFN_SYSCALL2(open, int, 5, const char *, int);
DEFN_SYSCALL3(read, int, 6, int, void *, size_t);
DEFN_SYSCALL3(execve, int, 7, char *, char **, char **);
DEFN_SYSCALL2(setxy, int, 8, int, int);
DEFN_SYSCALL0(clear, int, 9);
DEFN_SYSCALL0(reboot, int, 10);
DEFN_SYSCALL1(setx, int, 11,int);
DEFN_SYSCALL3(write, int, 12, int, const char *, int);
DEFN_SYSCALL1(put_dec, int, 13,int);
DEFN_SYSCALL2(fstat, int, 14, int, struct stat *);
DEFN_SYSCALL0(swap, int, 15);
DEFN_SYSCALL1(sbrk, void *, 16, ptrdiff_t);
DEFN_SYSCALL0(giveSCREEN, uint16 *, 17);
DEFN_SYSCALL0(get_system_time, int, 18);
DEFN_SYSCALL2(gettimeofday, int, 19, struct timeval *, void *);
DEFN_SYSCALL2(nanosleep, int, 20, const struct timespec *, struct timespec *);
DEFN_SYSCALL1(wait, int, 21, int *);
DEFN_SYSCALL2(physicaltovirt, int,22, uint32, uint32);
DEFN_SYSCALL0(getppid, int, 23);
DEFN_SYSCALL3(waitpid, int, 24, int, int *, int);
DEFN_SYSCALL0(VBEstart, int, 25);
DEFN_SYSCALL2(getcwd, char *, 26, char *, size_t);
DEFN_SYSCALL1(dup, int, 27, int);
DEFN_SYSCALL2(dup2, int, 28, int, int);
DEFN_SYSCALL1(pipe, int, 29, int *);
DEFN_SYSCALL2(access,int, 30, char *, int);
DEFN_SYSCALL3(ioctl,int, 31, int, int, void *);
DEFN_SYSCALL0(getpid, int, 32);
DEFN_SYSCALL2(stat, int, 32, const char *, struct stat *);
DEFN_SYSCALL1(chdir, int, 33, const char *);
DEFN_SYSCALL3(getdents, int, 34, int, void *, int);
DEFN_SYSCALL0(sti,int,35);

void sys__exit(int status) {
	asm volatile("int $0x80" : : "a" (0), "b" ((int)status));
}

sint64 sys_lseek(int fd, sint64 offset, int whence) {
	union {
		sint64 o64;
		uint32 u32[2];
	} arg, ret;
	arg.o64 = offset;
	asm volatile("int $0x80" : "=a" (ret.u32[0]), "=d" (ret.u32[1]) : "0" (13), "b" (fd), "c" (arg.u32[0]), "d"(arg.u32[1]), "S"(whence));
	return ret.o64;
}
 
  int execve(char *name, char **argv, char **env) {
        return sys_execve((char*)name,(char**)argv,(char**)env);
}
/****************
 *** WRAPPERS ***
 ****************/

int getppid(void) {
	return sys_getppid();
}

off_t lseek(int fd, off_t offset, int whence) {
	sint64 ret;
	ret = sys_lseek(fd, offset, whence);

	if (ret > 0) {
		return ret;
	}
	else {
		errno = (int)(-ret);
		return -1;
	}
}

int chdir(const char *path) {
	int r = sys_chdir(path);
	if (r == 0)
		return 0;
	else {
		errno = -r;
		return -1;
	}
}

void _exit(int status) {
	sys__exit(status);
	for(;;) { }
}

int close(int file) {
	//int r = sys_close(file);
	//if (r != 0) {
	//	errno = -r;
		//return -1;
	//}
}

int _execve(const char *name, char * const *argv, char * const *env) {
	return execve(name, argv, env);
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

int getpid(void) {

	return sys_getpid();
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

int kill(int pid, int sig) {
	
	// TODO: kill!
	errno = EINVAL;
	return -1;
}

int link(const char *old, const char *new) {
	
	// TODO: link!
	errno = EMLINK;
	return -1;
}

int open(const char *name, int flags, ...) {
	
	if (flags != O_RDONLY) {
		errno = EPERM; // TODO: allow writing
		return -1;
	}

	int ret;
	if ((ret = sys_open(name, flags)) < 0) {
		errno = -ret;
		return -1;
	}
	else
		return ret;
}

int read(int file, void *ptr, size_t len) {

	if (file < 0) {
		errno = -EBADF;
		return -1;
	}

	if (file == 0 && isatty(1))
		fflush(stdout);

	int ret;
	if ((ret = sys_read(file, ptr, len)) < 0) {
		errno = -ret;
		printf("ret is %d", ret);
		return -1;
	}
	else
		return ret;
}

void *sbrk(ptrdiff_t incr) {
	
	void *ret = (void *)sys_sbrk(incr);
	if ((signed long)ret < 0 && (signed long)ret > -200) {
		errno = - ((int)ret);
		return (void *)(-1);
	}
	else {
		return ret;
	}
}

int stat(const char *file, struct stat *st) {

	if (st == NULL) {
		errno = EFAULT;
		return -1;
	}

	int ret = sys_stat(file, st);
	if (ret == 0)
		return 0;
	else {
		errno = -ret;
		return -1;
	}
}

clock_t times(struct tms *buf) {

	// TODO: times!
	return -1;
}

int unlink(const char *name) {
	
	// TODO: unlink!
	errno = ENOENT;
	return -1;
}

int wait(int *status) {
	
	int ret;
	if ((ret = sys_wait(status)) >= 0) {
		return ret;
	}
	else {
		errno = -ret;
		return -1;
	}
}

int waitpid(int pid, int *status, int options) {

	int ret;
	if ((ret = sys_waitpid(pid, status, options)) >= 0) {
		return ret;
	}
	else {
		errno = -ret;
		return -1;
	}
}
int write_(int file, const char* buf, int length)
{   
	int i;
	int ret;
	char *s;
	int iter = 0;
	int x = 0;
	int y = 0;
	const char *p = (const char *)buf;
	for ( i = 0; i < length && *p; i++) {
		//	if(*p == 'm')
		//			sys_clear();
			//if(p[0] == '2' && p[1] == '4')
		//			sys_clear();
				
					
		if(*p == '\033')
		{
			if(p[1] == '\133')
			{		
				
					
				char find = 'H';

				const char *ptr = strchr(p, find);
				int index = ptr - p;
				if(ptr) {
						iter++;
				// do something
						}
				find = 'D';

				ptr = strchr(p, find);
				int index2 = ptr - p;
				if(ptr) {
				iter++;
					}
	
				if(p[index2] == 'D' )
					{
						sscanf(p, "\033\133%dD", &x);
						sys_setx(1);
						sys_putchar(*p++);
	  
					}		//nästa escape
		
				if(p[index] == 'H')
				{	
					char find = 'H';

					const char *ptr = strchr(p, find);
					int index = ptr - p;
					if(ptr) {
					iter++;
					// do something
					}
					sscanf(p, "\033\133%d;%dH",&y, &x);
	
					sys_setxy(x,y);
		
					p = p + index+1;
					sys_putchar(*p++);
				}
	
			}
		}
	else
	 sys_putchar(*p++);
			
	 ret++;
}
return ret;
}

int write(int file, const void *ptr, size_t len) {
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
int write2(int file, const void *ptr, size_t len) {


	sys_write(file, ptr, len);
}

int gettimeofday(struct timeval *p, void *__tz) {

	int ret;
	if ((ret = sys_gettimeofday(p, __tz)) == 0)
		return ret;
	else {
		errno = -ret;
		return -1;
	}
}

int getdents(int fd, void *dp, int count) {

	int ret;
	if ((ret = sys_getdents(fd, dp, count)) >= 0) {
		return ret;
	}
	else {
		errno = -ret;
		return -1;
	}
}

int fork(void) {
	
	int ret;
	//if ((ret = sys_fork()) >= 0)
		//return ret;
	//else {
		//errno = -ret;
		//return -1;
	//}
}

int pipe(int fildes[2]) {

	int ret;
	if ((ret = sys_pipe(fildes)) == 0)
		return 0;
	else {
		errno = -ret;
		return -1;
	}
}


int vfork(void)
{

  int pid = fork();
	if (pid == 0) {
		/* In child. */
		return 0;
	}
	else {
		/* In parent.  Wait for child to finish. */
		return waitpid(pid, NULL, 0);
	}
}

int dup(int fd) {

	return sys_dup(fd);
}

int dup2(int fd, int fd2) {

	return sys_dup2(fd, fd2);
}

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
	int ret;

	if ((ret = sys_nanosleep(rqtp, rmtp)) != 0) {
		errno = -ret;
		return -1;
	}
	else
		return 0;
}

char *getcwd(char *__buf, size_t __size) {
	
	if (__buf != NULL && __size > 0) {
		// User specified a proper buffer: use it, and give up if it fails
		char *ret = sys_getcwd(__buf, __size);
		int r = (int)ret;
		if (r < 0 && r > -150) {
			// Treat as errno
			errno = -r;
			return NULL;
		}
		else
			return ret;
	}
	else if (__size == 0 && __buf != NULL) {
		// size == 0 is invalid; however we ignore it if buf == NULL
		errno = EINVAL;
		return NULL;
	}
	else if (__buf == NULL) {
		// Allocate a buffer to use
		char *buf = malloc(PATH_MAX + 1);
		char *ret = sys_getcwd(buf, PATH_MAX + 1);
		if ((int)ret < 0 && (int)ret > -150) {
			free(buf);
			errno = -((int)ret);
			return NULL;
		}
		else
			return ret;
	}
}


int access(const char *pathname, int mode) {
int result = sys_access((char *)pathname, mode);
if (result < 0) {
errno = ENOENT; /* XXX */
}
return result;
}


#define TCGETS 0x4000 /* Get termios struct */
#define TCSETS 0x4001 /* Set termios struct */
#define TCSETSW 0x4002 /* Set, but let drain first */
#define TCSETSF 0x4003 /* Set, but let flush first */
#define TCSANOW 0x0001
#define TCSADRAIN 0x0002
#define TCSAFLUSH 0x0004
int ioctl(int fd, int request, void * argp) {

return sys_ioctl(fd, request, argp);
}

int tcgetattr(int fd, struct termios * tio) {
return ioctl(fd, TCGETS, tio);
}

int tcsetattr(int fd, int actions, struct termios * tio) {
switch (actions) {
case TCSANOW:
return ioctl(fd, TCSETS, tio);
case TCSADRAIN:
return ioctl(fd, TCSETSW, tio);
case TCSAFLUSH:
return ioctl(fd, TCSETSF, tio);
default:
return 0;
}
}
mode_t umask(mode_t mask) {
//return syscall_umask(mask);
} 

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {		//must fix

sighandler_t old = signal(signum, act->sa_handler);
if (oldact) {
oldact->sa_handler = old;
}

return 0;
}

int lstat(const char *path, struct stat *buf) {
return stat(path, buf);
}

int chmod(const char *path, mode_t mode) {
//return syscall_chmod((char *)path, mode);
}
int chown(const char *path, uid_t owner, gid_t group) {

return 0;
}

int fchown(int fd, uid_t owner, gid_t group)
{
	return 0;
}
int fchmod(int fildes, mode_t mode)
{
}
struct passwd *getpwuid(uid_t uid)
{
	
}

int utime(const char *filename, const struct utimbuf *times) {
return 0;
}

int fpathconf(int file, int name) {

return 0;
}

int geteuid() {
return 0;//return getuid();
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {

return -1;
}
int execvp(const char *file, char *const argv[]) {
}

//DIR * opendir (const char * dirname) {
/*int fd = open(dirname, O_RDONLY);
if (fd == -1) {
return NULL;
}

DIR * dir = (DIR *)malloc(sizeof(DIR));
dir->fd = fd;
dir->cur_entry = -1;
return dir;*/
//}
//
//int closedir (DIR * dir) {
/*if (dir && (dir->fd != -1)) {
return close(dir->fd);
} else {
return -1;
}*/
//}

//struct dirent * readdir (DIR * dirp) {
/*static struct dirent ent;

int ret = syscall_readdir(dirp->fd, ++dirp->cur_entry, &ent);
if (ret != 0) {
memset(&ent, 0, sizeof(struct dirent));
return NULL;
}

return &ent;*/
//}
int execl(const char *path, const char *arg, ...)
{
}
int execlp(const char *file, const char *arg, ...)
{
}

char *ttyname(int fd) {

errno = ENOTTY;

return NULL;

} 
int mkdir(const char *pathname, mode_t mode) {

return 1; //syscall_mkdir((char *)pathname, mode);

} 
typedef struct
{
    size_t gl_pathc;    /* Count of paths matched so far  */
    char **gl_pathv;    /* List of matched pathnames.  */
    size_t gl_offs;     /* Slots to reserve in `gl_pathv'.  */
} glob_t;
int glob(const char *pattern, int flags,
         int errfunc,
         glob_t *pglob)
         {
		 }
void globfree(glob_t *pglob)
{
}
int scandir(const char *dirp, struct dirent ***namelist,
       int (*filter)(const struct dirent *),
       int (*compar)(const struct dirent **, const struct dirent **))
       {
		   printf("scandir called!");
	   }
	   
	   ssize_t readlink(const char *path, char *buf, size_t bufsiz)
	   {
	   }
	   
	 


void regfree(regex_t *preg)
{
}
int	regcomp(regex_t * preg, const char * pattern, int cflags)
{
}
size_t	regerror(int errcode, const regex_t * preg, char * errbuf, size_t errbuf_size)
{
}
int	regexec(const regex_t * preg, const char * string, size_t nmatch, regmatch_t  pmatch[] , int efags)
{
}

int rmdir(const char *path)
{
}


int symlink(const char *path1, const char *path2)
{
} 
char *getwd(char *buf) {

return 1; //getcwd(buf, 256);

}

void endpwent(void)
{
}
void getpwent()
{
	
}

void _longjmp()
{

}
void _setjmp()
{
	
}
void _popen()
{
	
}

void _pclose()
{

}
ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
}
static int dupfd(unsigned int fd, unsigned int arg)
{
}

int setpgid(pid_t pid, pid_t pgid)
{
}
uid_t getuid(void)
 {
 }
 gid_t getgid(void)
 {
 }
 
 int getdtablesize(void)
 {
 }
  int setresuid(uid_t ruid, uid_t euid, uid_t suid)
  {
  }
int setresgid(gid_t rgid, gid_t egid, gid_t sgid)
	{
	}
struct passwd *getpwnam(const char *name)
	{
	}

	char *getlogin(void)
	{
	}
/*
void *pthread_getspecific(pthread_key_t key)
{
}
int pthread_setspecific(pthread_key_t key, const void *value)
{
	
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
}
int fcntl(int fildes, int cmd, ...)
{
}
int execv(const char *path, char *const argv[])
{
}


int pthread_mutex_lock(pthread_mutex_t *mutex)
{
}
int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
}
int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
}
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
}
int pthread_cond_timedwait(pthread_cond_t *cond, 
    pthread_mutex_t *mutex, const struct timespec *abstime)
    {
	}
	
	int pthread_cond_signal(pthread_cond_t *cond)
	{
	}
int pthread_cond_broadcast(pthread_cond_t *cond)
{
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
{
}
int pthread_sigmask(int how, const sigset_t  set,
       sigset_t  oset)
       {
	   }
	   
	   long sysconf(int name)
	   {
	   }
	   int select( 	int nfds,
  	fd_set *readfds,
  	fd_set *writefds,
  	fd_set *exceptfds,
  	struct timeval *timeout)
  	{
	}
	

int pthread_mutex_init(pthread_mutex_t *mutex, 
    const pthread_mutexattr_t *attr)
    {
	 }
int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
	}
	int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
}
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
	}
	int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
}
int pthread_rwlock_init(pthread_rwlock_t *rwlock,
    const pthread_rwlockattr_t *attr)
    {
	}
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
}
int pthread_condattr_init(pthread_condattr_t *attr)
{
}
int pthread_condattr_destroy(pthread_condattr_t *attr)
{
}
int pthread_attr_init(pthread_attr_t *attr)
{
}
int pthread_attr_destroy(pthread_attr_t *attr)
{
}

int creat(const char *path, mode_t mode)
{
}

int gethostname(char *name, size_t namelen)
{
}
gid_t getegid(void)
{
}
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)
{
}
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
}


int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
 {
 }
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
}
int pthread_cond_init(pthread_cond_t *cond,
    const pthread_condattr_t *attr)
    {
	}
int pthread_cond_destroy(pthread_cond_t *cond)
{
}
int pthread_key_delete(pthread_key_t key)
{
}
int pthread_detach(pthread_t thread)
{
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine)(void*), void *arg)
    {
	}
	
int sched_yield(void)
{
}
int pthread_join(pthread_t thread, void **value_ptr)
{
}
void pthread_exit(void *value_ptr)
{
}
void rewinddir(DIR *dirp)
{
}
*/
