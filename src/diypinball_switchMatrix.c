#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_switchMatrix.h"

void diypinball_switchMatrix_init(diypinball_switchMatrixInstance_t *instance, diypinball_switchMatrixInit_t *init) {
    instance->numSwitches = init->numSwitches;
    instance->readStateHandler = init->readStateHandler;
    instance->debounceChangedHandler = init->debounceChangedHandler;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->switches[i].lastState = 0;
        instance->switches[i].messageTriggerMask = 0;
        instance->switches[i].pollingInterval = 0;
        instance->switches[i].lastTick = 0;
        instance->switches[i].debounceLimit = 0;
        instance->switches[i].ruleMask = 0;
        instance->switches[i].closeRule.boardAddress = 0;
        instance->switches[i].closeRule.solenoidNum = 0;
        instance->switches[i].closeRule.attackStatus = 0;
        instance->switches[i].closeRule.attackDuration = 0;
        instance->switches[i].closeRule.sustainStatus = 0;
        instance->switches[i].closeRule.sustainDuration = 0;
        instance->switches[i].openRule.boardAddress = 0;
        instance->switches[i].openRule.solenoidNum = 0;
        instance->switches[i].openRule.attackStatus = 0;
        instance->switches[i].openRule.attackDuration = 0;
        instance->switches[i].openRule.sustainStatus = 0;
        instance->switches[i].openRule.sustainDuration = 0;
    }

    instance->featureDecoderInstance.concreteFeatureDecoderInstance = (void*) instance;
    instance->featureDecoderInstance.featureType = 1; // FIXME constant
    instance->featureDecoderInstance.messageHandler = diypinball_switchMatrix_messageReceivedHandler;
    instance->featureDecoderInstance.tickHandler = diypinball_switchMatrix_millisecondTickHandler;
    instance->featureDecoderInstance.routerInstance = init->routerInstance;
    diypinball_featureRouter_addFeature(init->routerInstance, &(instance->featureDecoderInstance));
}

void diypinball_switchMatrix_millisecondTickHandler(void *instance, uint32_t tickNum) {

}

void diypinball_switchMatrix_messageReceivedHandler(void *instance, diypinball_pinballMessage_t *message) {
	
}

void diypinball_switchMatrix_deinit(diypinball_switchMatrixInstance_t *instance) {
    instance->featureDecoderInstance.concreteFeatureDecoderInstance = NULL;
    instance->featureDecoderInstance.featureType = 0;
    instance->featureDecoderInstance.messageHandler = NULL;
    instance->featureDecoderInstance.tickHandler = NULL;
    instance->featureDecoderInstance.routerInstance = NULL;

    instance->numSwitches = 0;
    instance->readStateHandler = NULL;
    instance->debounceChangedHandler = NULL;

    uint8_t i;
    for(i=0; i<16; i++) {
        instance->switches[i].lastState = 0;
        instance->switches[i].messageTriggerMask = 0;
        instance->switches[i].pollingInterval = 0;
        instance->switches[i].lastTick = 0;
        instance->switches[i].debounceLimit = 0;
        instance->switches[i].ruleMask = 0;
        instance->switches[i].closeRule.boardAddress = 0;
        instance->switches[i].closeRule.solenoidNum = 0;
        instance->switches[i].closeRule.attackStatus = 0;
        instance->switches[i].closeRule.attackDuration = 0;
        instance->switches[i].closeRule.sustainStatus = 0;
        instance->switches[i].closeRule.sustainDuration = 0;
        instance->switches[i].openRule.boardAddress = 0;
        instance->switches[i].openRule.solenoidNum = 0;
        instance->switches[i].openRule.attackStatus = 0;
        instance->switches[i].openRule.attackDuration = 0;
        instance->switches[i].openRule.sustainStatus = 0;
        instance->switches[i].openRule.sustainDuration = 0;
    }
}