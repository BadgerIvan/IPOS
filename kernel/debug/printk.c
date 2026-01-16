#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <arch/drivers/tty.h>

#define MAX_NUM_STR_LEN 64
#define NUMBER_ASCII 128

#define UPPER 1
#define NOT_UPPER 0

typedef struct {
  int left_alignment;
  int sign;
  int space;
  int base;
  int fill;
} flags_t;

typedef struct {
  flags_t flags;
  size_t width;
  size_t precision;
  int precision_set;
  char length;
  char type;
} fmt_t;

char *parse_size_t_from_str(const char *str, size_t *result) {
  if (str == NULL || result == NULL) return NULL;
  *result = 0;
  char *buffer = (char *)str;
  while (isdigit((int)*buffer)) {
    *result = *result * 10 + (*buffer - '0');
    buffer++;
  }
  return buffer;
}

void reverse_str(char *str, int len) {
  for (int i = 0, j = len - 1; i < j; i++, j--) {
    char tmp = str[i];
    str[i] = str[j];
    str[j] = tmp;
  }
}

void size_t_to_string_with_base(char *str, size_t num, int base) {
  if (base < 2 || base > 36) {
    str[0] = '\0';
    return;
  }
  const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  int i = 0;
  if (num == 0) {
    str[i++] = '0';
  } else {
    while (num > 0) {
      str[i++] = digits[num % base];
      num /= base;
    }
  }
  str[i] = '\0';
  reverse_str(str, i);
}

void size_t_to_string(char *str, size_t num) {
  size_t_to_string_with_base(str, num, 10);
}

void to_upper_string(char *str) {
  while (*str) {
    if (*str >= 'a' && *str <= 'z') *str = toupper(*str);
    str++;
  }
}

void pointer_to_string(char *str, uintptr_t ptr) {
  if (ptr == (uintptr_t)NULL)
    strncpy(str, "(nil)", 6);
  else
    size_t_to_string_with_base(str, ptr, 16);
}

static void init_flags(flags_t *flags) {
  flags->left_alignment = 0;
  flags->sign = 0;
  flags->space = 0;
  flags->base = 0;
  flags->fill = 0;
}

static void init_fmt(fmt_t *fmt) {
  init_flags(&fmt->flags);
  fmt->width = 0;
  fmt->precision = 0;
  fmt->precision_set = 0;
  fmt->length = 0;
  fmt->type = 0;
}

static char *parse_flags(const char *format, flags_t *flags) {
  char *buffer = (char *)format;
  int flag = 1;
  while (*buffer && flag) {
    switch (*buffer) {
      case '-':
        flags->left_alignment = 1;
        break;
      case '+':
        flags->sign = 1;
        break;
      case ' ':
        flags->space = 1;
        break;
      case '#':
        flags->base = 1;
        break;
      case '0':
        flags->fill = 1;
        break;
      default:
        flag = 0;
    }
    if (flag) buffer++;
  }
  return buffer;
}

static void resolve_conflicts_flags(flags_t *flags) {
  if (flags->left_alignment) flags->fill = 0;
  if (flags->sign) flags->space = 0;
}

static char *parse_width(const char *format, va_list* args, size_t *result) {
  char *buffer = (char *)format;
  if (isdigit(*buffer))
    buffer = parse_size_t_from_str(buffer, result);
  else if (*buffer == '*') {
    *result = (size_t)va_arg(*args, size_t);
    buffer++;
  }
  return buffer;
}

static char *parse_precision(const char *format, va_list* args, size_t *result,
                             int *result_set) {
  if (*format != '.') return (char *)format;
  char *buffer = (char *)format;
  buffer++;
  if (isdigit(*buffer)) {
    buffer = parse_size_t_from_str(buffer, result);
  } else if (*buffer == '*') {
    *result = (size_t)va_arg(*args, size_t);
    buffer++;
  }
  *result_set = 1;
  return buffer;
}

static char *parse_length(const char *format, char *result) {
  char *buffer = (char *)format;
  if (*format == 'h' || *format == 'l' || *format == 'L') *result = *buffer++;
  return buffer;
}

static char *parse_type(const char *format, char *result) {
  char *buffer = (char *)format;
  *result = *buffer;
  return ++buffer;
}

static char *parse_all_in_flag(const char *format, va_list* args, fmt_t *fmt) {
  char *buffer = (char *)format;
  buffer = parse_flags(buffer, &fmt->flags);
  resolve_conflicts_flags(&fmt->flags);
  buffer = parse_width(buffer, args, &fmt->width);
  buffer = parse_precision(buffer, args, &fmt->precision, &fmt->precision_set);
  buffer = parse_length(buffer, &fmt->length);
  buffer = parse_type(buffer, &fmt->type);
  return buffer;
}

static int format_char_with_width_and_left_alignment(char ch, size_t width) {
  int offset = 0;
  terminal_putchar(ch);
  offset++;
  for(size_t i = 0; i < width; i++)
    terminal_putchar(' ');
  offset += width - 1;
  return offset;
}

static int format_char_with_width(char ch, size_t width) {
  int offset = 0;
  for(size_t i = 0; i < width; i++)
    terminal_putchar(' ');
  offset += width - 1;
  terminal_putchar(ch);
  offset++;
  return offset;
}

static int format_char(char ch) {
  terminal_putchar(ch);
  return 1;
}

static void process_char(va_list* args, fmt_t *fmt, size_t *len) {
  char ch = (char)va_arg(*args, int);
  if (fmt->width && fmt->flags.left_alignment) {
    *len += format_char_with_width_and_left_alignment(ch, fmt->width);
  } else if (fmt->width) {
    *len += format_char_with_width(ch, fmt->width);
  } else
    *len += format_char(ch);
}

static char *get_processed_string(char *str, fmt_t *fmt) {
  if (str != NULL) return str;
  str = "(null)";
  if (fmt->precision_set && fmt->precision <= strlen("(null)") - 1) str = "";
  return str;
}

static size_t calculate_string_len(const char *str, fmt_t *fmt) {
  size_t result = strlen(str);
  if (fmt->precision_set)
    result = result > fmt->precision ? fmt->precision : result;
  return result;
}

static size_t calculate_padding(fmt_t *fmt, size_t string_len) {
  size_t padding = 0;
  if (fmt->width > string_len) padding = fmt->width - string_len;
  return padding;
}

static size_t format_string_with_left_alignment(const char *string, size_t string_len, size_t padding) {
  size_t offset = 0;
  terminal_write(string, string_len);
  offset += string_len;
  if (padding) {
    for(size_t i = 0; i < padding; i++)
      terminal_putchar(' ');
    offset += padding;
  }
  return offset;
}

static size_t format_string(const char *string, size_t string_len, size_t padding) {
  size_t offset = 0;
  if (padding) {
    for(size_t i = 0; i < padding; i++)
      terminal_putchar(' ');
    offset += padding;
  }
  terminal_write(string, string_len);
  offset += string_len;
  return offset;
}

static void process_string(va_list* args, fmt_t *fmt, size_t *len) {
  char *string = (char *)va_arg(*args, char *);
  string = get_processed_string(string, fmt);
  size_t string_len = calculate_string_len(string, fmt);
  size_t padding = calculate_padding(fmt, string_len);
  if (fmt->flags.left_alignment)
    *len += format_string_with_left_alignment(string, string_len, padding);
  else
    *len += format_string(string, string_len, padding);
}

static char get_sign(int is_negative, fmt_t *fmt) {
  char sign = 0;
  if (is_negative)
    sign = '-';
  else if (fmt->flags.sign)
    sign = '+';
  else if (fmt->flags.space)
    sign = ' ';
  return sign;
}

static void handle_zero_precision_case(char *num_string, size_t num, fmt_t *fmt) {
  if (fmt->precision_set && fmt->precision == 0 && num == 0)
    num_string[0] = '\0';
}

static size_t get_num_zeros(size_t len_buffer, fmt_t *fmt) {
  size_t num_zeros = 0;
  if (fmt->precision_set && fmt->precision > len_buffer)
    num_zeros = fmt->precision - len_buffer;
  return num_zeros;
}

static size_t get_padding(size_t total_len, fmt_t *fmt) {
  return fmt->width > total_len ? fmt->width - total_len : 0;
}

static char get_fill(fmt_t *fmt) {
  return (fmt->flags.fill && !fmt->flags.left_alignment && !fmt->precision_set) ? '0' : ' ';
}

static size_t format_integer_with_left_alignment(const char *num_string, char sign, size_t num_string_len, 
                                                size_t num_zeros, size_t padding) {
  size_t offset = 0;
  if (sign) terminal_putchar(sign);
  if (num_zeros) {
    for(size_t i = 0; i < num_zeros; i++) terminal_putchar('0');
    offset += num_zeros;
  }
  terminal_write(num_string, num_string_len);
  offset += num_string_len;
  if (padding) {
    for(size_t i = 0; i < padding; i++) terminal_putchar(' ');
    offset += padding;
  }
  return offset;
}

static size_t format_integer_without_left_alignment(const char *num_string, char sign, size_t num_string_len,
                                                    size_t num_zeros, size_t padding, char fill) {
  size_t offset = 0;
  if (fill == ' ') {
    for(size_t i = 0; i < padding; i++) terminal_putchar(fill);
    offset += padding;
  }
  if (sign) terminal_putchar(sign);
  if (fill == '0') {
    for(size_t i = 0; i < padding; i++) terminal_putchar(fill);
    offset += padding;
  }
  if (num_zeros) {
    for(size_t i = 0; i < num_zeros; i++) terminal_putchar('0');
    offset += num_zeros;
  }
  terminal_write(num_string, num_string_len);
  offset += num_string_len;
  return offset;
}

static void format_integer(char *num_string, size_t num, int is_negative, fmt_t *fmt, size_t *len) {
  char sign = get_sign(is_negative, fmt);
  handle_zero_precision_case(num_string, num, fmt);
  size_t len_buffer = strlen(num_string);
  size_t num_zeros = get_num_zeros(len_buffer, fmt);

  size_t total_len = len_buffer + num_zeros;
  if (sign) total_len++;

  size_t padding = get_padding(total_len, fmt);
  char fill = get_fill(fmt);

  if (fmt->flags.left_alignment)
    *len += format_integer_with_left_alignment(num_string, sign, len_buffer, num_zeros, padding);
  else
    *len += format_integer_without_left_alignment(num_string, sign, len_buffer, num_zeros, padding, fill);
}

static void process_int(va_list* args, fmt_t *fmt, size_t *len) {
  long int num = 0;
  if (fmt->length == 'h')
    num = (long int)((short int)va_arg(*args, int));
  else if (fmt->length == 'l')
    num = va_arg(*args, long);
  else
    num = (long int)va_arg(*args, int);
  int is_negative = 0;
  if (num < 0) {
    num = -num;
    is_negative = 1;
  }
  char num_string[MAX_NUM_STR_LEN];
  size_t_to_string(num_string, num);
  format_integer(num_string, num, is_negative, fmt, len);
}

static char *make_prefix_for_base(char *buffer, int base, fmt_t *fmt) {
  if (!fmt->flags.base) return buffer;
  int len = 0;
  if (base == 8) {
    buffer[0] = '0';
    len = 1;
  } else {
    strncpy(buffer, "0x", 3);
    len = 2;
  }
  return buffer + len;
}

static void process_unsigned_integer(int base, int upper, va_list* args, fmt_t *fmt, size_t *len) {
  unsigned long int num = 0;
  if (fmt->length == 'h')
    num = (unsigned long int)((unsigned short int)va_arg(*args, unsigned int));
  else if (fmt->length == 'l')
    num = va_arg(*args, unsigned long int);
  else
    num = (unsigned long int)va_arg(*args, unsigned int);
  char num_string[MAX_NUM_STR_LEN];
  char *num_string_with_prefix = make_prefix_for_base(num_string, base, fmt);
  size_t_to_string_with_base(num_string_with_prefix, num, base);
  if (upper) to_upper_string(num_string);
  format_integer(num_string, num, 0, fmt, len);
}

static void process_unsigned_int(va_list* args, fmt_t *fmt, size_t *len) {
  process_unsigned_integer(10, NOT_UPPER, args, fmt, len);
}
static void process_hex_int(va_list* args, fmt_t *fmt, size_t *len) {
  process_unsigned_integer(16, NOT_UPPER, args, fmt, len);
}

static void process_hex_int_with_upper(va_list* args, fmt_t *fmt, size_t *len) {
  process_unsigned_integer(16, UPPER, args, fmt, len);
}

static void process_octal_int(va_list* args, fmt_t *fmt, size_t *len) {
  process_unsigned_integer(8, NOT_UPPER, args, fmt, len);
}

static void change_flags_to_correct_for_pointer(fmt_t *fmt) {
  fmt->flags.base = 1;
}

static void process_pointer(va_list* args, fmt_t *fmt, size_t *len) {
  uintptr_t ptr = va_arg(*args, uintptr_t);
  change_flags_to_correct_for_pointer(fmt);
  char num_string[MAX_NUM_STR_LEN];
  char *num_string_with_prefix = num_string;
  if (ptr != (uintptr_t)NULL)
    num_string_with_prefix = make_prefix_for_base(num_string, 16, fmt);
  pointer_to_string(num_string_with_prefix, ptr);
  format_integer(num_string, ptr, 0, fmt, len);
}

static void process_procent(__attribute__((unused)) va_list* args,
                            __attribute__((unused)) fmt_t *fmt, size_t *len) {
  terminal_putchar('%');
  (*len)++;
}

static void process_number(va_list* args, fmt_t *fmt, size_t *len) {
  long int *num = 0;
  if (fmt->length == 'l')
    num = (long int *)va_arg(*args, long int *);
  else if (fmt->length == 'h')
    num = (long int *)((short int *)va_arg(*args, int *));
  else
    num = (long int *)va_arg(*args, int *);
  *num = (long int)*len;
}

static void process_type(va_list* args, fmt_t *fmt, size_t *len) {
  switch (fmt->type)
  {
  case 'c':
    process_char(args, fmt, len);
    break;
  case 's':
    process_string(args, fmt, len);
    break;
  case 'd':
  case 'i':
    process_int(args, fmt, len);
    break;
  case 'u':
    process_unsigned_int(args, fmt, len);
    break;
  case 'x':
    process_hex_int(args, fmt, len);
    break;
  case 'X':
    process_hex_int_with_upper(args, fmt, len);
    break;
  case 'o':
    process_octal_int(args, fmt, len);
    break;
  case 'p':
    process_pointer(args, fmt, len);
    break;
  case '%':
    process_procent(args, fmt, len);
    break;
  case 'n': 
    process_number(args, fmt, len);
    break;
  default:
    break;
  }
}

int printk(const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *fmt_str = (char *)format;
  size_t len = 0;
  while (*fmt_str) {
    if (*fmt_str != '%')
      terminal_putchar(*fmt_str++);
    else {
      fmt_str++;
      fmt_t fmt;
      init_fmt(&fmt);
      fmt_str = parse_all_in_flag(fmt_str, &args, &fmt);
      process_type(&args, &fmt, &len);
    }
  }
  va_end(args);
  return len;
}