#ifndef SYSCALLS_H
#define SYSCALLS_H
#define DECL_SYSCALL0(fn) int sys_##fn();
#define DECL_SYSCALL1(fn,p1) int sys_##fn(p1);
#define DECL_SYSCALL2(fn,p1,p2) int sys_##fn(p1,p2);
#define DECL_SYSCALL3(fn,p1,p2,p3) int sys_##fn(p1,p2,p3);
#define DECL_SYSCALL4(fn,p1,p2,p3,p4) int sys_##fn(p1,p2,p3,p4);
#define DECL_SYSCALL5(fn,p1,p2,p3,p4,p5) int sys_##fn(p1,p2,p3,p4,p5);

#define DEFN_SYSCALL0(fn, num) \
int sys_##fn() \
{ \
 int a; \
 __asm__ __volatile__("int $0x80" : "=a" (a) : "0" (num)); \
 return a; \
}

#define DEFN_SYSCALL1(fn, num, P1) \
int sys_##fn(P1 p1) \
{ \
 int a; \
  __asm__ __volatile__("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1)); \
 return a; \
}

#define DEFN_SYSCALL2(fn, num, P1, P2) \
int sys_##fn(P1 p1, P2 p2) \
{ \
 int a; \
  __asm__ __volatile__("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2)); \
 return a; \
}

#define DEFN_SYSCALL3(fn, num, P1, P2, P3) \
int sys_##fn(P1 p1, P2 p2, P3 p3) \
{ \
 int a; \
  __asm__ __volatile__("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2),  "d" ((int)p3)); \
 return a; \
}


DECL_SYSCALL1(puts,  const char *)
DECL_SYSCALL0(cli)

DEFN_SYSCALL0(cli, 0);
DEFN_SYSCALL1(puts,1, const char *);

void _install_syscall_handler();
#define S_IFCHR  0020000
struct	stat 
{
  /*dev_t		st_dev;
  ino_t		st_ino;*/
  int	st_mode;
 /* nlink_t	st_nlink;
  uid_t		st_uid;
  gid_t		st_gid;
  dev_t		st_rdev;*/
  int		st_size;
  int     st_blksize;
};

#endif
