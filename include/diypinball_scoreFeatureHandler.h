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
 * \brief Function pointer to a score changed handler, whose implementation is platform-specific
 */
typedef void (*diypinball_scoreFeatureHandlerDisplayChangedHandler)(char* display);

/*
 * \brief Function pointer to a brightness changed handler, whose implementation is platform-specific
 */
typedef void (*diypinball_scoreFeatureHandlerBrightnessChangedHandler)(uint8_t brightness);

/*
 * \struct diypinball_scoreFeatureHandlerInstance_t diypinball_scoreFeatureHandlerInstance
 * \brief Stores information relating to the instance of a ScoreFeatureHandler feature
 */
typedef struct diypinball_scoreFeatureHandlerInstance {
    diypinball_featureHandlerInstance_t featureHandlerInstance;             /**< featureDecoder instance for the FeatureRouter */
    char display[8];
    uint8_t brightness;
    diypinball_scoreFeatureHandlerDisplayChangedHandler displayChangedHandler;
    diypinball_scoreFeatureHandlerBrightnessChangedHandler brightnessChangedHandler;
} diypinball_scoreFeatureHandlerInstance_t;

/*
 * \struct diypinball_diypinball_scoreFeatureHandlerInit_t diypinball_scoreFeatureHandlerInit
 * \brief Stores initialization information to set up a ScoreFeatureHandler instance
 */
typedef struct diypinball_scoreFeatureHandlerInit {
    diypinball_scoreFeatureHandlerDisplayChangedHandler displayChangedHandler;
    diypinball_scoreFeatureHandlerBrightnessChangedHandler brightnessChangedHandler;
    diypinball_featureRouterInstance_t *routerInstance;                       /**< FeatureRouter instance to connect to */
} diypinball_scoreFeatureHandlerInit_t;

/**
 * \brief Initialize the ScoreFeatureHandler feature from an initialization struct
 *
 * \param[in] instance                  ScoreFeatureHandler instance struct
 * \param[in] init                      ScoreFeatureHandler initialization struct
 *
 * \return Nothing
 */
void diypinball_scoreFeatureHandler_init(diypinball_scoreFeatureHandlerInstance_t *instance, diypinball_scoreFeatureHandlerInit_t *init);

/**
 * \brief Handle a millisecondTick event for a ScoreFeatureHandler instance
 *
 * \param[in] instance                  ScoreFeatureHandler instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_scoreFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Process a received Pinball message meant for a ScoreFeatureHandler instance
 *
 * \param[in] instance                  ScoreFeatureHandler instance struct
 * \param[in] message                   Pinball message struct
 *
 * \return Nothing
 */
void diypinball_scoreFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);

/**
 * \brief Deinitialize the ScoreFeatureHandler feature
 *
 * \param[in] instance                  ScoreFeatureHandler instance struct
 *
 * \return Nothing
 */
void diypinball_scoreFeatureHandler_deinit(diypinball_scoreFeatureHandlerInstance_t *instance);

#ifdef __cplusplus
}
#endif
