#include "diypinball.h"
#include "diypinball_switchMatrixScanner.h"

static void readMatrixRow(diypinball_switchMatrixScannerInstance_t *instance) {
    uint8_t rowBuffer, switchBuffer, switchNum;

    instance->readRowHandler(&rowBuffer);

    uint8_t i;
    for(i=0; i<4; i++) {
        switchBuffer = (rowBuffer & (1 << i)) ? 1 : 0;
        switchNum = (instance->currentColumn * 4) + i;

        if((instance->lastTick - instance->switches[switchNum].lastTick) >= instance->switches[switchNum].debounceLimit) {
            if(switchBuffer != instance->switches[switchNum].switchState) {
                instance->switches[switchNum].lastTick = instance->lastTick;
                instance->switches[switchNum].switchState = switchBuffer;
                instance->switchStateHandler(switchNum, switchBuffer);
            }
        }
    }
}

void diypinball_switchMatrixScanner_init(diypinball_switchMatrixScannerInstance_t *instance, diypinball_switchMatrixScannerInit_t *init) {
    instance->numColumns = init->numColumns;
    if(instance->numColumns > 4) instance->numColumns = 4;
    instance->lastTick = 0;
    instance->currentColumn = 0;

    instance->switchStateHandler = init->switchStateHandler;
    instance->setColumnHandler = init->setColumnHandler;
    instance->readRowHandler = init->readRowHandler;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->switches[i].switchState = 0;
        instance->switches[i].lastTick = 0;
        instance->switches[i].debounceLimit = 0;
    }
}

void diypinball_switchMatrixScanner_millisecondTickHandler(diypinball_switchMatrixScannerInstance_t *instance, uint32_t tickNum) {
    instance->lastTick = tickNum;
}

void diypinball_switchMatrixScanner_deinit(diypinball_switchMatrixScannerInstance_t *instance) {
    instance->numColumns = 0;
    instance->lastTick = 0;
    instance->currentColumn = 0;

    instance->switchStateHandler = NULL;
    instance->setColumnHandler = NULL;
    instance->readRowHandler = NULL;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->switches[i].switchState = 0;
        instance->switches[i].lastTick = 0;
        instance->switches[i].debounceLimit = 0;
    }
}

uint8_t diypinball_switchMatrixScanner_readSwitchState(diypinball_switchMatrixScannerInstance_t *instance, uint8_t switchNum) {
    if(switchNum >= 16) {
        return 0;
    }

    return instance->switches[switchNum].switchState;
}

void diypinball_switchMatrixScanner_setDebounceLimit(diypinball_switchMatrixScannerInstance_t *instance, uint8_t switchNum, uint8_t debounceLimit) {
    if(switchNum >= 16) {
        return;
    }

    instance->switches[switchNum].debounceLimit = debounceLimit;
}

void diypinball_switchMatrixScanner_isr(diypinball_switchMatrixScannerInstance_t *instance, diypinball_swtchMatrixScanner_interruptType_t interruptType) {
    if(interruptType == INTERRUPT_RESET) {
        // deassert the columns
        instance->setColumnHandler(-1);
    } else if(interruptType == INTERRUPT_MATCH_1) {
        // assert the column
        instance->setColumnHandler((int8_t) instance->currentColumn);
    } else if(interruptType == INTERRUPT_MATCH_2) {
        readMatrixRow(instance);
        // increment the current column
        instance->currentColumn = instance->currentColumn + 1;
        if(instance->currentColumn >= instance->numColumns) {
            instance->currentColumn = 0;
        }
    }
}
