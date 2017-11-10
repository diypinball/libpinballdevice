#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "diypinball.h"
#include "diypinball_featureRouter.h"

#include <stdint.h>

/*
 * \struct diypinball_rgbStatus_t diypinball_rgbStatus
 * \brief Stores information related to a rgb
 */
typedef struct diypinball_rgbStatus {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} diypinball_rgbStatus_t;

/*
 * \brief Function pointer to a read switch state handler, whose implementation is platform-specific
 */
typedef void (*diypinball_rgbFeatureHandlerRGBChangedHandler)(uint8_t rgbNum, diypinball_rgbStatus_t rgbStatus);

/*
 * \struct diypinball_rgbFeatureHandlerInstance_t diypinball_rgbFeatureHandlerInstance
 * \brief Stores information relating to the instance of a RGBFeatureHandler feature
 */
typedef struct diypinball_rgbFeatureHandlerInstance {
    diypinball_featureHandlerInstance_t featureHandlerInstance;             /**< featureDecoder instance for the FeatureRouter */
    diypinball_rgbStatus_t rgbs[16];
    uint8_t numRGBs;
    diypinball_rgbFeatureHandlerRGBChangedHandler rgbChangedHandler;
} diypinball_rgbFeatureHandlerInstance_t;

/*
 * \struct diypinball_diypinball_rgbFeatureHandlerInit_t diypinball_rgbFeatureHandlerInit
 * \brief Stores initialization information to set up a RGBFeatureHandler instance
 */
typedef struct diypinball_rgbFeatureHandlerInit {
    uint8_t numRGBs;
    diypinball_rgbFeatureHandlerRGBChangedHandler rgbChangedHandler;
    diypinball_featureRouterInstance_t *routerInstance;                       /**< FeatureRouter instance to connect to */
} diypinball_rgbFeatureHandlerInit_t;

/**
 * \brief Initialize the RGBFeatureHandler feature from an initialization struct
 *
 * \param[in] instance                  RGBFeatureHandler instance struct
 * \param[in] init                      RGBFeatureHandler initialization struct
 *
 * \return Nothing
 */
void diypinball_rgbFeatureHandler_init(diypinball_rgbFeatureHandlerInstance_t *instance, diypinball_rgbFeatureHandlerInit_t *init);

/**
 * \brief Handle a millisecondTick event for a RGBFeatureHandler instance
 *
 * \param[in] instance                  RGBFeatureHandler instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_rgbFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Process a received Pinball message meant for a RGBFeatureHandler instance
 *
 * \param[in] instance                  RGBFeatureHandler instance struct
 * \param[in] message                   Pinball message struct
 *
 * \return Nothing
 */
void diypinball_rgbFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);

/**
 * \brief Deinitialize the RGBFeatureHandler feature
 *
 * \param[in] instance                  RGBFeatureHandler instance struct
 *
 * \return Nothing
 */
void diypinball_rgbFeatureHandler_deinit(diypinball_rgbFeatureHandlerInstance_t *instance);

#ifdef __cplusplus
}
#endif
