#include <stdio.h>

void init_streams();

int open_stream(FILE *stream);

int close_stream(FILE *stream);

int write(int stream_id, const char *buf, size_t size);

int read(int stream_id, char *buf, size_t size);