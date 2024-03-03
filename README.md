| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# FooBar Challenge project

This code aims to solve the FooBar challenge provided by XXX company. The code is designed to run multiple tasks on different cores of ESP32 family microcontrollers with precise timing. All tasks are initially assigned to core 0 for compatibility across all chips, but you can assign each task to any core by adjusting the core value to the specified macro.

example for esp32-s3:
```
#define FOO_TASK_CORE 0
#define BAR_TASK_CORE 1
#define PRIME_TASK_CORE 0 
```
## How to use example

### Hardware Required

The example can be run on any development board, that is based on the Espressif SoC. The board shall be connected to a computer with a single USB cable for flashing and monitoring. The external interface should have 3.3V outputs. You may
use e.g. 3.3V compatible USB-to-Serial dongle.

### Setup the Hardware

Connect the external serial interface to the board as follows.

```
  -----------------------------------------------------------------------------------------
  | Target chip Interface | Kconfig Option     | Default ESP Pin      | External UART Pin |
  | ----------------------|--------------------|----------------------|--------------------
  | Transmit Data (TxD)   | EXAMPLE_UART_TXD   | GPIO4                | RxD               |
  | Receive Data (RxD)    | EXAMPLE_UART_RXD   | GPIO5                | TxD               |
  | Ground                | n/a                | GND                  | GND               |
  -----------------------------------------------------------------------------------------
```
Note: Some GPIOs can not be used with certain chips because they are reserved for internal use. Please refer to UART documentation for selected target.

## Code Output

this is code output when `15` is sent over serial
```
I (289) main_task: Started on CPU0
I (289) main_task: Calling app_main()

I (1662639) FizzBuzz: Received 15

I (1663639) FizzBuzz: new value dequeued 15

Bar 15
Foo 14
Bar 13 Prime
Foo 12
Bar 11 Prime
Foo 10
Bar 9
Foo 8
Bar 7 Prime
Foo 6
Bar 5 Prime
Foo 4
Bar 3 Prime
Foo 2 Prime
Bar 1
Foo 0
E (1680639) FizzBuzz: Buffer underflow
```

## Troubleshooting

You are not supposed to see the echo in the terminal which is used for flashing and monitoring, but in the other UART configured through Kconfig can be used.
