#include "CircularBuffer.h"
#include "esp_log.h"

static const char *TAG = "CBUFFER";
 
/**
 * @brief Creates a new CircularBuffer_t object.
 * @return A pointer to the newly created CircularBuffer_t object.
 */
CircularBuffer_t* createCircularBuffer(uint8_t size) 
{
    CircularBuffer_t *buffer = (CircularBuffer_t*)malloc(sizeof(CircularBuffer_t));
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    buffer->data = (int*)malloc(size * sizeof(uint8_t));
    if (buffer->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = 0;

    return buffer;
}

/**
 * @brief Frees the memory allocated for the CircularBuffer_t object.
 * @param buffer A pointer to the CircularBuffer_t object.
*/
void destroyCircularBuffer(CircularBuffer_t *buffer) {
    free(buffer->data);
    free(buffer);
}

/**
 * @brief Adds a new element to the buffer.
 * @param buffer A pointer to the CircularBuffer_t object.
 * @param value The value to be added to the buffer.
 */
void enqueue(CircularBuffer_t *buffer, int value) {
    if (buffer->size == BUFFER_SIZE) 
    {
        ESP_LOGD(TAG, "Buffer is currently full\n");
        return;
    }

    buffer->data[buffer->head] = value;
    buffer->head = (buffer->head + 1) % BUFFER_SIZE;
    buffer->size++;
    ESP_LOGD(TAG, "Enqueued %d", value);
}

/**
 * @brief Removes an element from the buffer.
 * @param buffer A pointer to the CircularBuffer_t object.
 * @return The value removed from the buffer.
 */
bool dequeue(CircularBuffer_t *buffer, int *value) 
{
    if (buffer->size == 0) 
    {
        ESP_LOGD(TAG, "Buffer underflow\n");
        return false;
    }

    *value = buffer->data[buffer->tail];
    buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
    buffer->size--;
    ESP_LOGD(TAG, "Dequeued %d", *value);
    return true;
}


int sample_app() {
    CircularBuffer_t *buffer = createCircularBuffer(100);

    // Example usage
    for (int i = 0; i < 110; ++i) {
        enqueue(buffer, i);
    }

    for (int i = 0; i < 100; ++i) {
        // ESP_LOGI(TAG, "%d ", dequeue(buffer, int newValue));
    }

    destroyCircularBuffer(buffer);

    return 0;
}
