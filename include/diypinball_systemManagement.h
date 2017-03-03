#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "diypinball.h"
#include "diypinball_featureRouter.h"

#include <stdint.h>

typedef void (*diypinball_systemManagementPowerStatusHandler)(uint8_t *voltages, uint8_t *currents);

typedef struct diypinball_systemManagementInstance {
    diypinball_featureDecoderInstance_t featureDecoderInstance;
    uint8_t firmwareVersionMajor;
    uint8_t firmwareVersionMinor;
    uint8_t firmwareVersionPatch;
    uint8_t powerStatusPollingInterval;
    uint32_t boardSerial[4];
    uint32_t boardSignature[2];
    diypinball_systemManagementPowerStatusHandler powerStatusHandler;
    uint32_t lastTick;
} diypinball_systemManagementInstance_t;

typedef struct diypinball_systemManagementInit {
    uint8_t firmwareVersionMajor;
    uint8_t firmwareVersionMinor;
    uint8_t firmwareVersionPatch;
    uint32_t boardSerial[4];
    uint32_t boardSignature[2];
    diypinball_systemManagementPowerStatusHandler powerStatusHandler;
    diypinball_featureRouterInstance_t *routerInstance;
} diypinball_systemManagementInit_t;

void diypinball_systemManagement_init(diypinball_systemManagementInstance_t *instance, diypinball_systemManagementInit_t *init);
void diypinball_systemManagement_millisecondTickHandler(void *instance, uint32_t tickNum);
void diypinball_systemManagement_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message);
void diypinball_systemManagement_sendPoweronMessages(diypinball_systemManagementInstance_t *instance);

#ifdef __cplusplus
}
#endif
