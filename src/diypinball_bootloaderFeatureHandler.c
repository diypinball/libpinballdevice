#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_bootloaderFeatureHandler.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

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

}
