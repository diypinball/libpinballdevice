#include "diypinball.h"
#include "diypinball_lampMatrixScanner.h"
#include "diypinball_featureRouter.h"

static void setRowValues(diypinball_lampMatrixScannerInstance_t *instance) {
    uint8_t rowValues[4];
    uint8_t i;
    uint8_t j;

    j = instance->currentColumn * 4;

    for(i=0; i<4; i++) {
        switch(instance->lamps[i+j].currentPhase) {
            case 0:
                rowValues[i] = instance->lamps[i+j].lampState.state1;
                break;
            case 1:
                rowValues[i] = instance->lamps[i+j].lampState.state2;
                break;
            case 2:
                rowValues[i] = instance->lamps[i+j].lampState.state3;
                break;
            default:
                rowValues[i] = 0;
                break;
        }
    }

    instance->setRowHandler(rowValues[0], rowValues[1], rowValues[2], rowValues[3]);
}

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

    uint8_t i;
    uint32_t timerCompare;
    uint8_t newPhase;

    for(i=0; i<16; i++) {
        if(instance->lamps[i].lampState.numStates > 1) {
            switch(instance->lamps[i].currentPhase) {
                case 0:
                    timerCompare = instance->lamps[i].lampState.state1Duration * 10;
                    if(instance->lamps[i].lampState.numStates > 1) {
                        newPhase = 1;
                    } else {
                        newPhase = 0;
                    }
                    break;
                case 1:
                    timerCompare = instance->lamps[i].lampState.state2Duration * 10;
                    if(instance->lamps[i].lampState.numStates > 2) {
                        newPhase = 2;
                    } else {
                        newPhase = 0;
                    }
                    break;
                case 2:
                    timerCompare = instance->lamps[i].lampState.state3Duration * 10;
                    newPhase = 0;
                    break;
                default:
                    timerCompare = tickNum;
                    newPhase = 0;
                    break;
            }
            if((timerCompare > 0) && ((tickNum - instance->lamps[i].lastTick) >= timerCompare)) {
                // non-zero threshold means we can advance to the next phase. zero means stay in that phase forever.
                instance->lamps[i].currentPhase = newPhase;
                instance->lamps[i].lastTick = tickNum;
            }
        } else if(instance->lamps[i].lampState.numStates == 1) {
            // Stay in phase 0
            instance->lamps[i].currentPhase = 0;
            instance->lamps[i].lastTick = tickNum;
        } else {
            // Stay in phase 0
            instance->lamps[i].currentPhase = 0;
            instance->lamps[i].lastTick = tickNum;
        }
    }
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
    if(interruptType == LAMP_INTERRUPT_RESET) {
        // clear the columns and rows
        instance->setColumnHandler(-1);
        instance->setRowHandler(0, 0, 0, 0);
    } else if(interruptType == LAMP_INTERRUPT_MATCH) {
        instance->setColumnHandler(instance->currentColumn);
        setRowValues(instance);
        // increment the current column
        instance->currentColumn = instance->currentColumn + 1;
        if(instance->currentColumn >= instance->numColumns) {
            instance->currentColumn = 0;
        }
    }
}
