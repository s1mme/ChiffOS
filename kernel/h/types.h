#ifndef TYPES_H_
#define TYPES_H_


typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
typedef enum {false = 0, true = 1} bool;


#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;

struct regs
{
    unsigned int gs, fs, es, ds;      
    unsigned int  edi, esi, ebp, ebx, edx, ecx, eax;  
    unsigned int int_no, err_code;    
    unsigned int eip, cs, eflags, useresp,ss;  
}__attribute__((packed));

typedef struct regs regs_t;
#endif 
