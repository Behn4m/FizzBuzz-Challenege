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

#define STACK_SIZE 2048


/**
 * This is an example which echos any data it receives on configured UART back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */

#define ECHO_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
#define ECHO_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (CONFIG_EXAMPLE_UART_PORT_NUM)
#define ECHO_UART_BAUD_RATE     (CONFIG_EXAMPLE_UART_BAUD_RATE)
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

static const char *TAG = "FizzBuzz";

#define BUF_SIZE (1024)

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

void timer_callback(void* arg) 
{
    static int countdown = 0;
    
    if (countdown == 0) 
    {
        countdown = 100;
    }
    else 
    {
        switch (countdown % 2) 
        {
            case 0:
                printf("Fizz\n");
                break;
            case 1:
                printf("Buzz\n");
                break;
        }

        if (isPrime(countdown) == true) 
        {
            printf("Prime\n");
        }
    }
    countdown--;
               
}

void serialTask(void *param) 
{
    char buffer[16];
    int num = 0;

    // Create a timer 
    esp_timer_create_args_t timer_args = {.callback = timer_callback,
                                          NULL,
                                          .name = "timer_task"
                                         };
    static esp_timer_handle_t timer;
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
                // Start timer with 1 second period
                esp_timer_start_periodic(timer, 1000000); // 1 second in microseconds
            }
        }
    }
}

void app_main() 
{
    // Initialize UART
    UART_config();

    // Create tasks
    xTaskCreate(serialTask, "SerialTask", STACK_SIZE, NULL, 1, NULL);

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
