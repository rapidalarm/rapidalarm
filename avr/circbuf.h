#include "config.h"

typedef struct {
    int8_t head;
    int16_t values[HIST_SIZE];
} circbuf;

// advance buffer head by n positions
void advancebuf(circbuf *buf, int8_t n) {
    buf->head += n;
    buf->head = buf->head % HIST_SIZE;
}

// advance head and write a single value to the circular buffer
void writebuf(circbuf *buf, int16_t value) {
    advancebuf(buf, 1);
    buf->values[buf->head] = value;
}

// advance head and get a pointer to the current buffer value
int16_t* pointbuf(circbuf *buf) {
    advancebuf(buf, 1);
    return &buf->values[buf->head];
}

// retrieve a value from the buffer n items in the past
int16_t getbuf(circbuf *buf, int8_t n) {
    return buf->values[(buf->head - n) % HIST_SIZE];
}
