#include <stdint.h>

#include <arch/drivers/io.h>

int init_serial(uint16_t port) {
    outb ( port + 1 , 0x00 ); 
    outb ( port + 3 , 0x80 ); 
    outb ( port + 0 , 0x03 ); 
    outb ( port + 1 , 0x00 ); 
    outb ( port + 3 , 0x03 ); 
    outb ( port + 2 , 0xC7 ); 
    outb ( port + 4 , 0x0B ); 
    outb ( port + 4 , 0x1E ); 
    outb ( port + 0 , 0xAE ); 
    
    if ( inb ( port ) != 0xAE ) {        
      return 0; 
    }

    outb ( port + 4 , 0x0F );   
      
   return 1;       
}

void serial_write_char(char a, uint16_t port) {
   while ((inb(port + 5) & 0x20) == 0);
   outb(port, a);
}

void serial_write_string(const char* str, uint16_t port) {
    uint32_t i = 0;
    while(str[i] != '\0') {
        serial_write_char(str[i], port);
        i++;
    }
}

char serial_read(uint16_t port) {
   while ((inb(port + 5) & 1) == 0);
   return inb(port);
}