#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_rgbFeatureHandler.h"

static void sendRGBStatus(diypinball_rgbFeatureHandlerInstance_t *instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    uint8_t rgbNum = message->featureNum;
    if(rgbNum >= instance->numRGBs) {
        return;
    }

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x05;
    response.featureNum = rgbNum;
    response.function = 0x00;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.dataLength = 3;

    response.data[0] = instance->rgbs[rgbNum].red;
    response.data[1] = instance->rgbs[rgbNum].green;
    response.data[2] = instance->rgbs[rgbNum].blue;

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void setRGBStatus(diypinball_rgbFeatureHandlerInstance_t *instance, diypinball_pinballMessage_t *message) {
    uint8_t rgbNum = message->featureNum;
    if(rgbNum >= instance->numRGBs) {
        return;
    }

    if(message->dataLength < 3) {
        return;
    }

    instance->rgbs[rgbNum].red = message->data[0];
    instance->rgbs[rgbNum].green = message->data[1];
    instance->rgbs[rgbNum].blue = message->data[2];

    (instance->rgbChangedHandler)(rgbNum, instance->rgbs[rgbNum]);
}

static void setAllRGBs(diypinball_rgbFeatureHandlerInstance_t *instance, diypinball_pinballMessage_t *message) {
    uint8_t rgbBase;
    uint8_t rgbMax;

    if(message->featureNum & 0x01) {
        rgbBase = 8;
        rgbMax = 15;
    } else {
        rgbBase = 0;
        rgbMax = 7;
    }

    if(rgbBase > instance->numRGBs) {
        return;
    }
    if(rgbMax > instance->numRGBs) {
        rgbMax = instance->numRGBs - 1;
    }

    if(message->dataLength < (rgbMax - rgbBase) + 1) {
        return;
    }

    uint8_t i;
    for(i=rgbBase; i <= rgbMax; i++) {
        if((message->featureNum & 0x0E) == 0x00) {
            instance->rgbs[i].red = message->data[i - rgbBase];
        } else if((message->featureNum & 0x0E) == 0x02) {
            instance->rgbs[i].green = message->data[i - rgbBase];
        } else if((message->featureNum & 0x0E) == 0x04) {
            instance->rgbs[i].blue = message->data[i - rgbBase];
        } else {
            break; // invalid colour/set to address
        }

        (instance->rgbChangedHandler)(i, instance->rgbs[i]);
    }
}

void diypinball_rgbFeatureHandler_init(diypinball_rgbFeatureHandlerInstance_t *instance, diypinball_rgbFeatureHandlerInit_t *init) {
    instance->numRGBs = init->numRGBs;
	if(instance->numRGBs > 16) instance->numRGBs = 16;
    instance->rgbChangedHandler = init->rgbChangedHandler;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->rgbs[i].red = 0;
        instance->rgbs[i].green = 0;
        instance->rgbs[i].blue = 0;
    }

    instance->featureHandlerInstance.concreteFeatureHandlerInstance = (void*) instance;
    instance->featureHandlerInstance.featureType = 5; // FIXME constant
    instance->featureHandlerInstance.messageHandler = diypinball_rgbFeatureHandler_messageReceivedHandler;
    instance->featureHandlerInstance.tickHandler = diypinball_rgbFeatureHandler_millisecondTickHandler;
    instance->featureHandlerInstance.routerInstance = init->routerInstance;
    diypinball_featureRouter_addFeature(init->routerInstance, &(instance->featureHandlerInstance));
}

void diypinball_rgbFeatureHandler_millisecondTickHandler(void *instance, uint32_t tickNum) {
    
}

void diypinball_rgbFeatureHandler_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message) {
    diypinball_rgbFeatureHandlerInstance_t* typedInstance = (diypinball_rgbFeatureHandlerInstance_t *) instance;

    switch(message->function) {
    case 0x00: // RGB status - set or request
        if(message->messageType == MESSAGE_REQUEST) {
            sendRGBStatus(typedInstance, message);
        } else {
            setRGBStatus(typedInstance, message);
        }
        break;
    case 0x01: // All rgbs - set only
        if(message->messageType == MESSAGE_COMMAND) {
            setAllRGBs(typedInstance, message);
        }
    default:
        break;
    }
}

void diypinball_rgbFeatureHandler_deinit(diypinball_rgbFeatureHandlerInstance_t *instance) {
    instance->featureHandlerInstance.concreteFeatureHandlerInstance = NULL;
    instance->featureHandlerInstance.featureType = 0;
    instance->featureHandlerInstance.messageHandler = NULL;
    instance->featureHandlerInstance.tickHandler = NULL;
    instance->featureHandlerInstance.routerInstance = NULL;

    instance->numRGBs = 0;
    instance->rgbChangedHandler = NULL;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->rgbs[i].red = 0;
        instance->rgbs[i].green = 0;
        instance->rgbs[i].blue = 0;
    }
}
