#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_scoreFeatureHandler.h"

#include <string.h>

static void sendDisplay(diypinball_scoreFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t* message) {
    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x04;
    response.featureNum = message->featureNum;
    response.function = 0x00;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.dataLength = 8;

    memcpy(response.data, instance->display, 8);
    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void setDisplay(diypinball_scoreFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t* message) {
    memset(instance->display, 0x00, 8);
    memcpy(instance->display, message->data, message->dataLength);
    instance->displayChangedHandler(instance->display);
}

void diypinball_scoreFeatureHandler_init(diypinball_scoreFeatureHandlerInstance_t *instance, diypinball_scoreFeatureHandlerInit_t *init) {
    instance->brightness = 0;
    instance->displayChangedHandler = init->displayChangedHandler;
    instance->brightnessChangedHandler = init->brightnessChangedHandler;

    uint8_t i;
    for(i=0; i<8; i++) {
        instance->display[i] = 0x00;
    }

    instance->featureHandlerInstance.concreteFeatureHandlerInstance = (void*) instance;
    instance->featureHandlerInstance.featureType = 4; // FIXME constant
    instance->featureHandlerInstance.messageHandler = diypinball_scoreFeatureHandler_messageReceivedHandler;
    instance->featureHandlerInstance.tickHandler = diypinball_scoreFeatureHandler_millisecondTickHandler;
    instance->featureHandlerInstance.routerInstance = init->routerInstance;
    diypinball_featureRouter_addFeature(init->routerInstance, &(instance->featureHandlerInstance));
}

void diypinball_scoreFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum) {
    
}

void diypinball_scoreFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message) {
    diypinball_scoreFeatureHandlerInstance_t* typedInstance = (diypinball_scoreFeatureHandlerInstance_t *) instance;

    switch(message->function) {
    case 0x00: // display contents - set or request
        if(message->messageType == MESSAGE_REQUEST) {
            sendDisplay(typedInstance, message);
        } else {
            setDisplay(typedInstance, message);
        }
        break;
    default:
        break;
    }
}

void diypinball_scoreFeatureHandler_deinit(diypinball_scoreFeatureHandlerInstance_t *instance) {
    instance->featureHandlerInstance.concreteFeatureHandlerInstance = NULL;
    instance->featureHandlerInstance.featureType = 0;
    instance->featureHandlerInstance.messageHandler = NULL;
    instance->featureHandlerInstance.tickHandler = NULL;
    instance->featureHandlerInstance.routerInstance = NULL;

    instance->brightness = 0;
    instance->displayChangedHandler = NULL;
    instance->brightnessChangedHandler = NULL;

    uint8_t i;
    for(i=0; i<8; i++) {
        instance->display[i] = 0x00;
    }
}
