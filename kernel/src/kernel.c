#include <types.h>
#include <video.h>

struct multiboot
{
   u32 flags;
   u32 mem_lower;
   u32 mem_upper;
   u32 boot_device;
   u32 cmdline;
   u32 mods_count;
   u32 mods_addr;
   u32 num;
   u32 size;
   u32 addr;
   u32 shndx;
   u32 mmap_length;
   u32 mmap_addr;
   u32 drives_length;
   u32 drives_addr;
   u32 config_table;
   u32 boot_loader_name;
   u32 apm_table;
   u32 vbe_control_info;
   u32 vbe_mode_info;
   u32 vbe_mode;
   u32 vbe_interface_seg;
   u32 vbe_interface_off;
   u32 vbe_interface_len;
}  __attribute__((packed));


void kmain(struct multiboot *mbp, u32 magic)
{
	  init_video_term();
	  char *text = "hello world";
	  kputs("\n\nmuu");
	  kputhex(magic);
	  int tal = 100;
	  kputint(tal);
	  kprint("%d, %x, %s",tal,magic,text);
}
