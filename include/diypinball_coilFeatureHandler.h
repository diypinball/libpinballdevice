#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "diypinball.h"
#include "diypinball_featureRouter.h"

#include <stdint.h>

/*
 * \struct diypinball_coilStatus_t diypinball_coilStatus
 * \brief Stores information related to a coil
 */
typedef struct diypinball_coilStatus {
    uint8_t attackState;
    uint8_t attackDuration;
    uint8_t sustainState;
    uint8_t sustainDuration;
} diypinball_coilStatus_t;

/*
 * \brief Function pointer to a coil state change handler, whose implementation is platform-specific
 */
typedef void (*diypinball_coilFeatureHandlerCoilChangedHandler)(uint8_t coilNum, diypinball_coilStatus_t coilStatus);

/*
 * \struct diypinball_coilFeatureHandlerInstance_t diypinball_coilFeatureHandlerInstance
 * \brief Stores information relating to the instance of a CoilFeatureHandler feature
 */
typedef struct diypinball_coilFeatureHandlerInstance {
    diypinball_featureHandlerInstance_t featureHandlerInstance;             /**< featureDecoder instance for the FeatureRouter */
    diypinball_coilStatus_t coils[16];
    uint8_t numCoils;
    diypinball_coilFeatureHandlerCoilChangedHandler coilChangedHandler;
} diypinball_coilFeatureHandlerInstance_t;

/*
 * \struct diypinball_coilFeatureHandlerInit_t diypinball_coilFeatureHandlerInit
 * \brief Stores initialization information to set up a CoilFeatureHandler instance
 */
typedef struct diypinball_coilFeatureHandlerInit {
    uint8_t numCoils;
    diypinball_coilFeatureHandlerCoilChangedHandler coilChangedHandler;
    diypinball_featureRouterInstance_t *routerInstance;                       /**< FeatureRouter instance to connect to */
} diypinball_coilFeatureHandlerInit_t;

/**
 * \brief Initialize the CoilFeatureHandler feature from an initialization struct
 *
 * \param[in] instance                  CoilFeatureHandler instance struct
 * \param[in] init                      CoilFeatureHandler initialization struct
 *
 * \return Nothing
 */
void diypinball_coilFeatureHandler_init(diypinball_coilFeatureHandlerInstance_t *instance, diypinball_coilFeatureHandlerInit_t *init);

/**
 * \brief Handle a millisecondTick event for a CoilFeatureHandler instance
 *
 * \param[in] instance                  CoilFeatureHandler instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_coilFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Process a received Pinball message meant for a CoilFeatureHandler instance
 *
 * \param[in] instance                  CoilFeatureHandler instance struct
 * \param[in] message                   Pinball message struct
 *
 * \return Nothing
 */
void diypinball_coilFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);

/**
 * \brief Deinitialize the CoilFeatureHandler feature
 *
 * \param[in] instance                  CoilFeatureHandler instance struct
 *
 * \return Nothing
 */
void diypinball_coilFeatureHandler_deinit(diypinball_coilFeatureHandlerInstance_t *instance);

#ifdef __cplusplus
}
#endif
