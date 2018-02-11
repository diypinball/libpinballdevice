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

#include <stdint.h>

typedef struct diypinball_featureRouterInstance diypinball_featureRouterInstance_t;
typedef struct diypinball_featureRouterInit diypinball_featureRouterInit_t;
typedef struct diypinball_featureHandlerInstance diypinball_featureHandlerInstance_t;

/*
 * \brief Function pointer to a message received handler, implemented by a FeatureHandler
 */
typedef void (*diypinball_messageReceivedHandler)(void *featureHandlerInstance, diypinball_pinballMessage_t *message);

/*
 * \brief Function pointer to a millisecond tick handler, implemented by a FeatureHandler
 */
typedef void (*diypinball_millisecondTickHandler)(void *featureHandlerInstance, uint32_t tickNum);

/*
 * \struct diypinball_featureRouterInstance
 * \brief Stores information relating to the instance of a FeatureRouter
 */
struct diypinball_featureRouterInstance {
    uint8_t boardAddress;                               /**< The board address for this FeatureRouter */
    diypinball_featureHandlerInstance_t* features[16];  /**< Array of pointers to the implemented FeatureHandlers */
    diypinball_canMessageSendHandler canSendHandler;    /**< Pointer to the function to send a CAN message */
};

/*
 * \struct diypinball_featureRouterInit
 * \brief Stores initialization information to set up a FeatureRouter instance
 */
struct diypinball_featureRouterInit {
    uint8_t boardAddress;                               /**< The board address for this FeatureRouter */
    diypinball_canMessageSendHandler canSendHandler;    /**< Pointer to the function to send a CAN message */
};

/*
 * \struct diypinball_featureHandlerInstance
 * \brief Stores information relating to the instance of a FeatureHandler
 */
struct diypinball_featureHandlerInstance {
    uint8_t featureType;                                /**< The feature type of the feature implemented */
    void *concreteFeatureHandlerInstance;               /**< Pointer to the concrete FeatureHandler instance */
    diypinball_featureRouterInstance_t *routerInstance; /**< Pointer to the instance of the FeatureRouter. Provided by the diypinball_featureRouter_addFeature */
    diypinball_messageReceivedHandler messageHandler;   /**< Pointer to the MessageReceivedHandler of the FeatureHandler */
    diypinball_millisecondTickHandler tickHandler;      /**< Pointer to the MillisecondTickHandler of the FeatureHandler */
};

/**
 * \brief Initialize the FeatureRouter from an initialization struct
 *
 * \param[in] featureRouterInstance     FeatureRouter instance struct
 * \param[in] init                      FeatureRouter initialization struct
 *
 * \return Nothing
 */
void diypinball_featureRouter_init(diypinball_featureRouterInstance_t* featureRouterInstance, diypinball_featureRouterInit_t* init);

/**
 * \brief Deinitialize the FeatureRouter
 *
 * \param[in] featureRouterInstance     FeatureRouter instance struct
 *
 * \return Nothing
 */
void diypinball_featureRouter_deinit(diypinball_featureRouterInstance_t* featureRouterInstance);

/**
 * \brief Add a feature's FeatureHandler instance to the FeatureRouter
 *
 * \param[in] featureRouterInstance     FeatureRouter instance struct
 * \param[in] featureHandler            FeatureHandler instance struct
 *
 * \return RESULT_SUCCESS on success, RESULT_FAIL_INVALID_PARAMETER if the featureNum is invalid
 */
diypinball_result_t diypinball_featureRouter_addFeature(diypinball_featureRouterInstance_t* featureRouterInstance, diypinball_featureHandlerInstance_t* featureHandlerInstance);

/**
 * \brief Process a received CAN message, and route it to the proper FeatureHandler
 *
 * \param[in] featureRouterInstance     FeatureRouter instance struct
 * \param[in] message                   CAN message struct
 *
 * \return Nothing
 */
void diypinball_featureRouter_receiveCAN(diypinball_featureRouterInstance_t* featureRouterInstance, diypinball_canMessage_t* message);

/**
 * \brief Get a bitmap of features that have been implemented
 *
 * \param[in] featureRouterInstance     FeatureRouter instance struct
 * \param[out] bitmap                   Feature bitmap (1 bit per implemented feature)
 *
 * \return Nothing
 */
void diypinball_featureRouter_getFeatureBitmap(diypinball_featureRouterInstance_t* featureRouterInstance, uint16_t *bitmap);

/**
 * \brief Distribute a millisecondTick event to all implemented features
 *
 * \param[in] featureRouterInstance     FeatureRouter instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_featureRouter_millisecondTick(diypinball_featureRouterInstance_t* featureRouterInstance, uint32_t tickNum);

/**
 * \brief Send a PinballMessage from a FeatureHandler to the CAN Send handler
 *
 * \param[in] featureRouterInstance     FeatureRouter instance struct
 * \param[in] message                   pinballMessage struct to be sent
 *
 * \return Nothing
 */
void diypinball_featureRouter_sendPinballMessage(diypinball_featureRouterInstance_t *featureRouterInstance, diypinball_pinballMessage_t *message);

#ifdef __cplusplus
}
#endif