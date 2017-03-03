#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "diypinball.h"

#include <stdint.h>

typedef struct diypinball_featureRouterInstance diypinball_featureRouterInstance_t;
typedef struct diypinball_featureRouterInit diypinball_featureRouterInit_t;
typedef struct diypinball_featureDecoderInstance diypinball_featureDecoderInstance_t;

/*
 * \brief Function pointer to a message received handler, implemented by a FeatureDecoder
 */
typedef void (*diypinball_messageReceivedHandler)(void *featureDecoderInstance, diypinball_pinballMessage_t *message);

/*
 * \brief Function pointer to a millisecond tick handler, implemented by a FeatureDecoder
 */
typedef void (*diypinball_millisecondTickHandler)(void *featureDecoderInstance, uint32_t tickNum);

/*
 * \struct diypinball_featureRouterInstance
 * \brief Stores information relating to the instance of a FeatureRouter
 */
struct diypinball_featureRouterInstance {
    uint8_t boardAddress;                               /**< The board address for this FeatureRouter */
    diypinball_featureDecoderInstance_t* features[16];  /**< Array of pointers to the implemented FeatureDecoders */
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
 * \struct diypinball_featureDecoderInstance
 * \brief Stores information relating to the instance of a FeatureDecoder
 */
struct diypinball_featureDecoderInstance {
    uint8_t featureType;                                /**< The feature type of the feature implemented */
    void *concreteFeatureDecoderInstance;               /**< Pointer to the concrete FeatureDecoder instance */
    diypinball_featureRouterInstance_t *routerInstance; /**< Pointer to the instance of the FeatureRouter. Provided by the diypinball_featureRouter_addFeature */
    diypinball_messageReceivedHandler messageHandler;   /**< Pointer to the MessageReceivedHandler of the FeatureDecoder */
    diypinball_millisecondTickHandler tickHandler;      /**< Pointer to the MillisecondTickHandler of the FeatureDecoder */
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
 * \brief Add a feature's FeatureDecoder instance to the FeatureRouter
 *
 * \param[in] featureRouterInstance     FeatureRouter instance struct
 * \param[in] featureDecoder            FeatureDecoder instance struct
 *
 * \return RESULT_SUCCESS on success, RESULT_FAIL_INVALID_PARAMETER if the featureNum is invalid
 */
diypinball_result_t diypinball_featureRouter_addFeature(diypinball_featureRouterInstance_t* featureRouterInstance, diypinball_featureDecoderInstance_t* featureDecoderInstance);

/**
 * \brief Process a received CAN message, and route it to the proper FeatureDecoder
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
 * \brief Send a PinballMessage from a FeatureDecoder to the CAN Send handler
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