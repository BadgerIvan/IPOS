//#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char *convert(unsigned int num, int base) 
{ 
    static char Representation[]= "0123456789ABCDEF";
    static char buffer[50]; 
    char *ptr; 

    ptr = &buffer[49]; 
    *ptr = '\0'; 

    do 
    { 
        *--ptr = Representation[num%base]; 
        num /= base; 
    }while(num != 0); 

    return(ptr); 
}

int printf(const char* restrict format, ...) {
	char *traverse; 
    unsigned int i; 
    char *s; 
    int count = 0;

    va_list arg; 
    va_start(arg, format); 

    for(traverse = format; *traverse != '\0'; traverse++) 
    { 
        while( *traverse != '%' && *traverse != '\0') 
        { 
            putchar(*traverse);
            count++;
            traverse++; 
        } 

        if (*traverse == '\0') break;

        traverse++; 

        char* temp = NULL;

        switch(*traverse) 
        { 
        case 'c' : 
            i = va_arg(arg,int);     
            putchar(i);
            count++;
            break; 

        case 'd' : 
            i = va_arg(arg,int);         
            if (i < 0) {
                // if (i == INT_MIN) { 
                //     putchar('-');
                //     i = -(i + 1); 
                //     temp = convert(i, 10);
                //     puts(temp);
                //     count += strlen(temp) + 1; 
                // } else {
                    putchar('-');
                    i = -i;
                    temp = convert(i, 10);
                    puts(temp);
                    count += strlen(temp) + 1;
                //}
            } else {
                temp = convert(i, 10);
                puts(temp);
                count += strlen(temp);
            }
            break;
            
            case 'o': 
                i = va_arg(arg,unsigned int);
                temp = convert(i,8);
                puts(temp);
                count += strlen(temp);
                break; 

            case 's': 
                s = va_arg(arg,char *);      
                puts(s);
                count += strlen(s); 
                break; 

            case 'x': 
                i = va_arg(arg,unsigned int); 
                temp = convert(i, 16);
                puts(temp);
                count += strlen(temp);
                break; 
        }   
    } 

    va_end(arg); 
	return count;
}
