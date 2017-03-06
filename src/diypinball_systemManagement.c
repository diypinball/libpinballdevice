#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_systemManagement.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

static void sendBoardID1(diypinball_systemManagementInstance_t* instance, uint8_t priority) {
    diypinball_pinballMessage_t response;

    response.priority = priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x00;
    response.featureNum = 0x00;
    response.function = 0x00;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;
    response.data[0] = 0;
    response.data[1] = 0;
    response.data[2] = 0;
    response.data[3] = 0;
    response.data[4] = 0;
    response.data[5] = 0;
    response.data[6] = 0;
    response.data[7] = 0;

    uint16_t bitmap = 0;

    diypinball_featureRouter_getFeatureBitmap(instance->featureDecoderInstance.routerInstance, &bitmap);

    uint8_t lastNibble = 0;
    uint8_t i;

    for(i = 0; i < 16; i++) {
        if(bitmap & (1 << i)) {
            if(lastNibble & 0x01) {
                response.data[lastNibble / 2] |= i << 4;
            } else {
                response.data[lastNibble / 2] |= i;
            }
            lastNibble++;
        }
    }

    response.dataLength = (lastNibble + 1) / 2;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &response);
}

static void sendBoardID2(diypinball_systemManagementInstance_t* instance, uint8_t priority) {
    diypinball_pinballMessage_t response;

    response.priority = priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x00;
    response.featureNum = 0x00;
    response.function = 0x01;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.data[0] = instance->firmwareVersionMajor;
    response.data[1] = instance->firmwareVersionMinor;
    response.data[2] = instance->firmwareVersionPatch;

    response.dataLength = 3;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &response);
}

static void sendPowerStatus(diypinball_systemManagementInstance_t* instance, uint8_t priority) {
    uint8_t voltages[4], currents[4];
    diypinball_pinballMessage_t response;

    (instance->powerStatusHandler)(voltages, currents);

    response.priority = priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x00;
    response.featureNum = 0x00;
    response.function = 0x02;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.data[0] = voltages[0];
    response.data[1] = voltages[1];
    response.data[2] = voltages[2];
    response.data[3] = voltages[3];
    response.data[4] = currents[0];
    response.data[5] = currents[1];
    response.data[6] = currents[2];
    response.data[7] = currents[3];

    response.dataLength = 8;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &response);
}


static void sendPowerStatusPolling(diypinball_systemManagementInstance_t* instance, uint8_t priority) {
    diypinball_pinballMessage_t response;

    response.priority = priority;
    response.unitSpecific = 0x01;
    response.featureType = 0x00;
    response.featureNum = 0x00;
    response.function = 0x03;
    response.reserved = 0x00;
    response.messageType = MESSAGE_RESPONSE;

    response.data[0] = instance->powerStatusPollingInterval;

    response.dataLength = 1;

    diypinball_featureRouter_sendPinballMessage(instance->featureDecoderInstance.routerInstance, &response);
}

static void setPowerStatusPolling(diypinball_systemManagementInstance_t* instance, diypinball_pinballMessage_t *message) {
    if(message->dataLength >= 1) {
        instance->powerStatusPollingInterval = message->data[0];
    }
}

void diypinball_systemManagement_init(diypinball_systemManagementInstance_t *instance, diypinball_systemManagementInit_t *init) {
    instance->firmwareVersionMajor = init->firmwareVersionMajor;
    instance->firmwareVersionMinor = init->firmwareVersionMinor;
    instance->firmwareVersionPatch = init->firmwareVersionPatch;
    instance->powerStatusPollingInterval = 0;
    instance->lastTick = 0;
    memcpy(instance->boardSerial, init->boardSerial, 4 * sizeof(instance->boardSerial[0]));
    memcpy(instance->boardSignature, init->boardSignature, 2 * sizeof(instance->boardSignature[0]));
    instance->powerStatusHandler = init->powerStatusHandler;

    instance->featureDecoderInstance.concreteFeatureDecoderInstance = (void*) instance;
    instance->featureDecoderInstance.featureType = 0;
    instance->featureDecoderInstance.messageHandler = diypinball_systemManagement_messageReceivedHandler;
    instance->featureDecoderInstance.tickHandler = diypinball_systemManagement_millisecondTickHandler;
    instance->featureDecoderInstance.routerInstance = init->routerInstance;
    diypinball_featureRouter_addFeature(init->routerInstance, &(instance->featureDecoderInstance));
}

void diypinball_systemManagement_millisecondTickHandler(void *instance, uint32_t tickNum) {
    diypinball_systemManagementInstance_t* typedInstance = (diypinball_systemManagementInstance_t *) instance;
    
    if(typedInstance->powerStatusPollingInterval) {
        if((tickNum - typedInstance->lastTick) >= typedInstance->powerStatusPollingInterval) {
            typedInstance->lastTick = tickNum;
            sendPowerStatus(typedInstance, 0x0E);
        }
    }
}

void diypinball_systemManagement_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message) {
    diypinball_systemManagementInstance_t* typedInstance = (diypinball_systemManagementInstance_t *) instance;

    switch(message->function) {
    case 0x00: // Board ID 1 - requestable only
        if(message->messageType == MESSAGE_REQUEST) sendBoardID1(typedInstance, message->priority);
        break;
    case 0x01: // Board ID 2 - requestable only
        if(message->messageType == MESSAGE_REQUEST) sendBoardID2(typedInstance, message->priority);
        break;
    case 0x02: // Power status - requestable only
        if(message->messageType == MESSAGE_REQUEST) sendPowerStatus(typedInstance, message->priority);
        break;
    case 0x03: // Power Status Polling - set or request
        if(message->messageType == MESSAGE_REQUEST) {
            sendPowerStatusPolling(typedInstance, message->priority);
        } else {
            setPowerStatusPolling(typedInstance, message);
        }
        break;
    default:
        break;
    }
}

void diypinball_systemManagement_sendPoweronMessages(diypinball_systemManagementInstance_t *instance) {
    sendBoardID1(instance, 0x0F);
    sendBoardID2(instance, 0x0F);
}
