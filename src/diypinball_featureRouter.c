#include "diypinball.h"
#include "diypinball_featureRouter.h"

#include <stdint.h>

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
