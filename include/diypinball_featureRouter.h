#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "diypinball.h"

#include <stdint.h>

typedef struct diypinball_featureRouterInstance diypinball_featureRouterInstance_t;
typedef struct diypinball_featureRouterInit diypinball_featureRouterInit_t;
typedef struct diypinball_featureDecoderInstance diypinball_featureDecoderInstance_t;

typedef void (*diypinball_messageReceivedHandler)(void *instance, diypinball_pinballMessage_t *message);
typedef void (*diypinball_millisecondTickHandler)(void *instance, uint32_t tickNum);

struct diypinball_featureRouterInstance {
    uint8_t boardAddress;
    diypinball_featureDecoderInstance_t* features[16];
    diypinball_canMessageSendHandler canSendHandler;
};

struct diypinball_featureRouterInit {
    uint8_t boardAddress;
    diypinball_canMessageSendHandler canSendHandler;
};

struct diypinball_featureDecoderInstance {
    uint8_t featureNum;
    void *instance;
    diypinball_featureRouterInstance_t *routerInstance;
    diypinball_messageReceivedHandler messageHandler; //function pointer to message received handler
    diypinball_millisecondTickHandler tickHandler; //function pointer to millisecond tick handler
};

void diypinball_featureRouter_init(diypinball_featureRouterInstance_t* context, diypinball_featureRouterInit_t* init);
void diypinball_featureRouter_deinit(diypinball_featureRouterInstance_t* context);
diypinball_result_t diypinball_featureRouter_addFeature(diypinball_featureRouterInstance_t* context, diypinball_featureDecoderInstance_t* featureDecoder);
void diypinball_featureRouter_receiveCAN(diypinball_featureRouterInstance_t* context, diypinball_canMessage_t* message);
void diypinball_featureRouter_getFeatureBitmap(diypinball_featureRouterInstance_t *context, uint16_t *bitmap);
void diypinball_featureRouter_millisecondTick(diypinball_featureRouterInstance_t* context, uint32_t tickNum);

#ifdef __cplusplus
}
#endif