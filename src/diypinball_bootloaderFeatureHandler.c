#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_bootloaderFeatureHandler.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

static void sendApplicationInfo(diypinball_bootloaderFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x07;
    response.featureNum = 0x00;
    response.function = 0x00;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.data[0] = instance->applicationVersionMajor;
    response.data[1] = instance->applicationVersionMinor;
    response.data[2] = instance->applicationVersionPatch;

    response.dataLength = 3;

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void sendFlashPageSize(diypinball_bootloaderFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x07;
    response.featureNum = 0x01;
    response.function = 0x00;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    memcpy(response.data, &(instance->flashPageSize), 4);

    response.dataLength = 4;

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

void diypinball_bootloaderFeatureHandler_init(diypinball_bootloaderFeatureHandlerInstance_t *instance, diypinball_bootloaderFeatureHandlerInit_t *init) {
    instance->applicationVersionMajor = init->applicationVersionMajor;
    instance->applicationVersionMinor = init->applicationVersionMinor;
    instance->applicationVersionPatch = init->applicationVersionPatch;
    instance->flashPageSize = init->flashPageSize;
    instance->flashBufferSize = init->flashBufferSize;
    instance->applicationBaseAddress = init->applicationBaseAddress;
    instance->flashSize = init->flashSize;
    instance->rebootHandler = init->rebootHandler;
    instance->flashReadHandler = init->flashReadHandler;
    instance->flashWriteHandler = init->flashWriteHandler;
    instance->flashVerifyHandler = init->flashVerifyHandler;
    instance->bufferHashHandler = init->bufferHashHandler;
    instance->bufferReadHandler = init->bufferReadHandler;
    instance->bufferWriteHandler = init->bufferWriteHandler;

    instance->featureHandlerInstance.concreteFeatureHandlerInstance = (void*) instance;
    instance->featureHandlerInstance.featureType = 7; // FIXME constant
    instance->featureHandlerInstance.messageHandler = diypinball_bootloaderFeatureHandler_messageReceivedHandler;
    instance->featureHandlerInstance.tickHandler = diypinball_bootloaderFeatureHandler_millisecondTickHandler;
    instance->featureHandlerInstance.routerInstance = init->routerInstance;
    diypinball_featureRouter_addFeature(init->routerInstance, &(instance->featureHandlerInstance));
}

void diypinball_bootloaderFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum) {

}

void diypinball_bootloaderFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message) {
    diypinball_bootloaderFeatureHandlerInstance_t* typedInstance = (diypinball_bootloaderFeatureHandlerInstance_t *) instance;

    switch(message->function) {
    case 0x00: // Bootloader info - requestable only
        switch(message->featureNum) {
        case 0x00:
            if(message->messageType == MESSAGE_REQUEST) sendApplicationInfo(typedInstance, message);
            break;
        case 0x01:
            if(message->messageType == MESSAGE_REQUEST) sendFlashPageSize(typedInstance, message);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void diypinball_bootloaderFeatureHandler_deinit(diypinball_bootloaderFeatureHandlerInstance_t *instance) {
    instance->featureHandlerInstance.concreteFeatureHandlerInstance = NULL;
    instance->featureHandlerInstance.featureType = 0;
    instance->featureHandlerInstance.messageHandler = NULL;
    instance->featureHandlerInstance.tickHandler = NULL;
    instance->featureHandlerInstance.routerInstance = NULL;

    instance->applicationVersionMajor = 0;
    instance->applicationVersionMinor = 0;
    instance->applicationVersionPatch = 0;
    instance->flashPageSize = 0;
    instance->flashBufferSize = 0;
    instance->applicationBaseAddress = 0;
    instance->flashSize = 0;
    instance->rebootHandler = NULL;
    instance->flashReadHandler = NULL;
    instance->flashWriteHandler = NULL;
    instance->flashVerifyHandler = NULL;
    instance->bufferHashHandler = NULL;
    instance->bufferReadHandler = NULL;
    instance->bufferWriteHandler = NULL;
}
