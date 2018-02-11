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
#include "diypinball_featureRouter.h"

#include <stdint.h>

/*
 * \brief Function pointer to a power status request handler, whose implementation is platform-specific
 */
typedef void (*diypinball_systemManagementFeatureHandlerPowerStatusHandler)(uint8_t *voltages, uint8_t *currents);

/*
 * \struct diypinball_systemManagementFeatureHandlerInstance_t diypinball_systemManagementFeatureHandlerInstance
 * \brief Stores information relating to the instance of a SystemManagementFeatureHandler feature
 */
typedef struct diypinball_systemManagementFeatureHandlerInstance {
    diypinball_featureHandlerInstance_t featureHandlerInstance;         /**< featureDecoder instance for the FeatureRouter */
    uint8_t firmwareVersionMajor;                                       /**< Major version number */
    uint8_t firmwareVersionMinor;                                       /**< Minor version number */
    uint8_t firmwareVersionPatch;                                       /**< Patch version number */
    uint8_t powerStatusPollingInterval;                                 /**< Interval in milliseconds between automatic power status messages */
    uint32_t boardSerial[4];                                            /**< Board serial number, 128-bit */
    uint32_t boardSignature[2];                                         /**< Board hardware signature, 64-bit */
    diypinball_systemManagementFeatureHandlerPowerStatusHandler powerStatusHandler;   /**< Function pointer to power status request handler */
    uint32_t lastTick;                                                  /**< Last tick number received by millisecond tick handler */
} diypinball_systemManagementFeatureHandlerInstance_t;

/*
 * \struct diypinball_systemManagementFeatureHandlerInit_t diypinball_systemManagementFeatureHandlerInit
 * \brief Stores initialization information to set up a SystemManagementFeatureHandler instance
 */
typedef struct diypinball_systemManagementFeatureHandlerInit {
    uint8_t firmwareVersionMajor;                                       /**< Major version number */
    uint8_t firmwareVersionMinor;                                       /**< Minor version number */
    uint8_t firmwareVersionPatch;                                       /**< Patch version number */
    uint32_t boardSerial[4];                                            /**< Board serial number, 128-bit */
    uint32_t boardSignature[2];                                         /**< Board hardware signature, 64-bit */
    diypinball_systemManagementFeatureHandlerPowerStatusHandler powerStatusHandler;   /**< Function pointer to power status request handler */
    diypinball_featureRouterInstance_t *routerInstance;                 /**< FeatureRouter instance to connect to */
} diypinball_systemManagementFeatureHandlerInit_t;

/**
 * \brief Initialize the SystemManagementFeatureHandler feature from an initialization struct
 *
 * \param[in] instance                  SystemManagementFeatureHandler instance struct
 * \param[in] init                      SystemManagementFeatureHandler initialization struct
 *
 * \return Nothing
 */
void diypinball_systemManagementFeatureHandler_init(diypinball_systemManagementFeatureHandlerInstance_t *instance, diypinball_systemManagementFeatureHandlerInit_t *init);

/**
 * \brief Handle a millisecondTick event for a SystemManagementFeatureHandler instance
 *
 * \param[in] instance                  SystemManagementFeatureHandler instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_systemManagementFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Process a received Pinball message meant for a SystemManagementFeatureHandler instance
 *
 * \param[in] instance                  SystemManagementFeatureHandler instance struct
 * \param[in] message                   Pinball message struct
 *
 * \return Nothing
 */
void diypinball_systemManagementFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);

/**
 * \brief Send the two Board ID messages on system powerup
 *
 * \param[in] instance                  SystemManagementFeatureHandler instance struct
 *
 * \return Nothing
 */
void diypinball_systemManagementFeatureHandler_sendPoweronMessages(diypinball_systemManagementFeatureHandlerInstance_t *instance);

/**
 * \brief Deinitialize the SystemManagementFeatureHandler feature
 *
 * \param[in] instance                  SystemManagementFeatureHandler instance struct
 *
 * \return Nothing
 */
void diypinball_systemManagementFeatureHandler_deinit(diypinball_systemManagementFeatureHandlerInstance_t *instance);

#ifdef __cplusplus
}
#endif
