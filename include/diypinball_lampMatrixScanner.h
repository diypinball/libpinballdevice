/*
libpinballdevice
Copyright (C) 2018 Randy Glenn <randy.glenn@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "diypinball.h"
#include "diypinball_lampFeatureHandler.h"

#include <stdint.h>

/*
 * \brief Interrupt type
 */
typedef enum diypinball_lampMatrixScanner_interruptType {
    LAMP_INTERRUPT_RESET,                               /**< Timer rollover */
    LAMP_INTERRUPT_MATCH,                               /**< Timer match */
} diypinball_lampMatrixScanner_interruptType_t;

/*
 * \brief Function pointer to a set column handler, whose implementation is platform-specific. -1 deasserts all columns.
 */
typedef void (*diypinball_lampMatrixScannerSetColumnHandler)(int8_t colNum);

/*
 * \brief Function pointer to a set row handler, whose implementation is platform-specific
 */
typedef void (*diypinball_lampMatrixScannerSetRowHandler)(uint8_t row0val, uint8_t row1val, uint8_t row2val, uint8_t row3val);

/*
 * \struct diypinball_lampMatrixState_t diypinball_lampMatrixState
 * \brief Stores information related to an individual lamp in the matrix
 */
typedef struct diypinball_lampMatrixState {
    diypinball_lampStatus_t lampState;                                      /**< The lamp's state */
    uint32_t lastTick;                                                      /**< Last timer tick where a change occured*/
    uint8_t currentPhase;                                                   /**< Which phase of the lamp's state we're in */
} diypinball_lampMatrixState_t;

/*
 * \struct diypinball_lampMatrixScannerInstance_t diypinball_lampMatrixScannerInstance
 * \brief Stores information relating to the instance of a LampMatrixScanner
 */
typedef struct diypinball_lampMatrixScannerInstance {
    diypinball_lampMatrixState_t lamps[16];                                 /**< Array of lamp state objects */
    uint8_t numColumns;                                                     /**< The number of columns to be scanned */
    uint8_t currentColumn;                                                  /**< The current column being scanned */
    uint32_t lastTick;                                                      /**< Most recent tick number */
    diypinball_lampMatrixScannerSetColumnHandler setColumnHandler;          /**< Function pointer to the set column handler */
    diypinball_lampMatrixScannerSetRowHandler setRowHandler;                /**< Function pointer to the set row handler */
} diypinball_lampMatrixScannerInstance_t;

/*
 * \struct diypinball_lampMatrixScannerInit_t diypinball_lampMatrixScannerInit
 * \brief Stores initialization information to set up a LampMatrixScanner instance
 */
typedef struct diypinball_lampMatrixScannerInit {
    uint8_t numColumns;                                                     /**< The number of columns to be scanned */
    diypinball_lampMatrixScannerSetColumnHandler setColumnHandler;          /**< Function pointer to the set column handler */
    diypinball_lampMatrixScannerSetRowHandler setRowHandler;                /**< Function pointer to the set row handler */
} diypinball_lampMatrixScannerInit_t;

/**
 * \brief Initialize the LampMatrixScanner feature from an initialization struct
 *
 * \param[in] instance                  LampMatrixScanner instance struct
 * \param[in] init                      LampMatrixScanner initialization struct
 *
 * \return Nothing
 */
void diypinball_lampMatrixScanner_init(diypinball_lampMatrixScannerInstance_t *instance, diypinball_lampMatrixScannerInit_t *init);

/**
 * \brief Handle a millisecondTick event for a LampMatrixScanner instance
 *
 * \param[in] instance                  LampMatrixScanner instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_lampMatrixScanner_millisecondTickHandler(diypinball_lampMatrixScannerInstance_t *instance, uint32_t tickNum);

/**
 * \brief Deinitialize the LampMatrixScanner feature
 *
 * \param[in] instance                  LampMatrixScanner instance struct
 *
 * \return Nothing
 */
void diypinball_lampMatrixScanner_deinit(diypinball_lampMatrixScannerInstance_t *instance);

/**
 * \brief Set a lamp state in the LampMatrixScanner
 *
 * \param[in] instance                  LampMatrixScanner instance struct
 * \param[in] lampNum                   Which lamp is being changed
 * \param[in] state                     The new state for the lamp
 *
 * \return Nothing
 */
void diypinball_lampMatrixScanner_setLampState(diypinball_lampMatrixScannerInstance_t *instance, uint8_t lampNum, diypinball_lampStatus_t *state);

/**
 * \brief Pass an interrupt to the LampMatrixScanner
 *
 * \param[in] instance                  LampMatrixScanner instance struct
 * \param[in] interruptType             Which interrupt is being passed
 *
 * \return Nothing
 */
void diypinball_lampMatrixScanner_isr(diypinball_lampMatrixScannerInstance_t *instance, diypinball_lampMatrixScanner_interruptType_t interruptType);

#ifdef __cplusplus
}
#endif
