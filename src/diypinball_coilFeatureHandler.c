#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_coilFeatureHandler.h"

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

}
