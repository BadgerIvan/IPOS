#ifndef _FRAMES_H
#define _FRAMES_H

#include <multiboot.h>

void init_frames();
void mark_with_mmap(multiboot_info_t* mbd);

#endif