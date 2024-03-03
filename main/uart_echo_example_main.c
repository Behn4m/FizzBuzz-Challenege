/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "CircularBuffer.h"

#define STACK_SIZE 2048

#define FOO_TASK_CORE 0
#define BAR_TASK_CORE 0
#define PRIME_TASK_CORE 0

#define ECHO_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
#define ECHO_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (CONFIG_EXAMPLE_UART_PORT_NUM)
#define ECHO_UART_BAUD_RATE     (CONFIG_EXAMPLE_UART_BAUD_RATE)
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

static const char *TAG = "FizzBuzz";
#define BUF_SIZE (1024)

// Timer handle
static esp_timer_handle_t timer;


void EvenFooTask(void *pvParameters) 
{
    int *num = (int *)pvParameters;
    printf("\nFoo %d", *num);
    vTaskDelete(NULL);
}

void OddBarTask(void *pvParameters) 
{
    int *num = (int *)pvParameters;
    printf("\nBar %d", *num);
    vTaskDelete(NULL);
}

void PrimeTask() 
{
    printf(" Prime");
    vTaskDelete(NULL);
}

static void UART_config()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
}

// Function to check if a number is prime
bool isPrime(int number) {
    if (number <= 1) {
        return false;
    }
    for (int i = 2; i * i <= number; i++) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}

// Timer callback function
void timer_callback(void* arg) 
{
    CircularBuffer_t *circularBuffer = (CircularBuffer_t *)arg;
    static int countdown = 0;
    
    if (countdown == 0) 
    {
        if (dequeue(circularBuffer, &countdown) == false) 
        {
            //ESP_LOGE(TAG, "Buffer underflow\n");
            esp_timer_stop(timer);
        } 
        else
        {
            ESP_LOGI(TAG, "new value dequeued %d", countdown);
        }
    }
    else
    {
        countdown--;

        switch (countdown % 2) 
        {
            case 0:
                xTaskCreatePinnedToCore(EvenFooTask, "EvenFooTask", 4096, &countdown, 1, NULL, FOO_TASK_CORE);
                break;
            case 1:
                xTaskCreatePinnedToCore(OddBarTask, "OddBarTask", 4096, &countdown, 1, NULL, BAR_TASK_CORE);
                break;
        }

        if (isPrime(countdown) == true) 
        {
            xTaskCreatePinnedToCore(PrimeTask, "PrimeTask", 4096, NULL, 1, NULL, PRIME_TASK_CORE);
        }
    }
    
               
}

void serialTask(void *param) 
{
    CircularBuffer_t *circularBuffer = (CircularBuffer_t *)param;
    char buffer[16];
    int num = 0;

    // Create a timer 
    esp_timer_create_args_t timer_args = {.callback = timer_callback,
                                          .arg = circularBuffer,
                                          .name = "timer_task"
                                         };
    esp_timer_create(&timer_args, &timer);
    
    while (1) 
    {
        if (uart_read_bytes(ECHO_UART_PORT_NUM, buffer, sizeof(buffer), pdMS_TO_TICKS(200)) > 0) 
        {
            num = atoi(buffer);
            ESP_LOGI(TAG, "Received %d", num);
            if (num == 0) 
            {
                esp_restart();
            } 
            else if (num > 0) 
            {
                enqueue(circularBuffer, num);
                // Start timer with 1 second period
                esp_timer_start_periodic(timer, 1000000); // 1 second in microseconds
            }
        }
    }
}

void app_main() 
{
    CircularBuffer_t *CircularBuffer = createCircularBuffer();

    // Initialize UART
    UART_config();

    // Create tasks
    xTaskCreate(serialTask, "SerialTask", STACK_SIZE, CircularBuffer, 1, NULL);

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

