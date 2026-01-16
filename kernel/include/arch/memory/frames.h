#ifndef _FRAMES_H
#define _FRAMES_H

#include <stdint.h>
#include <multiboot.h>

void init_frames(multiboot_info_t* mbd);
uint32_t alloc_frame(); //return frame_phys_addr
void free_frame(uint32_t frame_phys_addr);

#endif