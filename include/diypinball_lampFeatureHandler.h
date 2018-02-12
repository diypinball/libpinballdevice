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
 * \struct diypinball_lampStatus_t diypinball_lampStatus
 * \brief Stores information related to a lamp
 */
typedef struct diypinball_lampStatus {
    uint8_t state1;
    uint8_t state1Duration;
    uint8_t state2;
    uint8_t state2Duration;
    uint8_t state3;
    uint8_t state3Duration;
    uint8_t numStates;
} diypinball_lampStatus_t;

/*
 * \brief Function pointer to a read switch state handler, whose implementation is platform-specific
 */
typedef void (*diypinball_lampFeatureHandlerLampChangedHandler)(uint8_t lampNum, diypinball_lampStatus_t lampStatus);

/*
 * \struct diypinball_lampFeatureHandlerInstance_t diypinball_lampFeatureHandlerInstance
 * \brief Stores information relating to the instance of a LampFeatureHandler feature
 */
typedef struct diypinball_lampFeatureHandlerInstance {
    diypinball_featureHandlerInstance_t featureHandlerInstance;             /**< featureDecoder instance for the FeatureRouter */
    diypinball_lampStatus_t lamps[16];
    uint8_t numLamps;
    diypinball_lampFeatureHandlerLampChangedHandler lampChangedHandler;
} diypinball_lampFeatureHandlerInstance_t;

/*
 * \struct diypinball_diypinball_lampFeatureHandlerInit_t diypinball_lampFeatureHandlerInit
 * \brief Stores initialization information to set up a LampFeatureHandler instance
 */
typedef struct diypinball_lampFeatureHandlerInit {
    uint8_t numLamps;
    diypinball_lampFeatureHandlerLampChangedHandler lampChangedHandler;
    diypinball_featureRouterInstance_t *routerInstance;                       /**< FeatureRouter instance to connect to */
} diypinball_lampFeatureHandlerInit_t;

/**
 * \brief Initialize the LampFeatureHandler feature from an initialization struct
 *
 * \param[in] instance                  LampFeatureHandler instance struct
 * \param[in] init                      LampFeatureHandler initialization struct
 *
 * \return Nothing
 */
void diypinball_lampFeatureHandler_init(diypinball_lampFeatureHandlerInstance_t *instance, diypinball_lampFeatureHandlerInit_t *init);

/**
 * \brief Handle a millisecondTick event for a LampFeatureHandler instance
 *
 * \param[in] instance                  LampFeatureHandler instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_lampFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Process a received Pinball message meant for a LampFeatureHandler instance
 *
 * \param[in] instance                  LampFeatureHandler instance struct
 * \param[in] message                   Pinball message struct
 *
 * \return Nothing
 */
void diypinball_lampFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);

/**
 * \brief Deinitialize the LampFeatureHandler feature
 *
 * \param[in] instance                  LampFeatureHandler instance struct
 *
 * \return Nothing
 */
void diypinball_lampFeatureHandler_deinit(diypinball_lampFeatureHandlerInstance_t *instance);

#ifdef __cplusplus
}
#endif
