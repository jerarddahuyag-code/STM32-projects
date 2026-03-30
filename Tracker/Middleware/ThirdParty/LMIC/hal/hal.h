/*******************************************************************************
 * Copyright (c) 2015-2016 Matthijs Kooijman
 * Copyright (c) 2016-2024 MCCI Corporation
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * This the HAL to run LMIC on top of the Arduino environment.
 *******************************************************************************/
#ifndef _hal_hal_h_
#define _hal_hal_h_

#include <stdint.h>
#include <stdbool.h>

// 1. Define the basic types LMIC expects since we don't have Arduino.h
typedef uint8_t  u1_t;
typedef int8_t   s1_t;
typedef uint16_t u2_t;
typedef int16_t  s2_t;
typedef uint32_t u4_t;
typedef int32_t  s4_t;

// 2. Define a simple C structure for the pins.
// This replaces the complex C++ 'HalPinmap_t'.
typedef struct {
    uint32_t nss;    // Chip Select pin
    uint32_t rxtx;   // Antenna switch (usually unused)
    uint32_t rst;    // Reset pin
    uint32_t dio[3]; // DIO0, DIO1, DIO2 pins
} lmic_pinmap;

// 3. Constants for unused pins
#define LMIC_UNUSED_PIN 0xffffffff

// 4. Function declarations that your lmic_hal.c MUST implement
void hal_init (void);
void hal_pin_nss (u1_t val);
void hal_pin_rst (u1_t val);
u1_t hal_spi (u1_t out);
void hal_waitUntil (u4_t time);
u4_t hal_ticks (void);
bool hal_checkIrq (void);
void hal_sleep (void);

// 5. External reference so LMIC can see your pin configuration in main.c
extern const lmic_pinmap lmic_pins;

#endif // _hal_hal_h_
