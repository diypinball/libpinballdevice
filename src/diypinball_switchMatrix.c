#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_switchMatrix.h"

static void fireRule(diypinball_switchMatrixInstance_t *instance, uint8_t switchNum, uint8_t rule) {
    diypinball_pinballMessage_t command;
    diypinball_switchMatrixRule_t activeRule;
    uint8_t ruleMask;

    if(switchNum >= instance->numSwitches) {
        return;
    }

    activeRule = rule ? instance->switches[switchNum].closeRule : instance->switches[switchNum].openRule;
    ruleMask = rule ? 0x02 : 0x01;

    if(!(instance->switches[switchNum].ruleMask & ruleMask)) {
        return;
    }

    command.priority = 0x01;
    command.boardAddress = activeRule.boardAddress;
    command.unitSpecific = 0x01;
    command.featureType = 0x03;
    command.featureNum = activeRule.solenoidNum;
    command.function = 0x00;
    command.reserved = 0x00;
    command.messageType = MESSAGE_COMMAND;

    command.dataLength = 4;
    command.data[0] = activeRule.attackStatus;
    command.data[1] = activeRule.attackDuration;
    command.data[2] = activeRule.sustainStatus;
    command.data[3] = activeRule.sustainDuration;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &command);
}

static void fireDeactivationRule(diypinball_switchMatrixInstance_t *instance, uint8_t switchNum, uint8_t rule) {
    diypinball_pinballMessage_t command;
    diypinball_switchMatrixRule_t activeRule;
    uint8_t ruleMask;

    if(switchNum > instance->numSwitches) {
        return;
    }

    activeRule = rule ? instance->switches[switchNum].closeRule : instance->switches[switchNum].openRule;
    ruleMask = rule ? 0x02 : 0x01;

    if(!(instance->switches[switchNum].ruleMask & ruleMask)) {
        return;
    }

    command.priority = 0x01;
    command.boardAddress = activeRule.boardAddress;
    command.unitSpecific = 0x01;
    command.featureType = 0x03;
    command.featureNum = activeRule.solenoidNum;
    command.function = 0x00;
    command.reserved = 0x00;
    command.messageType = MESSAGE_COMMAND;

    command.dataLength = 1;
    command.data[0] = 0x00;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &command);
}

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
    if(response.data[1] == 1) {
        fireRule(instance, switchNum, 1);
    } else if(response.data[1] == 2) {
        fireRule(instance, switchNum, 0);
    }
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

static void sendSwitchRule(diypinball_switchMatrixInstance_t *instance, diypinball_pinballMessage_t *message, uint8_t rule) {
    diypinball_pinballMessage_t response;
    diypinball_switchMatrixRule_t *activeRule;
    uint8_t ruleMask;

    uint8_t switchNum = message->featureNum;
    if(switchNum >= instance->numSwitches) {
        return;
    }

    activeRule = rule ? &(instance->switches[switchNum].closeRule) : &(instance->switches[switchNum].openRule);
    ruleMask = rule ? 0x02 : 0x01;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x01;
    response.featureNum = switchNum;
    response.function = rule ? 0x05 : 0x04;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.dataLength = 7;
    response.data[0] = instance->switches[switchNum].ruleMask & ruleMask ? 1 : 0;
    response.data[1] = activeRule->boardAddress;
    response.data[2] = activeRule->solenoidNum;
    response.data[3] = activeRule->attackStatus;
    response.data[4] = activeRule->attackDuration;
    response.data[5] = activeRule->sustainStatus;
    response.data[6] = activeRule->sustainDuration;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &response);
}

static void setSwitchRule(diypinball_switchMatrixInstance_t *instance, diypinball_pinballMessage_t *message, uint8_t rule) {
    diypinball_switchMatrixRule_t *activeRule;
    uint8_t ruleMask;
    uint8_t switchNum = message->featureNum;

    if(switchNum >= instance->numSwitches) {
        return;
    }

    if(message->dataLength == 0) {
        return;
    }

    if((message->data[0]) && (message->dataLength < 5)) {
        // not enough data for rule
        return;
    }

    activeRule = rule ? &(instance->switches[switchNum].closeRule) : &(instance->switches[switchNum].openRule);
    ruleMask = rule ? 0x02 : 0x01;

    // if an existing rule is there (mask is set), send a deactivation message to that solenoid
    if(instance->switches[switchNum].ruleMask & ruleMask) {
        fireDeactivationRule(instance, switchNum, rule);
    }

    instance->switches[switchNum].ruleMask &= ~ruleMask;
    instance->switches[switchNum].ruleMask |= message->data[0] ? ruleMask : 0;
    activeRule->boardAddress = message->data[1];
    activeRule->solenoidNum = message->data[2];
    activeRule->attackStatus = message->data[3];
    activeRule->attackDuration = message->data[4];

    if(message->dataLength >= 7) {
        activeRule->sustainStatus = message->data[5];
        activeRule->sustainDuration = message->data[6];
    } else {
        activeRule->sustainStatus = 0;
        activeRule->sustainDuration = 0;
    }
}

static void sendAllSwitchStatus(diypinball_switchMatrixInstance_t *instance, diypinball_pinballMessage_t *message) {
    uint8_t newState;

    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x01;
    response.featureNum = 0;
    response.function = 0x06;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.data[0] = 0;
    response.data[1] = 0;

    uint8_t i;

    for(i=0; i < instance->numSwitches; i++) {
        (instance->readStateHandler)(&newState, i);

        if(newState != instance->switches[i].lastState) {
            instance->switches[i].lastState = newState; // also fire rules?
        }

        if(newState) {
            if(i < 8) {
                response.data[0] |= (1 << i);
            } else {
                response.data[1] |= (1 << (i - 8));
            }
        }
    }   

    response.dataLength = 2;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &response);
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
            sendSwitchDebounce(typedInstance, message);
        } else {
            setSwitchDebounce(typedInstance, message);
        }
        break;
    case 0x04: // Switch rule - open event params
        if(message->messageType == MESSAGE_REQUEST) {
            sendSwitchRule(typedInstance, message, 0);
        } else {
            setSwitchRule(typedInstance, message, 0);
        }
        break;
    case 0x05: // Switch rule - close event params
        if(message->messageType == MESSAGE_REQUEST) {
            sendSwitchRule(typedInstance, message, 1);
        } else {
            setSwitchRule(typedInstance, message, 1);
        }
        break;
    case 0x06: // Switch status - all
        if(message->messageType == MESSAGE_REQUEST) {
            sendAllSwitchStatus(typedInstance, message);
        }
        break;
    default:
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
        if(state && !(instance->switches[switchNum].lastState)) {
            if (instance->switches[switchNum].messageTriggerMask & 0x01) {
                sendSwitchUpdate(instance, switchNum, state, 0x01);
            } else {
                fireRule(instance, switchNum, 1);
            }
        } else if(!state && (instance->switches[switchNum].lastState)) {
            if (instance->switches[switchNum].messageTriggerMask & 0x02) {
                sendSwitchUpdate(instance, switchNum, state, 0x01);
            } else {
                fireRule(instance, switchNum, 0);
            }
        }
        instance->switches[switchNum].lastState = state;
    }
}