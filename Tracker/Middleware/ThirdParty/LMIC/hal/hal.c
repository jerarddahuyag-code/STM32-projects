#include "lmic.h"
#include "main.h"
#include "hal/hal.h"
#include <string.h>

// Global variables for HAL state
static uint8_t irqlevel = 0;
static lmic_hal_failure_handler_t* custom_failure_handler = NULL;

// Link to CubeMX generated handles
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim2;

// --- 1. System & Initialization ---

void lmic_hal_init (void) {
    // Start TIM2 (32-bit) at 1MHz
    HAL_TIM_Base_Start(&htim2);
}

void lmic_hal_init_ex (const void *pContext) {
    // Usually used for pinmap pointers, but since we hard-coded
    // pins in main.h for your 16 trackers, we just call init.
    lmic_hal_init();
}

void lmic_hal_failed (const char *file, u2_t line) {
    if (custom_failure_handler) {
        custom_failure_handler(file, line);
    }
    __disable_irq();
    while (1); // Halt
}

void lmic_hal_set_failure_handler (lmic_hal_failure_handler_t* handler) {
    custom_failure_handler = handler;
}

// --- 2. Radio Control Pins ---

void lmic_hal_pin_rxtx (u1_t val) {
    // Not used for RFM95W unless you have an external antenna switch chip
}

void lmic_hal_pin_rst (u1_t val) {
    if (val == 0 || val == 1) { // Drive pin
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = LORA_RST_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(LORA_RST_GPIO_Port, &GPIO_InitStruct);
        HAL_GPIO_WritePin(LORA_RST_GPIO_Port, LORA_RST_Pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
    } else { // Float pin
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = LORA_RST_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(LORA_RST_GPIO_Port, &GPIO_InitStruct);
    }
}

// --- 3. SPI Transactions ---

// Internal helper for Chip Select
static void nss_select(u1_t on) {
    HAL_GPIO_WritePin(LORA_NSS_GPIO_Port, LORA_NSS_Pin, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void lmic_hal_spi_write(u1_t cmd, const u1_t* buf, size_t len) {
    nss_select(1);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
    if (len > 0) {
        HAL_SPI_Transmit(&hspi1, (u1_t*)buf, len, 10);
    }
    nss_select(0);
}

void lmic_hal_spi_read(u1_t cmd, u1_t* buf, size_t len) {
    nss_select(1);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
    if (len > 0) {
        memset(buf, 0, len);
        HAL_SPI_Receive(&hspi1, buf, len, 10);
    }
    nss_select(0);
}

bit_t lmic_hal_radio_spi_is_busy() {
    return (hspi1.State == HAL_SPI_STATE_BUSY);
}

// --- 4. Timing & Waiting ---

u4_t lmic_hal_ticks (void) {
	/*uint32_t raw_us = (u4_t)__HAL_TIM_GET_COUNTER(&htim2);

	uint32_t scaled_ticks = (uint32_t)( ((uint64_t)raw_us * OSTICKS_PER_SEC) / 1000000 );
	return scaled_ticks;*/
	// This static variable persists between calls to track overflows
	static uint8_t overflow = 0;

	// 1. Get the raw microsecond count from your 1MHz TIM2
	// This replaces the Arduino micros() function
	uint32_t current_micros = __HAL_TIM_GET_COUNTER(&htim2);

	// 2. Scale down the timestamp by the exponent (e.g., divide by 16)
	uint32_t scaled = current_micros >> US_PER_OSTICK_EXPONENT;

	// 3. Extract the Most Significant Byte of the scaled value
	uint8_t msb = (uint8_t)(scaled >> 24);

	// 4. Create the mask for the 'overlapping bit' to detect the wrap-around
	const uint8_t mask = (1 << (7 - US_PER_OSTICK_EXPONENT));

	// 5. Update the overflow byte.
	// This clever bitwise math detects when the 32-bit counter resets to 0
	// and increments the "hidden" upper bits stored in 'overflow'.
	overflow += (msb ^ overflow) & mask;

	// 6. Combine the 32-bit scaled value with the overflow extension
	// This effectively creates a ~40-bit timer represented in a 32-bit u4_t
	return scaled | ((uint32_t)overflow << 24);
}

u4_t lmic_hal_waitUntil (u4_t time) {
    s4_t delta = (s4_t)(time - lmic_hal_ticks());
    if (delta < 0) return (u4_t)-delta; // We are late
    while ((s4_t)(time - lmic_hal_ticks()) > 0);
    return 0; // On time

    // Calculate the difference between the target time and current time.
	// Casting to a signed integer (s4_t) handles 32-bit wrap-around natively.
	/*s4_t delta = (s4_t)(time - os_getTime());

	// If delta is negative, the target time has already passed.
	// Return how late we are so the library can log it or adjust.
	if (delta < 0) {
		return (u4_t)(-delta);
	}

	// Spin in place until our hardware-backed timer catches up.
	// This is 100% safe to do even with interrupts disabled because
	// os_getTime() reads directly from the TIM2 hardware register.
	while ((s4_t)(time - os_getTime()) > 0) {
		// Just wait.
		// Optional: You can insert a __NOP(); instruction here to
		// prevent the compiler from optimizing the loop away, though
		// os_getTime() is usually volatile enough.
		__NOP();
	}

	// We reached the exact tick without being late.
	return 0;*/
}

u1_t lmic_hal_checkTimer (u4_t targettime) {
    // Return 1 if target time is reached or very close (< 5 ticks)
    return (s4_t)(targettime - lmic_hal_ticks()) < 5;
}

// --- 5. Interrupts & Polling ---

void lmic_hal_disableIRQs (void) {
    __disable_irq();
    irqlevel++;
}

void lmic_hal_enableIRQs (void) {
    if (--irqlevel == 0) {
        __enable_irq();
    }
}

uint8_t lmic_hal_getIrqLevel (void) {
    return irqlevel;
}

// Polling DIO pins (Since we aren't using hardware EXTI interrupts)
void lmic_hal_pollPendingIRQs_helper() {
    /*if (HAL_GPIO_ReadPin(LORA_DIO0_GPIO_Port, LORA_DIO0_Pin) == GPIO_PIN_SET ||
        HAL_GPIO_ReadPin(LORA_DIO1_GPIO_Port, LORA_DIO1_Pin) == GPIO_PIN_SET) {
        // This is where LMIC's internal radio_irq_handler would be called by the OS
        // But in polling mode, LMIC calls hal_processPendingIRQs
    }*/
}

void lmic_hal_processPendingIRQs(void) {
    if (HAL_GPIO_ReadPin(LORA_DIO0_GPIO_Port, LORA_DIO0_Pin) == GPIO_PIN_SET) {
        radio_irq_handler_v2(0, os_getTime());
    }
    if (HAL_GPIO_ReadPin(LORA_DIO1_GPIO_Port, LORA_DIO1_Pin) == GPIO_PIN_SET) {
        radio_irq_handler_v2(1, os_getTime());
    }
}

// --- 6. Stubs & Queries (Required to link) ---

void lmic_hal_sleep (void) { /* For future battery optimization */ }

s1_t lmic_hal_getRssiCal (void) { return 0; }

ostime_t lmic_hal_setModuleActive (bit_t val) { return 0; }

bit_t lmic_hal_queryUsingTcxo(void) { return 0; }
bit_t lmic_hal_queryUsingDcdc(void) { return 0; }
bit_t lmic_hal_queryUsingDIO2AsRfSwitch(void) { return 0; }
bit_t lmic_hal_queryUsingDIO3AsTCXOSwitch(void) { return 0; }

uint8_t lmic_hal_getTxPowerPolicy(u1_t inputPolicy, s1_t requestedPower, u4_t freq) {
    return 0; // Use default
}
