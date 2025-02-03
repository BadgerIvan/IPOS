#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char *convert(unsigned int num, int base) 
{ 
    static const char Representation[]= "0123456789ABCDEF";
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

int vfprintf(FILE *stream, const char* format, va_list arg) {
	char *traverse; 
    unsigned int ui;
    int i; 
    char *s; 
    int count = 0;

    for(traverse = format; *traverse != '\0'; traverse++) 
    { 
        while( *traverse != '%' && *traverse != '\0') 
        { 
            fputc(*traverse, stream);
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
            fputc(i, stream);
            count++;
            break; 

        case 'd' : 
        case 'i' :
            i = va_arg(arg,int);         
            if (i < 0) {
                 if (i == INT32_MIN) { 
                     i = -(i + 1); 
                 } else {
                    i = -i;
                }
                temp = convert(i, 10);
                fputc('-', stream);
                fputs(temp, stream);
                count += strlen(temp) + 1; 
            } else {
                temp = convert(i, 10);
                fputs(temp, stream);
                count += strlen(temp);
            }
            break;

        case 'u':
            ui = va_arg(arg, unsigned int);
            temp = convert(ui, 10);
            fputs(temp, stream);
            count += strlen(temp);
            break;

        case 'o': 
            ui = va_arg(arg,unsigned int);
            temp = convert(ui,8);
            fputs(temp, stream);
            count += strlen(temp);
            break; 

        case 's': 
            s = va_arg(arg,char *);      
            fputs(s, stream);
            count += strlen(s); 
            break; 

        case 'x': 
            ui = va_arg(arg,unsigned int); 
            temp = convert(ui, 16);
            fputs(temp, stream);
            count += strlen(temp);
            break; 
        }   
    } 
     
	return count;
}
