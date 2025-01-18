#ifndef AMK_INVERTER_H
#define AMK_INVERTER_H

// AMK Inverter ---------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.12
//
// Description: Object representing the AMK Racing Kit Inverter CAN node.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	CANDriver*		driver;
	sysinterval_t	timeoutPeriod;
	uint16_t		baseId;
} amkInverterConfig_t;

/**
 * @brief Represents the generalized state of an AMK inverter. Note these are ordered by their relative priority, that is, the
 * importance of an inverter in this state.
 */
typedef enum
{
	/// @brief Indicates the data in this structure is not valid, see @c canState for more info.
	AMK_STATE_INVALID = 0,

	/// @brief Indicates the inverter is reporting an error.
	AMK_STATE_ERROR = 1,

	/// @brief Indicates the inverter is ready, error-free, but high-voltage is not present.
	AMK_STATE_READY_LOW_VOLTAGE = 5,

	/// @brief Indicates the inverter is ready, error-free, and the DC bus is charged.
	AMK_STATE_READY_HIGH_VOLTAGE = 6,

	/// @brief Indicates the inverter is ready, error-free, and the motor is energized.
	AMK_STATE_READY_ENERGIZED = 7
} amkInverterState_t;

typedef struct
{
	CAN_NODE_FIELDS;
	uint16_t baseId;

	/// @brief Indicates whether the inverter is ready and error-free.
	bool systemReady;

	/// @brief Indicates whether a system error is present.
	bool error;

	/// @brief Indicates whether a system warning is present.
	bool warning;

	/// @brief Acknowledgement of the DC bus being enabled and energized. Is not asserted until the @c dcOn bit is set and
	/// the DC bus voltage exceeds the minimum.
	bool quitDcOn;

	/// @brief Indicates whether the DC bus is enabled or not. See @c quitDcOn for whether the bus is energized.
	bool dcOn;

	/// @brief Acknowledgement of the inverter being enabled and energized. Is not asserted until the DC bus is energized, the
	/// @c inverterOn bit is set, and the system is error-free.
	bool quitInverter;

	/// @brief Indicates whether the inverter controller is enabled or not. See @c quitInverter for whether the system is
	/// energized or not.
	bool inverterOn;

	/// @brief Indicates whether the output torque is being de-rated due to hardware conditions.
	bool derating;

	/// @brief The actual torque being produced / regenerated at the motor shaft (may not match requested torque due to
	/// de-rating).
	float actualTorque;

	/// @brief The actual speed of the motor shaft.
	float actualSpeed;

	/// @brief The measured voltage of the DC bus.
	float dcBusVoltage;

	/// @brief The measured torque current (Id) of the motor.
	float torqueCurrent;

	/// @brief The actual power consumption of the device.
	float actualPower;
} amkInverter_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void amkInit (amkInverter_t* amk, amkInverterConfig_t* config);

/**
 * @brief Gets the current state of the inverter, as an enum. Note the CAN node should be locked beforehand.
 * @param amk The inverter to check the state of.
 * @return The state of the inverter.
 */
amkInverterState_t amkGetState (amkInverter_t* amk);

// Array Functions ------------------------------------------------------------------------------------------------------------

/**
 * @brief Gets the global state of a group of inverters.
 * @param amks The array of inverters to check the state of.
 * @param count The number of elements in @c amks .
 * @return The global state of the group of inverters.
 */
amkInverterState_t amksGetState (amkInverter_t* amks, uint32_t count);

/**
 * @brief Gets the global power consumption of a group of inverters.
 * @param amks The array of inverters to get the power of.
 * @param count The number of elements in @c amks .
 * @return The total amount of power being consumed, in Watts.
 */
float amksGetCumulativePower (amkInverter_t* amks, uint32_t count);

// Transmit Functions ---------------------------------------------------------------------------------------------------------

/**
 * @brief Sends a request to energize / de-energize the inverter.
 * @param amk The inverter to request.
 * @param energized True if the inverter should be energized, false if de-energized.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t amkSendEnergizationRequest (amkInverter_t* amk, bool energized, sysinterval_t timeout);

/**
 * @brief Sends a request for a specific amount of torque to be generated. Will request to energize the inverter if it is not
 * already.
 * @param amk The inverter to request.
 * @param torqueRequest The amount of torque to request, in Nm.
 * @param torqueLimitPositive The upper torque limit to apply, in Nm.
 * @param torqueLimitNegative The lower torque limit to apply, in Nm.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t amkSendTorqueRequest (amkInverter_t* amk, float torqueRequest, float torqueLimitPositive, float torqueLimitNegative,
	sysinterval_t timeout);

/**
 * @brief Sends a request to clear all system errors, if any are present.
 * @param amk The inverter to request.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t amkSendErrorResetRequest (amkInverter_t* amk, sysinterval_t timeout);

#endif // AMK_INVERTER_H