#include "diypinball.h"
#include "diypinball_lampMatrixScanner.h"
#include "diypinball_featureRouter.h"

void diypinball_lampMatrixScanner_init(diypinball_lampMatrixScannerInstance_t *instance, diypinball_lampMatrixScannerInit_t *init) {
    instance->numColumns = init->numColumns;
    if(instance->numColumns > 4) instance->numColumns = 4;
    instance->currentColumn = 0;
    instance->lastTick = 0;

    instance->setColumnHandler = init->setColumnHandler;
    instance->setRowHandler = init->setRowHandler;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->lamps[i].lampState.state1 = 0;
        instance->lamps[i].lampState.state1Duration = 0;
        instance->lamps[i].lampState.state2 = 0;
        instance->lamps[i].lampState.state2Duration = 0;
        instance->lamps[i].lampState.state3 = 0;
        instance->lamps[i].lampState.state3Duration = 0;
        instance->lamps[i].lampState.numStates = 0;
        instance->lamps[i].lastTick = 0;
        instance->lamps[i].currentPhase = 0;
    }
}

void diypinball_lampMatrixScanner_millisecondTickHandler(diypinball_lampMatrixScannerInstance_t *instance, uint32_t tickNum) {
    instance->lastTick = tickNum;
}

void diypinball_lampMatrixScanner_deinit(diypinball_lampMatrixScannerInstance_t *instance) {
    instance->numColumns = 0;
    instance->currentColumn = 0;
    instance->lastTick = 0;

    instance->setColumnHandler = NULL;
    instance->setRowHandler = NULL;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->lamps[i].lampState.state1 = 0;
        instance->lamps[i].lampState.state1Duration = 0;
        instance->lamps[i].lampState.state2 = 0;
        instance->lamps[i].lampState.state2Duration = 0;
        instance->lamps[i].lampState.state3 = 0;
        instance->lamps[i].lampState.state3Duration = 0;
        instance->lamps[i].lampState.numStates = 0;
        instance->lamps[i].lastTick = 0;
        instance->lamps[i].currentPhase = 0;
    }
}

void diypinball_lampMatrixScanner_setLampState(diypinball_lampMatrixScannerInstance_t *instance, uint8_t lampNum, diypinball_lampStatus_t *state) {
    if(lampNum >= 16) {
        return;
    }

    instance->lamps[lampNum].lampState.state1 = state->state1;
    instance->lamps[lampNum].lampState.state1Duration = state->state1Duration;
    instance->lamps[lampNum].lampState.state2 = state->state2;
    instance->lamps[lampNum].lampState.state2Duration = state->state2Duration;
    instance->lamps[lampNum].lampState.state3 = state->state3;
    instance->lamps[lampNum].lampState.state3Duration = state->state3Duration;
    instance->lamps[lampNum].lampState.numStates = state->numStates;
    instance->lamps[lampNum].lastTick = instance->lastTick;
    instance->lamps[lampNum].currentPhase = 0;
}

void diypinball_lampMatrixScanner_isr(diypinball_lampMatrixScannerInstance_t *instance, diypinball_lampMatrixScanner_interruptType_t interruptType) {
    if(interruptType == INTERRUPT_RESET) {

    } else if(interruptType == INTERRUPT_MATCH) {

    }
}
