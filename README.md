# MSP430FR2155 Project

This repository contains demonstration projects for the MSP430FR2155 microcontroller, including both bare-metal and FreeRTOS-based implementations.

## Overview

The MSP430FR2155 is a low-power microcontroller from Texas Instruments featuring FRAM (Ferroelectric Random Access Memory) technology. This project provides a comprehensive BSP (Board Support Package) and example applications demonstrating various peripherals and real-time operating system integration.

## Features

- **Bare-metal Demo**: Simple demonstration without RTOS
- **FreeRTOS Demo**: Real-time operating system integration with task management
- **BSP Layer**: Hardware abstraction layer for easy portability
- **Driver Support**: I2C, SPI, UART, Timer, CRC drivers
- **Common Components**: Key handling, circular buffer, USMART debugging tool

## Project Structure

```
MSP430_CCS_project/
├── demo/                      # Bare-metal demo
│   ├── BSP/                   # Board Support Package
│   │   ├── Board.c/h          # Board initialization and LED control
│   │   ├── driver/            # Peripheral drivers
│   │   │   ├── i2c/          # I2C driver
│   │   │   ├── spi/          # SPI driver
│   │   │   ├── uart/         # UART driver
│   │   │   ├── timer/        # Timer driver
│   │   │   └── crc/          # CRC driver
│   │   └── common/           # Common utilities
│   │       ├── key/          # Key handling
│   │       ├── trans_loopbuffer/  # Circular buffer
│   │       └── USMART/       # Debugging tool
│   ├── driverlib/            # TI DriverLib
│   ├── main.c                # Main application
│   └── targetConfigs/        # CCS target configuration
│
└── freertos_demo/            # FreeRTOS demo
    ├── Board.c/h             # Board support
    ├── freertos/             # FreeRTOS source files
    ├── driverlib/            # TI DriverLib
    ├── main.c                # FreeRTOS application
    └── targetConfigs/        # CCS target configuration
```

## Hardware Configuration

### Target MCU
- **Microcontroller**: MSP430FR2155 (32-pin VQFN package)
- **Memory**: FRAM-based memory
- **Clock**: 12 MHz external crystal (configurable)

### Peripherals Used

#### LEDs
- **Red LED**: P3.6
- **Yellow LED**: P3.7
- **Green LED**: P1.4
- **Blue LED**: P1.5

#### Communication Interfaces
- **UART (eUSCI_A1)**: P4.2 (RXD), P4.3 (TXD)
- **I2C (eUSCI_B0)**: P1.2 (SDA), P1.3 (SCL)
- **SPI (eUSCI_A1)**: Configurable (when SPI_FUNCTION is defined)

#### Other Peripherals
- **Timer B0**: Used for PWM and FreeRTOS tick generation
- **External Crystal**: 12 MHz on P2.6 (XIN), P2.7 (XOUT)

## Getting Started

### Prerequisites

- **Code Composer Studio (CCS)**: Version 12.0.0.9
- **MSP430FR2155 LaunchPad** or compatible development board
- **MSP430 DriverLib**: Included in the project
- **FreeRTOS**: Included in freertos_demo (for RTOS demo)

### Bare-metal Demo

The bare-metal demo demonstrates basic peripheral initialization and usage without an operating system:

- **Clock Configuration**: 12 MHz system clock initialization
- **UART Communication**: UART initialization and basic communication
- **LED Control**: Simple LED control functions
- **Interrupt Handling**: Basic interrupt service routines

**Key Features:**
- Direct hardware register access
- Simple polling-based operations
- Low memory footprint
- Suitable for simple applications

### FreeRTOS Demo

The FreeRTOS demo shows how to integrate a real-time operating system:

- **Task Management**: LED task demonstration
- **Timer Services**: Software timer callback example
- **Task Scheduling**: Preemptive multitasking
- **Stack Management**: Stack overflow detection hooks

**Key Features:**
- Preemptive task scheduling
- Software timers
- Queue and semaphore support
- Memory management hooks
- Tick interrupt configuration

**Tasks in FreeRTOS Demo:**
- `LED_task`: Blinks LEDs with configurable timing
- `Check Timer`: Periodic callback for system monitoring

## Configuration Options

### Board.h Configuration

Key configuration options in `Board.h`:

```c
#define REDEFINE_PRINTF      // Redirect printf to UART
#define USMART              // Enable USMART debugging tool
#define SPI_FUNCTION        // Enable SPI instead of UART (optional)
```

### Clock Configuration

- **MCLK Frequency**: 12 MHz (default)
- **SMCLK Frequency**: 6 MHz (for FreeRTOS tick)
- **External Crystal**: 12 MHz

### FreeRTOS Configuration

FreeRTOS settings can be modified in `FreeRTOSConfig.h`:
- Tick rate: `configTICK_RATE_HZ`
- Heap size: `configTOTAL_HEAP_SIZE`
- Task priorities and stack sizes

## API Reference

### LED Control
```c
void ledControl(uint16_t led_type, uint8_t state);
// led_type: LED_RED, LED_YELLOW, LED_GREEN, LED_BLUE
// state: LED_ON, LED_OFF, LED_SWITCH
```

### Clock Initialization
```c
void init_clock_12Mhz(void);
void init_clockDCO_16Mhz(void);
```

### UART Functions
```c
void init_uartA1(void);
// Use standard printf() when REDEFINE_PRINTF is defined
```

## Troubleshooting

### Common Issues

1. **Build Errors**
   - Ensure DriverLib path is correct
   - Check target configuration matches your hardware

2. **Program Won't Flash**
   - Verify USB connection
   - Check target configuration file
   - Ensure board is powered

3. **UART Not Working**
   - Verify baud rate settings
   - Check pin configuration in Board.h
   - Ensure UART is initialized before use

4. **FreeRTOS Scheduler Won't Start**
   - Check stack sizes are sufficient
   - Verify tick interrupt is configured correctly
   - Review FreeRTOSConfig.h settings

## License

This project is provided as-is for educational and demonstration purposes.

## References

- [MSP430FR2155 Datasheet](https://www.ti.com/product/MSP430FR2155)
