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
typedef void (*diypinball_switchFeatureHandlerReadStateHandler)(uint8_t *state, uint8_t switchNum);

/*
 * \brief Function pointer to a debounce parameter change handler, whose implementation is platform-specific
 */
typedef void (*diypinball_switchFeatureHandlerDebounceChangedHandler)(uint8_t switchNum, uint8_t debounceLimit);

/*
 * \struct diypinball_switchRule_t diypinball_switchRule
 * \brief Stores information related to a switch matrix hardware rule
 */
typedef struct diypinball_switchRule {
    uint8_t boardAddress;                                                   /**< Board address to send the rule's message to */
    uint8_t solenoidNum;                                                    /**< Which solenoid to change on the target board */
    uint8_t attackStatus;                                                   /**< The PWM level at which to drive the solenoid during attack */
    uint8_t attackDuration;                                                 /**< The duration of the solenoid attack phase, in 10ms units */
    uint8_t sustainStatus;                                                  /**< The PWM level at which to drive the solenoid during sustain */
    uint8_t sustainDuration;                                                /**< The duration of the solenoid sustain phase */
} diypinball_switchRule_t;

/*
 * \struct diypinball_switchStatus_t diypinball_switchStatus
 * \brief Stores information related to an individual switch in the matrix
 */
typedef struct diypinball_switchStatus {
    uint8_t lastState;                                                      /**< The previous state of the switch */
    uint8_t messageTriggerMask;                                             /**< The mask to indicate which transitions should trigger an automatic response */
    uint8_t pollingInterval;                                                /**< Interval to automatically send out switch status messages */
    uint32_t lastTick;                                                      /**< Last timer tick */
    uint8_t debounceLimit;                                                  /**< Debounce limit parameter */
    uint8_t ruleMask;                                                       /**< Event mask for firing hardware rules */
    diypinball_switchRule_t closeRule;                                /**< Rule for when the switch is closed */
    diypinball_switchRule_t openRule;                                 /**< Rule for when the switch is opened */
} diypinball_switchStatus_t;

/*
 * \struct diypinball_switchFeatureHandlerInstance_t diypinball_switchFeatureHandlerInstance
 * \brief Stores information relating to the instance of a SwitchFeatureHandler feature
 */
typedef struct diypinball_switchFeatureHandlerInstance {
    diypinball_featureHandlerInstance_t featureHandlerInstance;             /**< featureDecoder instance for the FeatureRouter */
    diypinball_switchStatus_t switches[16];                           /**< Array of switch status objects */
    uint8_t numSwitches;                                                    /**< The number of switches to be scanned */
    diypinball_switchFeatureHandlerReadStateHandler readStateHandler;               /**< Function pointer to the read switch state handler */
    diypinball_switchFeatureHandlerDebounceChangedHandler debounceChangedHandler;   /**< Function pointer to the debounce parameter change handler */
} diypinball_switchFeatureHandlerInstance_t;

/*
 * \struct diypinball_switchFeatureHandlerInit_t diypinball_switchFeatureHandlerInit
 * \brief Stores initialization information to set up a SwitchFeatureHandler instance
 */
typedef struct diypinball_switchFeatureHandlerInit {
    uint8_t numSwitches;                                                    /**< The number of switches to be scanned */
    diypinball_switchFeatureHandlerReadStateHandler readStateHandler;               /**< Function pointer to the read switch state handler */
    diypinball_switchFeatureHandlerDebounceChangedHandler debounceChangedHandler;   /**< Function pointer to the debounce parameter change handler */
    diypinball_featureRouterInstance_t *routerInstance;                       /**< FeatureRouter instance to connect to */
} diypinball_switchFeatureHandlerInit_t;

/**
 * \brief Initialize the SwitchFeatureHandler feature from an initialization struct
 *
 * \param[in] instance                  SwitchFeatureHandler instance struct
 * \param[in] init                      SwitchFeatureHandler initialization struct
 *
 * \return Nothing
 */
void diypinball_switchFeatureHandler_init(diypinball_switchFeatureHandlerInstance_t *instance, diypinball_switchFeatureHandlerInit_t *init);

/**
 * \brief Handle a millisecondTick event for a SwitchFeatureHandler instance
 *
 * \param[in] instance                  SwitchFeatureHandler instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_switchFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Process a received Pinball message meant for a SwitchFeatureHandler instance
 *
 * \param[in] instance                  SwitchFeatureHandler instance struct
 * \param[in] message                   Pinball message struct
 *
 * \return Nothing
 */
void diypinball_switchFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);

/**
 * \brief Deinitialize the SwitchFeatureHandler feature
 *
 * \param[in] instance                  SwitchFeatureHandler instance struct
 *
 * \return Nothing
 */
void diypinball_switchFeatureHandler_deinit(diypinball_switchFeatureHandlerInstance_t *instance);

/**
 * \brief Register a switch state update with the SwitchFeatureHandler
 *
 * \param[in] instance                  SwitchFeatureHandler instance struct
 * \param[in] switchNum                 Which switch is being updated
 * \param[in] state                     Current state (0 = open, 1 = closed)
 *
 * \return Nothing
 */
void diypinball_switchFeatureHandler_registerSwitchState(diypinball_switchFeatureHandlerInstance_t *instance, uint8_t switchNum, uint8_t state);

#ifdef __cplusplus
}
#endif
