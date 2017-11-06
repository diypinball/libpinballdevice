#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_coilFeatureHandler.h"

static void sendCoilStatus(diypinball_coilFeatureHandlerInstance_t *instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    uint8_t coilNum = message->featureNum;
    if(coilNum >= instance->numCoils) {
        return;
    }

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x03;
    response.featureNum = coilNum;
    response.function = 0x00;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.dataLength = 4;

    response.data[0] = instance->coils[coilNum].attackState;
    response.data[1] = instance->coils[coilNum].attackDuration;
    response.data[2] = instance->coils[coilNum].sustainState;
    response.data[3] = instance->coils[coilNum].sustainDuration;

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void setCoilStatus(diypinball_coilFeatureHandlerInstance_t *instance, diypinball_pinballMessage_t *message) {
    uint8_t coilNum = message->featureNum;
    if(coilNum >= instance->numCoils) {
        return;
    }

    if(message->dataLength >= 4) {
        instance->coils[coilNum].attackState = message->data[0];
        instance->coils[coilNum].attackDuration = message->data[1];
        instance->coils[coilNum].sustainState = message->data[2];
        instance->coils[coilNum].sustainDuration = message->data[3];
    } else if(message->dataLength >= 2) {
        instance->coils[coilNum].attackState = message->data[0];
        instance->coils[coilNum].attackDuration = message->data[1];
        instance->coils[coilNum].sustainState = 0;
        instance->coils[coilNum].sustainDuration = 0;
    } else {
        return;
    }

    (instance->coilChangedHandler)(coilNum, instance->coils[coilNum]);
}

void diypinball_coilFeatureHandler_init(diypinball_coilFeatureHandlerInstance_t *instance, diypinball_coilFeatureHandlerInit_t *init) {
    instance->numCoils = init->numCoils;
    if(instance->numCoils > 16) instance->numCoils = 16;
    instance->coilChangedHandler = init->coilChangedHandler;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->coils[i].attackState = 0;
        instance->coils[i].attackDuration = 0;
        instance->coils[i].sustainState = 0;
        instance->coils[i].sustainDuration = 0;
    }

    instance->featureHandlerInstance.concreteFeatureHandlerInstance = (void*) instance;
    instance->featureHandlerInstance.featureType = 3; // FIXME constant
    instance->featureHandlerInstance.messageHandler = diypinball_coilFeatureHandler_messageReceivedHandler;
    instance->featureHandlerInstance.tickHandler = diypinball_coilFeatureHandler_millisecondTickHandler;
    instance->featureHandlerInstance.routerInstance = init->routerInstance;
    diypinball_featureRouter_addFeature(init->routerInstance, &(instance->featureHandlerInstance));
}

void diypinball_coilFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum) {
    
}

void diypinball_coilFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message) {
    diypinball_coilFeatureHandlerInstance_t* typedInstance = (diypinball_coilFeatureHandlerInstance_t *) instance;

    switch(message->function) {
    case 0x00: // Coil status - set or request
        if(message->messageType == MESSAGE_REQUEST) {
            sendCoilStatus(typedInstance, message);
        } else {
            setCoilStatus(typedInstance, message);
        }
        break;
    default:
        break;
    }

}

void diypinball_coilFeatureHandler_deinit(diypinball_coilFeatureHandlerInstance_t *instance) {
    instance->featureHandlerInstance.concreteFeatureHandlerInstance = NULL;
    instance->featureHandlerInstance.featureType = 0;
    instance->featureHandlerInstance.messageHandler = NULL;
    instance->featureHandlerInstance.tickHandler = NULL;
    instance->featureHandlerInstance.routerInstance = NULL;

    instance->numCoils = 0;
    instance->coilChangedHandler = NULL;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->coils[i].attackState = 0;
        instance->coils[i].attackDuration = 0;
        instance->coils[i].sustainState = 0;
        instance->coils[i].sustainDuration = 0;
    }
}

void diypinball_coilFeatureHandler_registerCoilState(diypinball_coilFeatureHandlerInstance_t *instance, uint8_t coilNum, diypinball_coilStatus_t state) {
    if(coilNum < instance->numCoils) {
        instance->coils[coilNum].attackState = state.attackState;
        instance->coils[coilNum].attackDuration = state.attackDuration;
        instance->coils[coilNum].sustainState = state.sustainState;
        instance->coils[coilNum].sustainDuration = state.sustainDuration;
    }
}
