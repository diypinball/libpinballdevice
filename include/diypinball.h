#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include <stdint.h>

typedef struct diypinball_canMessage {
    uint32_t id;
    uint8_t rtr;
    uint8_t dlc;
    uint8_t data[8];
} diypinball_canMessage_t;

typedef enum diypinball_pinballMessageType {
    MESSAGE_RESPONSE,
    MESSAGE_COMMAND,
    MESSAGE_REQUEST
} diypinball_pinballMessageType_t;

typedef struct diypinball_pinballMessage {
    uint8_t priority;
    uint8_t unitSpecific;
    uint8_t boardAddress;
    uint8_t featureType;
    uint8_t featureNum;
    uint8_t function;
    uint8_t reserved;
    diypinball_pinballMessageType_t messageType;
    uint8_t dataLength;
    uint8_t data[8];
} diypinball_pinballMessage_t;

typedef void (*diypinball_canMessageSendHandler)(diypinball_canMessage_t *message);

#ifdef __cplusplus
}
#endif