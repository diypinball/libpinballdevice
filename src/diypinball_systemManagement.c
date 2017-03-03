#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_systemManagement.h"

#include <stdint.h>
#include <string.h>

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
}

void diypinball_systemManagement_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message) {
}

void diypinball_systemManagement_sendPoweronMessages(diypinball_systemManagementInstance_t *instance) {
    sendBoardID1(instance, 0x0F);
    sendBoardID2(instance, 0x0F);
}
