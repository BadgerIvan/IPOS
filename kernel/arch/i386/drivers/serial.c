#include <stdint.h>
#include <stdio.h>

#include <arch/drivers/io.h>

int init_serial(uint16_t port) {
    outb (port + 1 , 0x00);
    io_wait();
    outb (port + 3 , 0x80);
    io_wait();
    outb (port + 0 , 0x03);
    io_wait();
    outb (port + 1 , 0x00);
    io_wait();
    outb (port + 3 , 0x03);
    io_wait();
    outb (port + 2 , 0xC7);
    io_wait();
    outb (port + 4 , 0x0B);
    io_wait();
    outb (port + 4 , 0x1E);
    io_wait();
    outb (port + 0 , 0xAE);
    io_wait();
    
    if (inb (port) != 0xAE) {        
        return 0;
    }

    outb (port + 4 , 0x0F);
    io_wait();
    
    return 1;
}

void serial_write_char(uint16_t port, char a) {
   while ((inb(port + 5) & 0x20) == 0);
   outb(port, a);
   io_wait();
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
