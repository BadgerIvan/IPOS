#include <stdio.h>

#include <arch/drivers/tty.h>
#include <arch/drivers/serial.h>

#define MAX_STREAMS 1024

int std_streams = 0;

FILE *streams[MAX_STREAMS];

FILE stdout_stream = {
    .id = 0,
    .buffer = NULL,
    .size_buffer = 0,
    .position = 0,
    .read = NULL,
    .write = terminal_write,
    .flush = NULL
};

FILE stderr_stream = {
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
    streams[0] = stdout;
    std_streams++;

    stderr = &stderr_stream;
    streams[1] = stderr;
    std_streams++;
}

int open_stream(FILE *stream) {
    for (int i = std_streams; i < MAX_STREAMS; i++) {
        if (streams[i] == NULL || streams[i]->id == 0) {
            stream->id = i;
            streams[i] = stream;
            return i;
        }
    }
    return -1; 
}

void close_stream(const FILE *stream) {
    streams[stream->id] = NULL;
}

int write(int stream_id, const void *buf, size_t size) {
    if (stream_id < 0 || stream_id >= MAX_STREAMS || streams[stream_id] == NULL) 
        return -1;
    return streams[stream_id]->write(streams[stream_id], buf, size);
}

int read(int stream_id, void *buf, size_t size) {
    if (stream_id < 0 || stream_id >= MAX_STREAMS || streams[stream_id] == NULL) 
        return -1;
    return streams[stream_id]->read(streams[stream_id], buf, size);
}