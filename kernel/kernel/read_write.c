#include <stdio.h>

#include <arch/drivers/tty.h>
#include <arch/drivers/serial.h>

#define MAX_STREAMS 1024

FILE streams[MAX_STREAMS];

FILE stdout_stream = {
    .id = 1,
    .buffer = NULL,
    .size_buffer = 0,
    .position = 0,
    .read = NULL,
    .write = terminal_write,
    .flush = NULL
};

void init_streams() {
    stdout = &stdout_stream;
    register_stream(stdout);
}

int register_stream(FILE *stream) {
    for (int i = 0; i < MAX_STREAMS; i++) {
        if (streams[i].id == 0) { 
            streams[i] = *stream;
            return i; 
        }
    }
    return -1; 
}

int write(int stream_id, const char *buf, size_t size) {
    if (stream_id < 0 || stream_id >= MAX_STREAMS) return -1;
    return streams[stream_id].write(&streams[stream_id], buf, size);
}

int read(int stream_id, char *buf, size_t size) {
    if (stream_id < 0 || stream_id >= MAX_STREAMS) return -1;
    return streams[stream_id].read(&streams[stream_id], buf, size);
}