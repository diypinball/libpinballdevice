#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_switchMatrix.h"

static void sendSwitchUpdate(diypinball_switchMatrixInstance_t *instance, uint8_t switchNum, uint8_t state, uint8_t priority) {
    diypinball_pinballMessage_t response;

    response.priority = priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x01;
    response.featureNum = switchNum;
    response.function = 0x00;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.dataLength = 2;
    response.data[0] = state;
    if(state && !(instance->switches[switchNum].lastState)) {
        response.data[1] = 1;
    } else if((!state) && instance->switches[switchNum].lastState) {
        response.data[1] = 2;
    } else {
        response.data[1] = 0;
    }

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &response);
}

static void sendSwitchStatus(diypinball_switchMatrixInstance_t *instance, diypinball_pinballMessage_t *message) {
    uint8_t newState;
    uint8_t switchNum = message->featureNum;
    if(switchNum >= instance->numSwitches) {
        return;
    }

    (instance->readStateHandler)(&newState, switchNum);

    sendSwitchUpdate(instance, switchNum, newState, message->priority);
    if(newState != instance->switches[switchNum].lastState) {
        instance->switches[switchNum].lastState = newState;
    }
}

static void sendSwitchPolling(diypinball_switchMatrixInstance_t *instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    uint8_t switchNum = message->featureNum;
    if(switchNum >= instance->numSwitches) {
        return;
    }

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x01;
    response.featureNum = switchNum;
    response.function = 0x01;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.dataLength = 1;
    response.data[0] = instance->switches[switchNum].pollingInterval;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &response);
}

static void setSwitchPolling(diypinball_switchMatrixInstance_t *instance, diypinball_pinballMessage_t *message) {
    uint8_t switchNum = message->featureNum;
    if(switchNum >= instance->numSwitches) {
        return;
    }

    if(message->dataLength < 1) {
        return;
    }

    instance->switches[switchNum].pollingInterval = message->data[0];
}

static void sendSwitchTriggering(diypinball_switchMatrixInstance_t *instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    uint8_t switchNum = message->featureNum;
    if(switchNum >= instance->numSwitches) {
        return;
    }

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x01;
    response.featureNum = switchNum;
    response.function = 0x02;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.dataLength = 1;
    response.data[0] = instance->switches[switchNum].messageTriggerMask;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &response);
}

static void setSwitchTriggering(diypinball_switchMatrixInstance_t *instance, diypinball_pinballMessage_t *message) {
    uint8_t switchNum = message->featureNum;
    if(switchNum >= instance->numSwitches) {
        return;
    }

    if(message->dataLength < 1) {
        return;
    }

    instance->switches[switchNum].messageTriggerMask = message->data[0] & 0x03;
}

static void sendSwitchDebounce(diypinball_switchMatrixInstance_t *instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    uint8_t switchNum = message->featureNum;
    if(switchNum >= instance->numSwitches) {
        return;
    }

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x01;
    response.featureNum = switchNum;
    response.function = 0x03;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.dataLength = 1;
    response.data[0] = instance->switches[switchNum].debounceLimit;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &response);
}

static void setSwitchDebounce(diypinball_switchMatrixInstance_t *instance, diypinball_pinballMessage_t *message) {
    uint8_t switchNum = message->featureNum;
    if(switchNum >= instance->numSwitches) {
        return;
    }

    if(message->dataLength < 1) {
        return;
    }

    instance->switches[switchNum].debounceLimit = message->data[0];
    (instance->debounceChangedHandler)(switchNum, message->data[0]);
}

void diypinball_switchMatrix_init(diypinball_switchMatrixInstance_t *instance, diypinball_switchMatrixInit_t *init) {
    instance->numSwitches = init->numSwitches;
    if(instance->numSwitches > 16) instance->numSwitches = 16;

    instance->readStateHandler = init->readStateHandler;
    instance->debounceChangedHandler = init->debounceChangedHandler;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->switches[i].lastState = 0;
        instance->switches[i].messageTriggerMask = 0;
        instance->switches[i].pollingInterval = 0;
        instance->switches[i].lastTick = 0;
        instance->switches[i].debounceLimit = 0;
        instance->switches[i].ruleMask = 0;
        instance->switches[i].closeRule.boardAddress = 0;
        instance->switches[i].closeRule.solenoidNum = 0;
        instance->switches[i].closeRule.attackStatus = 0;
        instance->switches[i].closeRule.attackDuration = 0;
        instance->switches[i].closeRule.sustainStatus = 0;
        instance->switches[i].closeRule.sustainDuration = 0;
        instance->switches[i].openRule.boardAddress = 0;
        instance->switches[i].openRule.solenoidNum = 0;
        instance->switches[i].openRule.attackStatus = 0;
        instance->switches[i].openRule.attackDuration = 0;
        instance->switches[i].openRule.sustainStatus = 0;
        instance->switches[i].openRule.sustainDuration = 0;
    }

    instance->featureDecoderInstance.concreteFeatureDecoderInstance = (void*) instance;
    instance->featureDecoderInstance.featureType = 1; // FIXME constant
    instance->featureDecoderInstance.messageHandler = diypinball_switchMatrix_messageReceivedHandler;
    instance->featureDecoderInstance.tickHandler = diypinball_switchMatrix_millisecondTickHandler;
    instance->featureDecoderInstance.routerInstance = init->routerInstance;
    diypinball_featureRouter_addFeature(init->routerInstance, &(instance->featureDecoderInstance));
}

void diypinball_switchMatrix_millisecondTickHandler(void *instance, uint32_t tickNum) {
    diypinball_switchMatrixInstance_t* typedInstance = (diypinball_switchMatrixInstance_t *) instance;

    uint8_t i;
    uint8_t newState;

    for(i=0; i<typedInstance->numSwitches; i++) {
        if(typedInstance->switches[i].pollingInterval) {
            if(tickNum - typedInstance->switches[i].lastTick >= typedInstance->switches[i].pollingInterval) {
                typedInstance->switches[i].lastTick = tickNum;
                // send switch update
                (typedInstance->readStateHandler)(&newState, i);

                sendSwitchUpdate(typedInstance, i, newState, 0x01); // FIXME constant priority
                if(newState != typedInstance->switches[i].lastState) {
                    typedInstance->switches[i].lastState = newState;
                }
            }
        }
    }
}

void diypinball_switchMatrix_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message) {
	diypinball_switchMatrixInstance_t* typedInstance = (diypinball_switchMatrixInstance_t *) instance;

    switch(message->function) {
    case 0x00: // Switch status - requestable only
        if(message->messageType == MESSAGE_REQUEST) sendSwitchStatus(typedInstance, message);
        break;
    case 0x01: // Switch polling interval - set or requestable
        if(message->messageType == MESSAGE_REQUEST) {
            sendSwitchPolling(typedInstance, message);
        } else {
            setSwitchPolling(typedInstance, message);
        }
        break;
    case 0x02: // Switch triggering - set or requestable
        if(message->messageType == MESSAGE_REQUEST) {
            sendSwitchTriggering(typedInstance, message);
        } else {
            setSwitchTriggering(typedInstance, message);
        }
        break;
    case 0x03: // Switch debouncing - set or requestable
        if(message->messageType == MESSAGE_REQUEST) {
            sendSwitchDebounce(instance, message);
        } else {
            setSwitchDebounce(instance, message);
        }
        break;
    }
}

void diypinball_switchMatrix_deinit(diypinball_switchMatrixInstance_t *instance) {
    instance->featureDecoderInstance.concreteFeatureDecoderInstance = NULL;
    instance->featureDecoderInstance.featureType = 0;
    instance->featureDecoderInstance.messageHandler = NULL;
    instance->featureDecoderInstance.tickHandler = NULL;
    instance->featureDecoderInstance.routerInstance = NULL;

    instance->numSwitches = 0;
    instance->readStateHandler = NULL;
    instance->debounceChangedHandler = NULL;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->switches[i].lastState = 0;
        instance->switches[i].messageTriggerMask = 0;
        instance->switches[i].pollingInterval = 0;
        instance->switches[i].lastTick = 0;
        instance->switches[i].debounceLimit = 0;
        instance->switches[i].ruleMask = 0;
        instance->switches[i].closeRule.boardAddress = 0;
        instance->switches[i].closeRule.solenoidNum = 0;
        instance->switches[i].closeRule.attackStatus = 0;
        instance->switches[i].closeRule.attackDuration = 0;
        instance->switches[i].closeRule.sustainStatus = 0;
        instance->switches[i].closeRule.sustainDuration = 0;
        instance->switches[i].openRule.boardAddress = 0;
        instance->switches[i].openRule.solenoidNum = 0;
        instance->switches[i].openRule.attackStatus = 0;
        instance->switches[i].openRule.attackDuration = 0;
        instance->switches[i].openRule.sustainStatus = 0;
        instance->switches[i].openRule.sustainDuration = 0;
    }
}

void diypinball_switchMatrix_registerSwitchState(diypinball_switchMatrixInstance_t *instance, uint8_t switchNum, uint8_t state) {
    if(switchNum < instance->numSwitches) {
        if(state && !(instance->switches[switchNum].lastState) && (instance->switches[switchNum].messageTriggerMask & 0x01)) {
            sendSwitchUpdate(instance, switchNum, state, 0x01);
        } else if(!state && (instance->switches[switchNum].lastState) && (instance->switches[switchNum].messageTriggerMask & 0x02)) {
            sendSwitchUpdate(instance, switchNum, state, 0x01);
        }
        instance->switches[switchNum].lastState = state;
    }
}