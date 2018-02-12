#include "diypinball.h"
#include "diypinball_coilEnvelopeGenerator.h"
#include "diypinball_coilFeatureHandler.h"

void diypinball_coilEnvelopeGenerator_init(diypinball_coilEnvelopeGeneratorInstance_t *instance, diypinball_coilEnvelopeGeneratorInit_t *init) {
    instance->numCoils = init->numCoils;
    if(instance->numCoils > 16) instance->numCoils = 16;

    instance->coilStateHandler = init->coilStateHandler;
    instance->lastTick = 0;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->coils[i].attackState = 0;
        instance->coils[i].attackDuration = 0;
        instance->coils[i].sustainState = 0;
        instance->coils[i].sustainDuration = 0;
        instance->lastTicks[i] = 0;
        instance->lastPhases[i] = 0;
    }
}

void diypinball_coilEnvelopeGenerator_millisecondTickHandler(diypinball_coilEnvelopeGeneratorInstance_t *instance, uint32_t tickNum) {
    instance->lastTick = tickNum;

    uint8_t i;

    for(i=0; i < instance->numCoils; i++) {
        if(instance->lastPhases[i] == 1) {
            if(instance->coils[i].attackDuration == 0) {
                // always attack
                instance->coilStateHandler(i, instance->coils[i].attackState);
            } else {
                // attack until time elapses
                if((tickNum - instance->lastTicks[i]) >= (instance->coils[i].attackDuration * 10)) {
                    // elapsed!
                    instance->coilStateHandler(i, instance->coils[i].sustainState);
                    instance->lastPhases[i] = 2;
                } else {
                    instance->coilStateHandler(i, instance->coils[i].attackState);
                }
            }
        } else if(instance->lastPhases[i] == 2) {
            if(instance->coils[i].sustainDuration == 0) {
                // always sustain
                instance->coilStateHandler(i, instance->coils[i].sustainState);
            } else {
                // sustain until time elapses
                if((tickNum - instance->lastTicks[i]) >= (instance->coils[i].sustainDuration * 10)) {
                    // elapsed!
                    instance->coilStateHandler(i, 0);
                    instance->lastPhases[i] = 0;
                } else {
                    instance->coilStateHandler(i, instance->coils[i].sustainState);
                }
            }
        } else {
            instance->coilStateHandler(i, 0);
            instance->lastPhases[i] = 0;
        }
    }
}

void diypinball_coilEnvelopeGenerator_deinit(diypinball_coilEnvelopeGeneratorInstance_t *instance) {
    instance->numCoils = 0;
    instance->lastTick = 0;

    instance->coilStateHandler = NULL;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->coils[i].attackState = 0;
        instance->coils[i].attackDuration = 0;
        instance->coils[i].sustainState = 0;
        instance->coils[i].sustainDuration = 0;
        instance->lastTicks[i] = 0;
        instance->lastPhases[i] = 0;
    }
}

void diypinball_coilEnvelopeGenerator_setCoilState(diypinball_coilEnvelopeGeneratorInstance_t *instance, uint8_t coilNum, diypinball_coilStatus_t *status) {
    if(coilNum >= instance->numCoils) {
        return;
    }

    instance->coils[coilNum].attackState = status->attackState;
    instance->coils[coilNum].attackDuration = status->attackDuration;
    instance->coils[coilNum].sustainState = status->sustainState;
    instance->coils[coilNum].sustainDuration = status->sustainDuration;

    instance->lastTicks[coilNum] = instance->lastTick;
    instance->lastPhases[coilNum] = 1;
    instance->coilStateHandler(coilNum, instance->coils[coilNum].attackState);
}
