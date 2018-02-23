//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 44, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHT3x Sample Code (V1.0)
// File      :  sht3x.h (V1.0)
// Author    :  RFU
// Date      :  16-Jun-2014
// Controller:  STM32F100RB
// IDE       :  �Vision V4.71.2.0
// Compiler  :  Armcc
// Brief     :  Sensor Layer: Definitions of commands and functions for sensor
//                            access.
//==============================================================================

#ifndef __SENSORS_SHT3X_H
#define __SENSORS_SHT3X_H

//-- Includes ------------------------------------------------------------------
#include "sys.h"
#include "sensors_i2c_hal.h"



//-- Enumerations --------------------------------------------------------------
// Sensor Commands
typedef enum{
  	CMD_READ_SERIALNBR  = 0x3780, // read serial number
  	CMD_READ_STATUS     = 0xF32D, // read status register
	CMD_CLEAR_STATUS    = 0x3041, // clear status register
	CMD_HEATER_ENABLE   = 0x306D, // enabled heater
	CMD_HEATER_DISABLE  = 0x3066, // disable heater
  	CMD_SOFT_RESET      = 0x30A2, // soft reset
	CMD_MEAS_CLOCKSTR_H = 0x2C06, // meas. clock stretching, high rep.
	CMD_MEAS_CLOCKSTR_M = 0x2C0D, // meas. clock stretching, medium rep.
	CMD_MEAS_CLOCKSTR_L = 0x2C10, // meas. clock stretching, low rep.
	CMD_MEAS_POLLING_H  = 0x2400, // meas. no clock stretching, high rep.
	CMD_MEAS_POLLING_M  = 0x240B, // meas. no clock stretching, medium rep.
	CMD_MEAS_POLLING_L  = 0x2416, // meas. no clock stretching, low rep.
	CMD_MEAS_PERI_05_H  = 0x2032, // meas. periodic 0.5 mps, high rep.
	CMD_MEAS_PERI_05_M  = 0x2024, // meas. periodic 0.5 mps, medium rep.
	CMD_MEAS_PERI_05_L  = 0x202F, // meas. periodic 0.5 mps, low rep.
	CMD_MEAS_PERI_1_H   = 0x2130, // meas. periodic 1 mps, high rep.
	CMD_MEAS_PERI_1_M   = 0x2126, // meas. periodic 1 mps, medium rep.
	CMD_MEAS_PERI_1_L   = 0x212D, // meas. periodic 1 mps, low rep.
	CMD_MEAS_PERI_2_H   = 0x2236, // meas. periodic 2 mps, high rep.
	CMD_MEAS_PERI_2_M   = 0x2220, // meas. periodic 2 mps, medium rep.
	CMD_MEAS_PERI_2_L   = 0x222B, // meas. periodic 2 mps, low rep.
	CMD_MEAS_PERI_4_H   = 0x2334, // meas. periodic 4 mps, high rep.
	CMD_MEAS_PERI_4_M   = 0x2322, // meas. periodic 4 mps, medium rep.
	CMD_MEAS_PERI_4_L   = 0x2329, // meas. periodic 4 mps, low rep.
	CMD_MEAS_PERI_10_H  = 0x2737, // meas. periodic 10 mps, high rep.
	CMD_MEAS_PERI_10_M  = 0x2721, // meas. periodic 10 mps, medium rep.
	CMD_MEAS_PERI_10_L  = 0x272A, // meas. periodic 10 mps, low rep.
	CMD_FETCH_DATA      = 0xE000, // readout measurements for periodic mode
	CMD_R_AL_LIM_LS     = 0xE102, // read alert limits, low set
	CMD_R_AL_LIM_LC     = 0xE109, // read alert limits, low clear
	CMD_R_AL_LIM_HS     = 0xE11F, // read alert limits, high set
	CMD_R_AL_LIM_HC     = 0xE114, // read alert limits, high clear
	CMD_W_AL_LIM_LS     = 0x6100, // write alert limits, low set
	CMD_W_AL_LIM_LC     = 0x610B, // write alert limits, low clear
	CMD_W_AL_LIM_HS     = 0x611D, // write alert limits, high set
	CMD_W_AL_LIM_HC     = 0x6116, // write alert limits, high clear
  CMD_NO_SLEEP        = 0x303E,
}etCommands;

typedef enum{
	REPEATAB_HIGH,   // high repeatability
	REPEATAB_MEDIUM, // medium repeatability
	REPEATAB_LOW,    // low repeatability
}etRepeatab;

typedef enum{
	MODE_CLKSTRETCH, // clock stretching
	MODE_POLLING,    // polling
}etMode;

typedef enum{
	FREQUENCY_HZ5,  //  0.5 measurements per seconds
	FREQUENCY_1HZ,  //  1.0 measurements per seconds
	FREQUENCY_2HZ,  //  2.0 measurements per seconds
	FREQUENCY_4HZ,  //  4.0 measurements per seconds
	FREQUENCY_10HZ, // 10.0 measurements per seconds
}etFrequency;

//-- Typedefs ------------------------------------------------------------------
// Status-Register
typedef union {
	u16t u16;
	struct{
		#ifdef LITTLE_ENDIAN  	// bit-order is little endian
		u16t CrcStatus     : 1; // write data checksum status
		u16t CmdStatus     : 1; // command status
		u16t Reserve0      : 2; // reserved
		u16t ResetDetected : 1; // system reset detected
		u16t Reserve1      : 5; // reserved
		u16t T_Alert       : 1; // temperature tracking alert
		u16t RH_Alert      : 1; // humidity tracking alert
		u16t Reserve2      : 1; // reserved
		u16t HeaterStatus  : 1; // heater status
		u16t Reserve3      : 1; // reserved
		u16t AlertPending  : 1; // alert pending status 
		#else                 	// bit-order is big endian
		u16t AlertPending  : 1;
		u16t Reserve3      : 1;
		u16t HeaterStatus  : 1;
		u16t Reserve2      : 1;
		u16t RH_Alert      : 1;
		u16t T_Alert       : 1;
		u16t Reserve1      : 5;
		u16t ResetDetected : 1;
		u16t Reserve0      : 2;
		u16t CmdStatus     : 1;
		u16t CrcStatus     : 1;
		#endif
	}bit;
} regStatus;


//==============================================================================
// Initializes the I2C bus for communication with the sensor.
//------------------------------------------------------------------------------
// input:  i2cAdr        I2C address, 0x44 ADDR pin low / 0x45 ADDR pin high
//------------------------------------------------------------------------------
void SHT3X_Init(u8t i2cAdr);


//==============================================================================
// Sets the I2C address.
//------------------------------------------------------------------------------
// input:  i2cAdr       I2C address, 0x44 ADDR pin low / 0x45 ADDR pin high
//------------------------------------------------------------------------------
void SHT3X_SetI2cAdr(u8t i2cAdr);


//==============================================================================
// Reads the serial number from sensor.
//------------------------------------------------------------------------------
// input:  *serialNbr   pointer to a variable to store the serialNumber
//
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3x_ReadSerialNumber(u32t *serialNbr);


//==============================================================================
// Reads the status register from the sensor.
//------------------------------------------------------------------------------
// input:  *status      pointer to a variable to store the status
//
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_ReadStatus(u16t *status);


//==============================================================================
// Clears all alert flags in status register from sensor.
//------------------------------------------------------------------------------
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_ClearAllAlertFlags(void);


//==============================================================================
// Gets the temperature [�C] and the relative humidity [%RH] from the sensor.
//------------------------------------------------------------------------------
// input:  *temp        pointer to a variable to store the temperature
//         *humi        pointer to a variable to store the humidity
//         repeatab     repeatability for the measurement [low, medium, high]
//         mode         command mode [clock stretching, polling]
//         timeout      timeout in milliseconds
//
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      PARM_ERROR     = parameter out of range
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_GetTempAndHumi(ft         *temp,
                             ft         *humi,
                             etRepeatab repeatab,
                             etMode     mode,
                             u8t        timeout);


//==============================================================================
// Gets the temperature [�C] and the relative humidity [%RH] from the sensor.
// This function uses the i2c clock stretching for waiting until measurement is
// ready.
//------------------------------------------------------------------------------
// input:  *temp        pointer to a variable to store the temperature
//         *humi        pointer to a variable to store the humidity
//         repeatab     repeatability for the measurement [low, medium, high]
//         timeout      clock stretching timeout in milliseconds
//
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      PARM_ERROR     = parameter out of range
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_GetTempAndHumiClkStretch(ft         *temp,
                                       ft         *humi,
                                       etRepeatab repeatab,
                                       u8t        timeout);


//==============================================================================
// Gets the temperature [�C] and the relative humidity [%RH] from the sensor.
// This function polls every 1ms until measurement is ready.
//------------------------------------------------------------------------------
// input:  *temp        pointer to a variable to store the temperature
//         *humi        pointer to a variable to store the humidity
//         repeatab     repeatability for the measurement [low, medium, high]
//         timeout      polling timeout in milliseconds
//
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      PARM_ERROR     = parameter out of range
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_GetTempAndHumiPolling(ft         *temp,
                                    ft         *humi,
                                    etRepeatab repeatab,
                                    u8t        timeout);


//==============================================================================
// Starts periodic measurement.
//------------------------------------------------------------------------------
// input:  repeatab     repeatability for the measurement [low, medium, high]
//         freq         measurement frequency [0.5, 1, 2, 4, 10] Hz
//
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      PARM_ERROR     = parameter out of range
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_StartPeriodicMeasurment(etRepeatab  repeatab, etFrequency freq);


//==============================================================================
// Reads last measurement from the sensor buffer
//------------------------------------------------------------------------------
// input:  *temp        pointer to a variable to store the temperature
//         *humi        pointer to a variable to store the humidity
//
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_ReadMeasurementBuffer(ft *temp, ft *humi);


//==============================================================================
// Enables the heater on sensor
//------------------------------------------------------------------------------
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_EnableHeater(void);


//==============================================================================
// Disables the heater on sensor
//------------------------------------------------------------------------------
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_DisbaleHeater(void);


//==============================================================================
// Calls the soft reset mechanism that forces the sensor into a well-defined
// state without removing the power supply.
//------------------------------------------------------------------------------
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_SoftReset(void);


//==============================================================================
// Resets the sensor by pulling down the reset pin.
//------------------------------------------------------------------------------
void SHT3X_HardReset(void);


//==============================================================================
// Writes a start condition and the sensor I2C address with the write flag.
//------------------------------------------------------------------------------
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_StartWriteAccess(void);


//==============================================================================
// Writes a start condition and the sensor I2C address with the read flag.
//------------------------------------------------------------------------------
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_StartReadAccess(void);

//==============================================================================
// Writes a stop condition.
//------------------------------------------------------------------------------
void SHT3X_StopAccess(void);


//==============================================================================
// Reads two bytes plus the checksum and verifies this. If the checksum
// verification is successful, then the two bytes stored in a 16-bit integer. 
//------------------------------------------------------------------------------
// input:  *data        pointer to a variable to store the data
//
//         finaleAck    set ACK to continue reading further bytes
//                      set NACK to stop reading
//
//         timeout      clock stretching timeout in milliseconds
//
// return: error:       CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_Read2BytesAndCrc(u16t *data, etI2cAck finaleAck, u8t timeout);


//==============================================================================
// Writes two bytes plus the checksum.
//------------------------------------------------------------------------------
// input:  data         write data: 2 bytes in a 16-bit variable
//
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      TIMEOUT_ERROR  = timeout
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_Write2BytesAndCrc(u16t data);

//==============================================================================
// Writes command to the sensor.
//------------------------------------------------------------------------------
// input:  cmd          sensor command
//
// return: error:       ACK_ERROR      = no acknowledgment from sensor
//                      TIMEOUT_ERROR  = timeout
//                      NO_ERROR       = no error
//------------------------------------------------------------------------------
u8t SHT3X_WriteCommand(etCommands cmd);


//==============================================================================
// Calculates the crc from a byte array
//------------------------------------------------------------------------------
// input:  data[]       checksum is built based on this data
//         nbrOfBytes   checksum is built for n bytes of data
//
// return:              crc, checksum
//------------------------------------------------------------------------------
u8t SHT3X_CalcCrc(u8t data[], u8t nbrOfBytes);


//==============================================================================
// Calculates checksum for n bytes of data and compares it with expected
// checksum. 
//------------------------------------------------------------------------------
// input:  data[]       checksum is built based on this data
//         nbrOfBytes   checksum is built for n bytes of data
//         checksum     expected checksum
//
// return: error:       CHECKSUM_ERROR = checksum does not match
//                      NO_ERROR       = checksum matches
//------------------------------------------------------------------------------
u8t SHT3X_CheckCrc(u8t data[], u8t nbrOfBytes, u8t checksum);


//==============================================================================
// Calculates the temperature [�C] as a floating point value from the raw data
// that are read from the sensor.
//------------------------------------------------------------------------------
// input:  rawValue     temperature raw value (16bit scaled)
//
// return:              temperature [�C] as a floating point value
//------------------------------------------------------------------------------
ft SHT3X_CalcTemperature(u16t rawValue);

//==============================================================================
// Calculates the relative humidity [%RH] as a floating point value from the raw
// data that are read from the sensor.
//------------------------------------------------------------------------------
// input:  rawValue     humidity raw value (16-bit scaled)
//
// return:              relative humidity [%RH] as a floating point value
//------------------------------------------------------------------------------
ft SHT3X_CalcHumidity(u16t rawValue);


//==============================================================================
// Calculates the raw data as 16-bit value from temperature [�C].
//------------------------------------------------------------------------------
// input:  temperature  temperature [�C] as a floating point value
//
// return:              temperature raw value (16-bit scaled)
//------------------------------------------------------------------------------
u16t SHT3X_CalcRawTemperature(ft temperature);


//==============================================================================
// Calculates the raw data as 16-bit value from relative humidity [%RH].
//------------------------------------------------------------------------------
// input:  humidity     humidity [%RH] as a floating point value
//
// return:              humidity raw value (16-bit scaled)
u16t SHT3X_CalcRawHumidity(ft humidity);


//==============================================================================
// Returns the state of the Alert-Pin.
//------------------------------------------------------------------------------
// return:              true:  Alert-Pin is high
//                      false: Alter-Pin is low
//------------------------------------------------------------------------------
bt SHT3X_ReadAlert(void);


#endif
