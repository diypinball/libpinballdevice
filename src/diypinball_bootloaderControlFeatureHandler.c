#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_bootloaderControlFeatureHandler.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

static void sendBootloaderInfo(diypinball_bootloaderControlFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x06;
    response.featureNum = 0x00;
    response.function = 0x00;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.data[0] = instance->bootloaderVersionMajor;
    response.data[1] = instance->bootloaderVersionMinor;
    response.data[2] = instance->bootloaderVersionPatch;

    response.dataLength = 3;

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void doBootloaderReboot(diypinball_bootloaderControlFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    if(message->dataLength >= 1) {
        if(message->data[0] == 0x42) {
            (instance->rebootHandler)();
        }
    }
}

void diypinball_bootloaderControlFeatureHandler_init(diypinball_bootloaderControlFeatureHandlerInstance_t *instance, diypinball_bootloaderControlFeatureHandlerInit_t *init) {
    instance->bootloaderVersionMajor = init->bootloaderVersionMajor;
    instance->bootloaderVersionMinor = init->bootloaderVersionMinor;
    instance->bootloaderVersionPatch = init->bootloaderVersionPatch;
    instance->rebootHandler = init->rebootHandler;

    instance->featureHandlerInstance.concreteFeatureHandlerInstance = (void*) instance;
    instance->featureHandlerInstance.featureType = 6; // FIXME constant
    instance->featureHandlerInstance.messageHandler = diypinball_bootloaderControlFeatureHandler_messageReceivedHandler;
    instance->featureHandlerInstance.tickHandler = diypinball_bootloaderControlFeatureHandler_millisecondTickHandler;
    instance->featureHandlerInstance.routerInstance = init->routerInstance;
    diypinball_featureRouter_addFeature(init->routerInstance, &(instance->featureHandlerInstance));
}

void diypinball_bootloaderControlFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum) {

}

void diypinball_bootloaderControlFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message) {
    diypinball_bootloaderControlFeatureHandlerInstance_t* typedInstance = (diypinball_bootloaderControlFeatureHandlerInstance_t *) instance;

    switch(message->function) {
    case 0x00: // Bootloader info - requestable only
        if(message->messageType == MESSAGE_REQUEST) sendBootloaderInfo(typedInstance, message);
        break;
    case 0x01: // Reboot to Bootloader - settable only
        if(message->messageType == MESSAGE_COMMAND) doBootloaderReboot(typedInstance, message);
        break;
    default:
        break;
    }
}

void diypinball_bootloaderControlFeatureHandler_deinit(diypinball_bootloaderControlFeatureHandlerInstance_t *instance) {
    instance->featureHandlerInstance.concreteFeatureHandlerInstance = NULL;
    instance->featureHandlerInstance.featureType = 0;
    instance->featureHandlerInstance.messageHandler = NULL;
    instance->featureHandlerInstance.tickHandler = NULL;
    instance->featureHandlerInstance.routerInstance = NULL;

    instance->bootloaderVersionMajor = 0;
    instance->bootloaderVersionMinor = 0;
    instance->bootloaderVersionPatch = 0;
    instance->rebootHandler = NULL;
}
