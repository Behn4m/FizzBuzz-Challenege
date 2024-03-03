#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE 100

typedef struct {
    int *data;
    int head;
    int tail;
    int size;
} CircularBuffer_t;

/**
 * Create a new circular buffer
 * @return a pointer to the new buffer
 */
CircularBuffer_t* createCircularBuffer();

/**
 * Destroy a circular buffer
 * @param buffer the buffer to destroy
 */
void destroyCircularBuffer(CircularBuffer_t *buffer);

/**
 * Enqueue a value into the buffer
 * @param buffer the buffer to enqueue into
 * @param value the value to enqueue
 */
void enqueue(CircularBuffer_t *buffer, int value);

/**
 * @brief Removes an element from the buffer.
 * @param buffer A pointer to the CircularBuffer_t object.
 * @return The value removed from the buffer.
 */
bool dequeue(CircularBuffer_t *buffer, int *value);

#endif /* CIRCULAR_BUFFER_H */
