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

static void sendFlashData(diypinball_bootloaderFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x07;
    response.featureNum = message->featureNum;
    response.function = 0x00;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    uint32_t dataBuffer;

    switch(message->featureNum) {
    case 0x01:
        dataBuffer = instance->flashPageSize;
        break;
    case 0x02:
        dataBuffer = instance->flashBufferSize;
        break;
    case 0x03:
        dataBuffer = instance->applicationBaseAddress;
        break;
    case 0x04:
        dataBuffer = instance->flashSize;
        break;
    default:
        return;
    }

    memcpy(response.data, &dataBuffer, 4);

    response.dataLength = 4;

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void runReadFlashToBuffer(diypinball_bootloaderFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x07;
    response.featureNum = message->featureNum;
    response.function = 0x02;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    uint8_t result;
    uint32_t param;

    if(message->dataLength < 4) {
        result = 255;
    } else {
        memcpy(&param, message->data, 4);

        result = (instance->flashReadHandler)(param);
    }

    response.data[0] = result;

    response.dataLength = 1;

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void runWriteBufferToFlash(diypinball_bootloaderFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x07;
    response.featureNum = message->featureNum;
    response.function = 0x02;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    uint8_t result;
    uint32_t param;

    if(message->dataLength < 4) {
        result = 255;
    } else {
        memcpy(&param, message->data, 4);

        result = (instance->flashWriteHandler)(param);
    }

    response.data[0] = result;

    response.dataLength = 1;

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void runVerifyBufferAgainstFlash(diypinball_bootloaderFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x07;
    response.featureNum = message->featureNum;
    response.function = 0x02;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    uint8_t result;
    uint32_t param;

    if(message->dataLength < 4) {
        result = 255;
    } else {
        memcpy(&param, message->data, 4);

        result = (instance->flashVerifyHandler)(param);
    }

    response.data[0] = result;

    response.dataLength = 1;

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void runCheckBufferCRC(diypinball_bootloaderFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x07;
    response.featureNum = message->featureNum;
    response.function = 0x02;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    uint8_t result;
    uint32_t param;

    if(message->dataLength < 4) {
        result = 255;
    } else {
        memcpy(&param, message->data, 4);

        result = (instance->bufferHashHandler)(param);
    }

    response.data[0] = result;

    response.dataLength = 1;

    diypinball_featureRouter_sendPinballMessage(instance->featureHandlerInstance.routerInstance, &response);
}

static void doApplicationReboot(diypinball_bootloaderFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    if(message->dataLength >= 1) {
        if(message->data[0] == 0x42) {
            (instance->rebootHandler)();
        }
    }
}

static void runWriteToBuffer(diypinball_bootloaderFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    uint8_t offset, i;

    offset = 0;
    offset |= message->featureNum & 0x0f;
    offset <<= 3;
    offset |= ((message->function & 0x0e) >> 1);

    if(message->dataLength < 8) { // we write 0xff to any byte not specified.
        for(i = message->dataLength; i < 8; i++) {
            message->data[i] = 0xff;
        }
    }

    (instance->bufferWriteHandler)(offset, message->data);
}

static void runReadFromBuffer(diypinball_bootloaderFeatureHandlerInstance_t* instance, diypinball_pinballMessage_t *message) {
    diypinball_pinballMessage_t response;

    response.priority = message->priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x07;
    response.featureNum = message->featureNum;
    response.function = message->function;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    uint8_t offset;

    offset = 0;
    offset |= message->featureNum & 0x0f;
    offset <<= 3;
    offset |= ((message->function & 0x0e) >> 1);

    (instance->bufferReadHandler)(offset, response.data);

    response.dataLength = 8;

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
        case 0x02:
        case 0x03:
        case 0x04:
            if(message->messageType == MESSAGE_REQUEST) sendFlashData(typedInstance, message);
            break;
        default:
            break;
        }
        break;
    case 0x02:
        switch(message->featureNum) {
        case 0x00:
            if(message->messageType == MESSAGE_COMMAND) runReadFlashToBuffer(typedInstance, message);
            break;
        case 0x01:
            if(message->messageType == MESSAGE_COMMAND) runWriteBufferToFlash(typedInstance, message);
            break;
        case 0x02:
            if(message->messageType == MESSAGE_COMMAND) runVerifyBufferAgainstFlash(typedInstance, message);
            break;
        case 0x03:
            if(message->messageType == MESSAGE_COMMAND) runCheckBufferCRC(typedInstance, message);
            break;
        default:
            break;
        }
        break;
    case 0x04:
        if(message->messageType == MESSAGE_COMMAND) doApplicationReboot(typedInstance, message);
        break;
    default:
        if(message->function & 0x01) {
            // these are buffer operations
            if(message->messageType == MESSAGE_REQUEST) {
                runReadFromBuffer(typedInstance, message);
            } else if(message->messageType == MESSAGE_COMMAND) {
                runWriteToBuffer(typedInstance, message);
            }
        }
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
