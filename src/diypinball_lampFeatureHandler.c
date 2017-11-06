#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_lampFeatureHandler.h"

static void sendLampStatus(diypinball_lampFeatureHandlerInstance_t *instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    uint8_t lampNum = message->featureNum;
    if(lampNum >= instance->numLamps) {
        return;
    }

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x02;
    response.featureNum = lampNum;
    response.function = 0x00;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    switch(instance->lamps[lampNum].numStates) {
        default:
        case 0:
            response.dataLength = 2;
            response.data[0] = 0;
            response.data[1] = 0;
            break;
        case 1:
            response.dataLength = 2;
            response.data[0] = instance->lamps[lampNum].state1;
            response.data[1] = instance->lamps[lampNum].state1Duration;
            break;
        case 2:
            response.dataLength = 4;
            response.data[0] = instance->lamps[lampNum].state1;
            response.data[1] = instance->lamps[lampNum].state1Duration;
            response.data[2] = instance->lamps[lampNum].state2;
            response.data[3] = instance->lamps[lampNum].state2Duration;
            break;
        case 3:
            response.dataLength = 6;
            response.data[0] = instance->lamps[lampNum].state1;
            response.data[1] = instance->lamps[lampNum].state1Duration;
            response.data[2] = instance->lamps[lampNum].state2;
            response.data[3] = instance->lamps[lampNum].state2Duration;
            response.data[4] = instance->lamps[lampNum].state3;
            response.data[5] = instance->lamps[lampNum].state3Duration;
            break;
    }

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void setLampStatus(diypinball_lampFeatureHandlerInstance_t *instance, diypinball_pinballMessage_t *message) {
    uint8_t lampNum = message->featureNum;
    if(lampNum >= instance->numLamps) {
        return;
    }

    switch(message->dataLength) {
        case 8:
        case 7:
        case 6:
            instance->lamps[lampNum].numStates = 3;
            instance->lamps[lampNum].state1 = message->data[0];
            instance->lamps[lampNum].state1Duration = message->data[1];
            instance->lamps[lampNum].state2 = message->data[2];
            instance->lamps[lampNum].state2Duration = message->data[3];
            instance->lamps[lampNum].state3 = message->data[4];
            instance->lamps[lampNum].state3Duration = message->data[5];
            break;
        case 5:
        case 4:
            instance->lamps[lampNum].numStates = 2;
            instance->lamps[lampNum].state1 = message->data[0];
            instance->lamps[lampNum].state1Duration = message->data[1];
            instance->lamps[lampNum].state2 = message->data[2];
            instance->lamps[lampNum].state2Duration = message->data[3];
            instance->lamps[lampNum].state3 = 0;
            instance->lamps[lampNum].state3Duration = 0;
            break;
        case 3:
        case 2:
        case 1:
            instance->lamps[lampNum].numStates = 1;
            instance->lamps[lampNum].state1 = message->data[0];
            instance->lamps[lampNum].state1Duration = 0;
            instance->lamps[lampNum].state2 = 0;
            instance->lamps[lampNum].state2Duration = 0;
            instance->lamps[lampNum].state3 = 0;
            instance->lamps[lampNum].state3Duration = 0;
            break;
        default:
            return;
            break;
    }
    (instance->lampChangedHandler)(lampNum, instance->lamps[lampNum]);
}

static void setAllLamps(diypinball_lampFeatureHandlerInstance_t *instance, diypinball_pinballMessage_t *message) {
    uint8_t lampBase;
    uint8_t lampMax;

    if(message->featureNum == 1) {
        lampBase = 8;
        lampMax = 15;
    } else if(message->featureNum == 0) {
        lampBase = 0;
        lampMax = 7;
    } else {
        return;
    }

    if(lampBase > instance->numLamps) {
        return;
    }
    if(lampMax > instance->numLamps) {
        lampMax = instance->numLamps - 1;
    }

    if(message->dataLength < (lampMax - lampBase) + 1) {
        return;
    }

    uint8_t i;
    for(i=lampBase; i <= lampMax; i++) {
        instance->lamps[i].numStates = 1;
        instance->lamps[i].state1 = message->data[i - lampBase];
        instance->lamps[i].state1Duration = 0;
        instance->lamps[i].state2 = 0;
        instance->lamps[i].state2Duration = 0;
        instance->lamps[i].state3 = 0;
        instance->lamps[i].state3Duration = 0;
        (instance->lampChangedHandler)(i, instance->lamps[i]);
    }
}

void diypinball_lampFeatureHandler_init(diypinball_lampFeatureHandlerInstance_t *instance, diypinball_lampFeatureHandlerInit_t *init) {
    instance->numLamps = init->numLamps;
	if(instance->numLamps > 16) instance->numLamps = 16;
    instance->lampChangedHandler = init->lampChangedHandler;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->lamps[i].state1 = 0;
        instance->lamps[i].state1Duration = 0;
        instance->lamps[i].state2 = 0;
        instance->lamps[i].state2Duration = 0;
        instance->lamps[i].state3 = 0;
        instance->lamps[i].state3Duration = 0;
        instance->lamps[i].numStates = 1;
    }

    instance->featureHandlerInstance.concreteFeatureHandlerInstance = (void*) instance;
    instance->featureHandlerInstance.featureType = 2; // FIXME constant
    instance->featureHandlerInstance.messageHandler = diypinball_lampFeatureHandler_messageReceivedHandler;
    instance->featureHandlerInstance.tickHandler = diypinball_lampFeatureHandler_millisecondTickHandler;
    instance->featureHandlerInstance.routerInstance = init->routerInstance;
    diypinball_featureRouter_addFeature(init->routerInstance, &(instance->featureHandlerInstance));
}

void diypinball_lampFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum) {
    
}

void diypinball_lampFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message) {
    diypinball_lampFeatureHandlerInstance_t* typedInstance = (diypinball_lampFeatureHandlerInstance_t *) instance;

    switch(message->function) {
    case 0x00: // Lamp status - set or request
        if(message->messageType == MESSAGE_REQUEST) {
            sendLampStatus(typedInstance, message);
        } else {
            setLampStatus(typedInstance, message);
        }
        break;
    case 0x01: // All lamps - set only
        if(message->messageType == MESSAGE_COMMAND) {
            setAllLamps(typedInstance, message);
        }
    default:
        break;
    }
}

void diypinball_lampFeatureHandler_deinit(diypinball_lampFeatureHandlerInstance_t *instance) {
    instance->featureHandlerInstance.concreteFeatureHandlerInstance = NULL;
    instance->featureHandlerInstance.featureType = 0;
    instance->featureHandlerInstance.messageHandler = NULL;
    instance->featureHandlerInstance.tickHandler = NULL;
    instance->featureHandlerInstance.routerInstance = NULL;

    instance->numLamps = 0;
    instance->lampChangedHandler = NULL;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->lamps[i].state1 = 0;
        instance->lamps[i].state1Duration = 0;
        instance->lamps[i].state2 = 0;
        instance->lamps[i].state2Duration = 0;
        instance->lamps[i].state3 = 0;
        instance->lamps[i].state3Duration = 0;
        instance->lamps[i].numStates = 0;
    }
}
