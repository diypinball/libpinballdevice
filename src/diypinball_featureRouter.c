#include "diypinball.h"
#include "diypinball_featureRouter.h"

#include <stdint.h>
#include <string.h>

void diypinball_featureRouter_init(diypinball_featureRouterInstance_t* context, diypinball_featureRouterInit_t* init) {
    uint8_t i;

    for(i=0; i<16; i++) {
        context->features[i] = NULL;
    }

    context->boardAddress = init->boardAddress;
    context->canSendHandler = init->canSendHandler;

    return;
}

void diypinball_featureRouter_deinit(diypinball_featureRouterInstance_t* context) {
	uint8_t i;

    for(i=0; i<16; i++) {
        context->features[i] = NULL;
    }

	context->boardAddress = 0;
	context->canSendHandler = NULL;

	return;
}

diypinball_result_t diypinball_featureRouter_addFeature(diypinball_featureRouterInstance_t* context, diypinball_featureDecoderInstance_t* featureDecoder) {
	uint8_t featureNum = featureDecoder->featureNum;

    if((featureNum >= 0) && (featureNum < 16)) {
        context->features[featureNum] = featureDecoder;
        return RESULT_SUCCESS;
    } else {
        return RESULT_FAIL_INVALID_PARAMETER;
    }
}

void diypinball_featureRouter_receiveCAN(diypinball_featureRouterInstance_t* context, diypinball_canMessage_t* message) {
	diypinball_pinballMessage_t decodedMessage;

    decodedMessage.priority = (message->id & 0x1E000000) >> 25;
    decodedMessage.unitSpecific = (message->id & 0x01000000) >> 24;
    decodedMessage.boardAddress = (message->id & 0x00FF0000) >> 16;
    decodedMessage.featureType = (message->id & 0x0000F000) >> 12;
    decodedMessage.featureNum = (message->id & 0x00000F00) >> 8;
    decodedMessage.function = (message->id & 0x000000F0) >> 4;
    decodedMessage.reserved = (message->id & 0x0000000F);

    if(message->rtr) {
        decodedMessage.messageType = MESSAGE_REQUEST;
    } else {
        decodedMessage.messageType = MESSAGE_COMMAND;
    }
    decodedMessage.dataLength = message->dlc;
    memcpy(decodedMessage.data, message->data, 8);

    if(context->features[decodedMessage.featureType] != NULL) {
        (context->features[decodedMessage.featureType]->messageHandler)(context->features[decodedMessage.featureType]->instance, &decodedMessage);
    }
}