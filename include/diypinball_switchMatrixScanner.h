#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "diypinball.h"
#include "diypinball_switchFeatureHandler.h"

#include <stdint.h>

/*
 * \brief Pinball message type
 */
typedef enum diypinball_switchMatrixScanner_interruptType {
    INTERRUPT_RESET,                            /**< Timer rollover */
    INTERRUPT_MATCH_1,                          /**< Timer match 1 */
    INTERRUPT_MATCH_2                           /**< Timer match 2 */
} diypinball_swtchMatrixScanner_interruptType_t;

/*
 * \brief Function pointer to a switch state handler, whose implementation is platform-specific
 */
typedef void (*diypinball_switchMatrixScannerSwitchStateHandler)(uint8_t switchNum, uint8_t state);

/*
 * \brief Function pointer to a set column handler, whose implementation is platform-specific
 */
typedef void (*diypinball_switchMatrixScannerSetColumnHandler)(uint8_t colNum);

/*
 * \brief Function pointer to a read row handler, whose implementation is platform-specific
 */
typedef void (*diypinball_switchMatrixScannerReadRowHandler)(uint8_t *row);

/*
 * \struct diypinball_switchMatrixStatus_t diypinball_switchMatrixStatus
 * \brief Stores information related to an individual switch in the matrix
 */
typedef struct diypinball_switchMatrixStatus {
    volatile uint8_t switchState                                            /**< The previous state of the switch */
    volatile uint8_t switchChanged;                                         /**< Did the switch just change? */
    volatile uint32_t lastTick;                                             /**< Last timer tick where a change occured*/
    volatile uint8_t debounceLimit;                                         /**< Debounce limit parameter */
} diypinball_switchMatriStatus_t;

/*
 * \struct diypinball_switchMatrixScannerInstance_t diypinball_switchMatrixScannerInstance
 * \brief Stores information relating to the instance of a SwitchMatrixScanner
 */
typedef struct diypinball_switchMatrixScannerInstance {
    diypinball_switchMatrixStatus_t switches[16];                           /**< Array of switch status objects */
    volatile uint8_t numColumns;                                            /**< The number of columns to be scanned */
    volatile uint8_t currentColumn;                                         /**< The current column being scanned */
    volatile uint32_t currentTick;                                          /**< Most recent tick number */
    diypinball_switchMatrixScannerSwitchStateHandler switchStateHandler;    /**< Function pointer to the switch state handler */
    diypinball_switchMatrixScannerSetColumnHandler setColumnHandler;        /**< Function pointer to the set column handler */
    diypinball_switchMatrixScannerReadRowHandler readRowHandler             /**< Function pointer to the read row handler */
} diypinball_switchMatrixScannerInstance_t;

/*
 * \struct diypinball_switchMatrixScannerInit_t diypinball_switchMatrixScannerInit
 * \brief Stores initialization information to set up a SwitchMatrixScanner instance
 */
typedef struct diypinball_switchMatrixScannerInit {
    uint8_t numColumns;                                                     /**< The number of columns to be scanned */
    diypinball_switchMatrixScannerSwitchStateHandler switchStateHandler;    /**< Function pointer to the switch state handler */
    diypinball_switchMatrixScannerSetColumnHandler setColumnHandler;        /**< Function pointer to the set column handler */
    diypinball_switchMatrixScannerReadRowHandler readRowHandler             /**< Function pointer to the read row handler */
} diypinball_switchMatrixScannerInit_t;

/**
 * \brief Initialize the SwitchMatrixScanner feature from an initialization struct
 *
 * \param[in] instance                  SwitchMatrixScanner instance struct
 * \param[in] init                      SwitchMatrixScanner initialization struct
 *
 * \return Nothing
 */
void diypinball_switchMatrixScanner_init(diypinball_switchMatrixScannerInstance_t *instance, diypinball_switchMatrixScannerInit_t *init);

/**
 * \brief Handle a millisecondTick event for a SwitchMatrixScanner instance
 *
 * \param[in] instance                  SwitchMatrixScanner instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_switchMatrixScanner_millisecondTickHandler(void *instance, uint32_t tickNum);

/**
 * \brief Deinitialize the SwitchMatrixScanner feature
 *
 * \param[in] instance                  SwitchMatrixScanner instance struct
 *
 * \return Nothing
 */
void diypinball_switchMatrixScanner_deinit(diypinball_switchMatrixScannerInstance_t *instance);

/**
 * \brief Read a switch state from the SwitchMatrixScanner
 *
 * \param[in] instance                  SwitchMatrixScanner instance struct
 * \param[in] switchNum                 Which switch is being read
 *
 * \return Switch state (1 = closed, 0 = open)
 */
uint8_t diypinball_switchMatrixScanner_readSwitchState(diypinball_switchMatrixScannerInstance_t *instance, uint8_t switchNum);

/**
 * \brief Set a debounce limit in the SwitchMatrixScanner
 *
 * \param[in] instance                  SwitchMatrixScanner instance struct
 * \param[in] switchNum                 Which switch is being read
 * \param[in] debounceLimit             Debounce limit parameter
 *
 * \return Nothing
 */
void diypinball_switchMatrixScanner_setDebounceLimit(diypinball_switchMatrixScannerInstance_t *instance, uint8_t switchNum, uint8_t debounceLimit);

/**
 * \brief Pass an interrupt to the SwitchMatrixScanner
 *
 * \param[in] instance                  SwitchMatrixScanner instance struct
 * \param[in] interruptType             Which interrupt is being passed
 *
 * \return Nothing
 */
void diypinball_switchMatrixScanner_isr(diypinball_switchMatrixScannerInstance_t *instance, diypinball_swtchMatrixScanner_interruptType_t interruptType);

#ifdef __cplusplus
}
#endif
