#include <stdint.h>
#include <stdio.h>

#include <arch/drivers/io.h>

int init_serial(uint16_t port) {
    int flag = 1;
    outb (port + 1 , 0x00); 
    outb (port + 3 , 0x80); 
    outb (port + 0 , 0x03); 
    outb (port + 1 , 0x00); 
    outb (port + 3 , 0x03); 
    outb (port + 2 , 0xC7); 
    outb (port + 4 , 0x0B); 
    outb (port + 4 , 0x1E); 
    outb (port + 0 , 0xAE); 
    
    if (inb (port) != 0xAE) {        
        flag = 0; 
    }

    if(flag)
        outb (port + 4 , 0x0F);   
    
    return flag;
}

void serial_write_char(uint16_t port, char a) {
   while ((inb(port + 5) & 0x20) == 0);
   outb(port, a);
}

void serial_write(uint16_t port, const char *buffer, size_t size) {
    for(size_t i = 0; i < size; i++) {
        serial_write_char(buffer[i], port);
        i++;
    }
}

char serial_read(uint16_t port, char *buffer, size_t size) {
   while ((inb(port + 5) & 1) == 0);
   return inb(port);
}
