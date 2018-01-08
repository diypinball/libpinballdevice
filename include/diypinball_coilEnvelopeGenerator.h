#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "diypinball.h"
#include "diypinball_coilFeatureHandler.h"

#include <stdint.h>

/*
 * \brief Function pointer to a switch state handler, whose implementation is platform-specific
 */
typedef void (*diypinball_coilEnvelopeGeneratorCoilStateHandler)(uint8_t coilNum, uint8_t state);

/*
 * \struct diypinball_coilEnvelopeGeneratorInstance_t diypinball_coilEnvelopeGeneratorInstance
 * \brief Stores information relating to the instance of a CoilEnvelopeGenerator
 */
typedef struct diypinball_coilEnvelopeGeneratorInstance {
    diypinball_coilStatus_t coils[16];                                          /**< Array of coil status objects */
    uint8_t numCoils;                                                           /**< The number of coils to be scanned */
    uint32_t lastTicks[16];                                                     /**< Most recent tick number */
    uint32_t lastTick;
    uint8_t lastPhases[16];                                                     /**< Most recent coil phase */
    diypinball_coilEnvelopeGeneratorCoilStateHandler coilStateHandler;         /**< Function pointer to the coil state handler */
} diypinball_coilEnvelopeGeneratorInstance_t;

/*
 * \struct diypinball_coilEnvelopeGeneratorInit_t diypinball_coilEnvelopeGeneratorInit
 * \brief Stores initialization information to set up a CoilEnvelopeGenerator instance
 */
typedef struct diypinball_coilEnvelopeGeneratorInit {
    uint8_t numCoils;                                                           /**< The number of coils to be scanned */
    diypinball_coilEnvelopeGeneratorCoilStateHandler coilStateHandler;         /**< Function pointer to the coil state handler */
} diypinball_coilEnvelopeGeneratorInit_t;

/**
 * \brief Initialize the CoilEnvelopeGenerator feature from an initialization struct
 *
 * \param[in] instance                  CoilEnvelopeGenerator instance struct
 * \param[in] init                      CoilEnvelopeGenerator initialization struct
 *
 * \return Nothing
 */
void diypinball_coilEnvelopeGenerator_init(diypinball_coilEnvelopeGeneratorInstance_t *instance, diypinball_coilEnvelopeGeneratorInit_t *init);

/**
 * \brief Handle a millisecondTick event for a CoilEnvelopeGenerator instance
 *
 * \param[in] instance                  CoilEnvelopeGenerator instance struct
 * \param[in] tickNum                   Current timer tick
 *
 * \return Nothing
 */
void diypinball_coilEnvelopeGenerator_millisecondTickHandler(diypinball_coilEnvelopeGeneratorInstance_t *instance, uint32_t tickNum);

/**
 * \brief Deinitialize the CoilEnvelopeGenerator feature
 *
 * \param[in] instance                  CoilEnvelopeGenerator instance struct
 *
 * \return Nothing
 */
void diypinball_coilEnvelopeGenerator_deinit(diypinball_coilEnvelopeGeneratorInstance_t *instance);

/**
 * \brief Set a coil state in the CoilEnvelopeGenerator
 *
 * \param[in] instance                  CoilEnvelopeGenerator instance struct
 * \param[in] switchNum                 Which coil is being set
 * \param[in] status                    Coil status
 *
 * \return Nothing
 */
void diypinball_coilEnvelopeGenerator_setCoilState(diypinball_coilEnvelopeGeneratorInstance_t *instance, uint8_t coilNum, diypinball_coilStatus_t *status);

#ifdef __cplusplus
}
#endif
