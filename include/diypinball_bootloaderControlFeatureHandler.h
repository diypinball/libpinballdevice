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
 * \brief Function pointer to a boot-to-bootloader request handler, whose implementation is platform-specific
 */
typedef void (*diypinball_bootloaderControlFeatureHandlerRebootHandler)(void);

/*
 * \struct diypinball_systemManagementFeatureHandlerInstance_t diypinball_systemManagementFeatureHandlerInstance
 * \brief Stores information relating to the instance of a SystemManagementFeatureHandler feature
 */
typedef struct diypinball_bootloaderControlFeatureHandlerInstance {
    diypinball_featureHandlerInstance_t featureHandlerInstance;         /**< featureDecoder instance for the FeatureRouter */
    uint8_t bootloaderVersionMajor;                                     /**< Major version number */
    uint8_t bootloaderVersionMinor;                                     /**< Minor version number */
    uint8_t bootloaderVersionPatch;                                     /**< Patch version number */
    diypinball_bootloaderControlFeatureHandlerRebootHandler rebootHandler;   /**< Function pointer to power status request handler */
    uint32_t lastTick;                                                  /**< Last tick number received by millisecond tick handler */
} diypinball_bootloaderControlFeatureHandlerInstance_t;

/*
 * \struct diypinball_bootloaderControlFeatureHandlerInit_t diypinball_bootloaderControlFeatureHandlerInit
 * \brief Stores initialization information to set up a BootloaderControlFeatureHandler instance
 */
typedef struct diypinball_bootloaderControlFeatureHandlerInit {
    uint8_t bootloaderVersionMajor;                                     /**< Major version number */
    uint8_t bootloaderVersionMinor;                                     /**< Minor version number */
    uint8_t bootloaderVersionPatch;                                     /**< Patch version number */
    diypinball_bootloaderControlFeatureHandlerRebootHandler rebootHandler;   /**< Function pointer to power status request handler */
    diypinball_featureRouterInstance_t *routerInstance;                 /**< FeatureRouter instance to connect to */
} diypinball_bootloaderControlFeatureHandlerInit_t;

/**
 * \brief Initialize the BootloaderControlFeatureHandler feature from an initialization struct
 *
 * \param[in] instance                  BootloaderControlFeatureHandler instance struct
 * \param[in] init                      BootloaderControlFeatureHandler initialization struct
 *
 * \return Nothing
 */
void diypinball_bootloaderControlFeatureHandler_init(diypinball_bootloaderControlFeatureHandlerInstance_t *instance, diypinball_bootloaderControlFeatureHandlerInit_t *init);

/**
 * \brief Handle a millisecondTick event for a BootloaderControlFeatureHandler instance
 *
 * \param[in] instance                  BootloaderControlFeatureHandler instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_bootloaderControlFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Process a received Pinball message meant for a BootloaderControlFeatureHandler instance
 *
 * \param[in] instance                  BootloaderControlFeatureHandler instance struct
 * \param[in] message                   Pinball message struct
 *
 * \return Nothing
 */
void diypinball_bootloaderControlFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);

/**
 * \brief Deinitialize the BootloaderControlFeatureHandler feature
 *
 * \param[in] instance                  BootloaderControlFeatureHandler instance struct
 *
 * \return Nothing
 */
void diypinball_bootloaderControlFeatureHandler_deinit(diypinball_bootloaderControlFeatureHandlerInstance_t *instance);

#ifdef __cplusplus
}
#endif
