#ifndef MC24LC32_H
#define MC24LC32_H

// MC24LC32 Device Driver -----------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.29
//
// Description: Driver for the Microchip 24LC32 I2C EEPROM.

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief Memory size of the MC24LC32 EEPROM in bytes.
#define MC24LC32_SIZE 4096

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef enum
{
	MC24LC32_STATE_FAILED	= 0,
	MC24LC32_STATE_INVALID	= 1,
	MC24LC32_STATE_READY	= 3
} mc24lc32State_t;

typedef struct
{
	/// @brief The 7-bit I2C address of the device.
	uint8_t addr;

	/// @brief The IC2 bus of the device.
	I2CDriver* i2c;

	/// @brief The timeout interval for the device's acknowledgement polling. If the device does not send an acknowledgement
	/// within this timeframe, it will be considered invalid.
	sysinterval_t timeoutPeriod;

	/// @brief The magic string used to validate the EEPROM's contents.
	const char* magicString;
} mc24lc32Config_t;

/// @brief Driver for the Microchip 24LC32 I2C EEPROM.
typedef struct
{
	/// @brief State of the device.
	mc24lc32State_t state;

	/// @brief The 7-bit I2C address of the device.
	uint8_t addr;

	/// @brief The IC2 bus of the device.
	I2CDriver* i2c;

	/// @brief The magic string used to validate the EEPROM's contents.
	const char* magicString;

	/// @brief Cached copy of the EEPROM's contents. Use for read / write operations.
	uint8_t cache [MC24LC32_SIZE];

	/// @brief The timeout interval for the device's acknowledgement polling. If the device does not send an acknowledgement
	/// within this timeframe, it will be considered invalid.
	sysinterval_t timeoutPeriod;
} mc24lc32_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the device using the specified configuration.
 * @param mc24lc32 The device to initialize.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool mc24lc32Init (mc24lc32_t* mc24lc32, mc24lc32Config_t* config);

/**
 * @brief Reads the contents of the devices memory into local cache.
 * @param mc24lc32 The device to read from.
 * @return True if successful and the memory is valid, false otherwise.
 */
bool mc24lc32Read (mc24lc32_t* mc24lc32);

/**
 * @brief Writes the local cached memory to the device.
 * @param mc24lc32 The device to write to.
 * @return True if successful, false otherwise.
 */
bool mc24lc32Write (mc24lc32_t* mc24lc32);

/**
 * @brief Writes the specified data to the device.
 * @param mc24lc32 The device to write to.
 * @param address The address to write to.
 * @param data The array of data to write.
 * @param dataCount The size of the data array.
 * @return True if successful, false otherwise.
 * @note The write operation cannot cross a page boundary (32 byte).
 */
bool mc24lc32WriteThrough (mc24lc32_t* mc24lc32, uint16_t address, uint8_t* data, uint8_t dataCount);

/**
 * @brief Checks whether the cached memory of the device is valid.
 * @param mc24lc32 The device to check.
 * @return True if the cached memory is valid, false otherwise.
 */
bool mc24lc32IsValid (mc24lc32_t* mc24lc32);

/**
 * @brief Validates the cached memory of the device. Changes will be committed on the next write.
 * @param mc24lc32 The device to validate.
 */
void mc24lc32Validate (mc24lc32_t* mc24lc32);

/**
 * @brief Invalidates the cached memory of the device. Changes will be committed on the next write.
 * @param mc24lc32 The device to invalidate.
 */
void mc34lc32Invalidate (mc24lc32_t* mc24lc32);

#endif // MC24LC32_H