#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "diypinball.h"
#include "diypinball_featureRouter.h"

#include <stdint.h>

/*
 * \brief Function pointer to a read switch state handler, whose implementation is platform-specific
 */
typedef void (*diypinball_switchMatrixReadStateHandler)(uint8_t *state, uint8_t switchNum);

/*
 * \brief Function pointer to a debounce parameter change handler, whose implementation is platform-specific
 */
typedef void (*diypinball_switchMatrixDebounceChangedHandler)(uint8_t switchNum, uint8_t debounceLimit);

/*
 * \struct diypinball_switchMatrixRule_t diypinball_switchMatrixRule
 * \brief Stores information related to a switch matrix hardware rule
 */
typedef struct diypinball_switchMatrixRule {
    uint8_t boardAddress;                                                   /**< Board address to send the rule's message to */
    uint8_t solenoidNum;                                                    /**< Which solenoid to change on the target board */
    uint8_t attackStatus;                                                   /**< The PWM level at which to drive the solenoid during attack */
    uint8_t attackDuration;                                                 /**< The duration of the solenoid attack phase, in 10ms units */
    uint8_t sustainStatus;                                                  /**< The PWM level at which to drive the solenoid during sustain */
    uint8_t sustainDuration;                                                /**< The duration of the solenoid sustain phase */
} diypinball_switchMatrixRule_t;

/*
 * \struct diypinball_switchMatrixStatus_t diypinball_switchMatrixStatus
 * \brief Stores information related to an individual switch in the matrix
 */
typedef struct diypinball_switchMatrixStatus {
    uint8_t lastState;                                                      /**< The previous state of the switch */
    uint8_t messageTriggerMask;                                             /**< The mask to indicate which transitions should trigger an automatic response */
    uint8_t pollingInterval;                                                /**< Interval to automatically send out switch status messages */
    uint32_t lastTick;                                                      /**< Last timer tick */
    uint8_t debounceLimit;                                                  /**< Debounce limit parameter */
    uint8_t ruleMask;                                                       /**< Event mask for firing hardware rules */
    diypinball_switchMatrixRule_t closeRule;                                /**< Rule for when the switch is closed */
    diypinball_switchMatrixRule_t openRule;                                 /**< Rule for when the switch is opened */
} diypinball_switchMatrixStatus_t;

/*
 * \struct diypinball_switchMatrixInstance_t diypinball_switchMatrixInstance
 * \brief Stores information relating to the instance of a SwitchMatrix feature
 */
typedef struct diypinball_switchMatrixInstance {
    diypinball_featureDecoderInstance_t featureDecoderInstance;             /**< featureDecoder instance for the FeatureRouter */
    diypinball_switchMatrixStatus_t switches[16];                           /**< Array of switch status objects */
    uint8_t numSwitches;                                                    /**< The number of switches to be scanned */
    diypinball_switchMatrixReadStateHandler readStateHandler;               /**< Function pointer to the read switch state handler */
    diypinball_switchMatrixDebounceChangedHandler debounceChangedHandler;   /**< Function pointer to the debounce parameter change handler */
} diypinball_switchMatrixInstance_t;

/*
 * \struct diypinball_switchMatrixInit_t diypinball_switchMatrixInit
 * \brief Stores initialization information to set up a SwitchMatrix instance
 */
typedef struct diypinball_switchMatrixInit {
    uint8_t numSwitches;                                                    /**< The number of switches to be scanned */
    diypinball_switchMatrixReadStateHandler readStateHandler;               /**< Function pointer to the read switch state handler */
    diypinball_switchMatrixDebounceChangedHandler debounceChangedHandler;   /**< Function pointer to the debounce parameter change handler */
    diypinball_featureRouterInstance_t *routerInstance;                       /**< FeatureRouter instance to connect to */
} diypinball_switchMatrixInit_t;

/**
 * \brief Initialize the SwitchMatrix feature from an initialization struct
 *
 * \param[in] instance                  SwitchMatrix instance struct
 * \param[in] init                      SwitchMatrix initialization struct
 *
 * \return Nothing
 */
void diypinball_switchMatrix_init(diypinball_switchMatrixInstance_t *instance, diypinball_switchMatrixInit_t *init);

/**
 * \brief Handle a millisecondTick event for a SwitchMatrix instance
 *
 * \param[in] instance                  SwitchMatrix instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_switchMatrix_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Process a received Pinball message meant for a SwitchMatrix instance
 *
 * \param[in] instance                  SwitchMatrix instance struct
 * \param[in] message                   Pinball message struct
 *
 * \return Nothing
 */
void diypinball_switchMatrix_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);

/**
 * \brief Deinitialize the SwitchMatrix feature
 *
 * \param[in] instance                  SwitchMatrix instance struct
 *
 * \return Nothing
 */
void diypinball_switchMatrix_deinit(diypinball_switchMatrixInstance_t *instance);

#ifdef __cplusplus
}
#endif
