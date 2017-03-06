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
typedef void (*diypinball_systemManagementPowerStatusHandler)(uint8_t *voltages, uint8_t *currents);

/*
 * \struct diypinball_systemManagementInstance_t diypinball_systemManagementInstance
 * \brief Stores information relating to the instance of a SystemManagement feature
 */
typedef struct diypinball_systemManagementInstance {
    diypinball_featureDecoderInstance_t featureDecoderInstance;         /**< featureDecoder instance for the FeatureRouter */
    uint8_t firmwareVersionMajor;                                       /**< Major version number */
    uint8_t firmwareVersionMinor;                                       /**< Minor version number */
    uint8_t firmwareVersionPatch;                                       /**< Patch version number */
    uint8_t powerStatusPollingInterval;                                 /**< Interval in milliseconds between automatic power status messages */
    uint32_t boardSerial[4];                                            /**< Board serial number, 128-bit */
    uint32_t boardSignature[2];                                         /**< Board hardware signature, 64-bit */
    diypinball_systemManagementPowerStatusHandler powerStatusHandler;   /**< Function pointer to power status request handler */
    uint32_t lastTick;                                                  /**< Last tick number received by millisecond tick handler */
} diypinball_systemManagementInstance_t;

/*
 * \struct diypinball_systemManagementInit_t diypinball_systemManagementInit
 * \brief Stores initialization information to set up a SystemManagement instance
 */
typedef struct diypinball_systemManagementInit {
    uint8_t firmwareVersionMajor;                                       /**< Major version number */
    uint8_t firmwareVersionMinor;                                       /**< Minor version number */
    uint8_t firmwareVersionPatch;                                       /**< Patch version number */
    uint32_t boardSerial[4];                                            /**< Board serial number, 128-bit */
    uint32_t boardSignature[2];                                         /**< Board hardware signature, 64-bit */
    diypinball_systemManagementPowerStatusHandler powerStatusHandler;   /**< Function pointer to power status request handler */
    diypinball_featureRouterInstance_t *routerInstance;                 /**< FeatureRouter instance to connect to */
} diypinball_systemManagementInit_t;

/**
 * \brief Initialize the SystemManagement feature from an initialization struct
 *
 * \param[in] instance                  SystemManagement instance struct
 * \param[in] init                      SystemManagement initialization struct
 *
 * \return Nothing
 */
void diypinball_systemManagement_init(diypinball_systemManagementInstance_t *instance, diypinball_systemManagementInit_t *init);

/**
 * \brief Handle a millisecondTick event for a SystemManagement instance
 *
 * \param[in] instance                  SystemManagement instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_systemManagement_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Process a received Pinball message meant for a SystemManagement instance
 *
 * \param[in] instance                  SystemManagement instance struct
 * \param[in] message                   Pinball message struct
 *
 * \return Nothing
 */
void diypinball_systemManagement_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);

/**
 * \brief Send the two Board ID messages on system powerup
 *
 * \param[in] instance                  SystemManagement instance struct
 *
 * \return Nothing
 */
void diypinball_systemManagement_sendPoweronMessages(diypinball_systemManagementInstance_t *instance);

/**
 * \brief Deinitialize the SystemManagement feature
 *
 * \param[in] instance                  SystemManagement instance struct
 *
 * \return Nothing
 */
void diypinball_systemManagement_deinit(diypinball_systemManagementInstance_t *instance);

#ifdef __cplusplus
}
#endif
