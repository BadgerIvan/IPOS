#ifndef _READ_WRITE_H
#define _READ_WRITE_H

#include <stdio.h>

void init_streams();

int open_stream(int stream_id);

void close_stream(int stream_id);

int write(int stream_id, const void *buf, size_t size);

int read(int stream_id, void *buf, size_t size);

#endif