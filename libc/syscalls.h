#ifndef SYSCALLS_H
#define SYSCALLS_H
#define DECL_SYSCALL0(fn, ret) ret sys_##fn(void);
#define DECL_SYSCALL1(fn, ret, p1) ret sys_##fn(p1);
#define DECL_SYSCALL2(fn, ret, p1,p2) ret sys_##fn(p1,p2);
#define DECL_SYSCALL3(fn, ret, p1,p2,p3) ret sys_##fn(p1,p2,p3);
#define DECL_SYSCALL4(fn, ret, p1,p2,p3,p4) ret sys_##fn(p1,p2,p3,p4);
#define DECL_SYSCALL5(fn, ret, p1,p2,p3,p4,p5) ret sys_##fn(p1,p2,p3,p4,p5);

#define DEFN_SYSCALL0(fn, ret, num) \
ret sys_##fn(void) \
{ \
  ret a; \
  __asm__ volatile("int $0x80" : "=a" (a) : "0" (num)); \
  return a; \
}

#define DEFN_SYSCALL1(fn, ret, num, P1) \
ret sys_##fn(P1 p1) \
{ \
  ret a; \
  __asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1)); \
  return a; \
}

#define DEFN_SYSCALL2(fn, ret, num, P1, P2) \
ret sys_##fn(P1 p1, P2 p2) \
{ \
  ret a; \
  __asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1), "c" ((ret)p2)); \
  return a; \
}

#define DEFN_SYSCALL3(fn, ret, num, P1, P2, P3) \
ret sys_##fn(P1 p1, P2 p2, P3 p3) \
{ \
  ret a; \
  __asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1), "c" ((ret)p2), "d"((ret)p3)); \
  return a; \
}

#define DEFN_SYSCALL4(fn, ret, num, P1, P2, P3, P4) \
ret sys_##fn(P1 p1, P2 p2, P3 p3, P4 p4) \
{ \
  ret a; \
  __asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1), "c" ((ret)p2), "d" ((ret)p3), "S" ((ret)p4)); \
  return a; \
}

#define DEFN_SYSCALL5(fn, ret, num, P1, P2, P3, P4, P5) \
ret sysc_##fn(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) \
{ \
  ret a; \
  __asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((ret)p1), "c" ((ret)p2), "d" ((ret)p3), "S" ((ret)p4), "D" ((ret)p5)); \
  return a; \
}
unsigned char sys_getchar();
unsigned char getch();
#endif
