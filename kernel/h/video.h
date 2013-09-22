#ifndef VIDEO_H_
#define VIDEO_H_
#include <types.h>
void clear(void);
init_video_term();
void kputch(char c);
void set_term_color(u8 color);
void kputs(char *text);
extern void kputhex(u32 n);
extern void kputint(u32 n);
void kprint(const char *fmt, ...);
#endif
