#include "gtest/gtest.h"

#include <stdint.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        return;
    }

    static void messageReceivedHandler1(void *instance, diypinball_canMessage_t *message) {
        return;
    }

    static void millisecondTickHandler1(void *instance, uint32_t tickNum) {
        return;
    }

    static void messageReceivedHandler2(void *instance, diypinball_canMessage_t *message) {
        return;
    }

    static void millisecondTickHandler2(void *instance, uint32_t tickNum) {
        return;
    }
}

TEST(diypinball_featureRouter_test, init_zeros_structure)
{
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    for(uint8_t i=0; i<16; i++) {
        ASSERT_EQ(NULL, instance.features[i]);
    }

    ASSERT_EQ(instance.boardAddress, init.boardAddress);
    ASSERT_EQ(instance.canSendHandler, testCanSendHandler);
}

TEST(diypinball_featureRouter_test, addfeature_adds_feature) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    diypinball_featureDecoderInstance feature;
    feature.featureNum = 1;
    feature.routerInstance = &instance;
    feature.messageHandler = messageReceivedHandler1;
    feature.tickHandler = millisecondTickHandler1;

    diypinball_result_t featureResult;

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    for(uint8_t i=0; i<16; i++) {
        if(i == 1) break;
        ASSERT_EQ(NULL, instance.features[i]);
    }

    ASSERT_EQ((instance.features[1])->featureNum, feature.featureNum);
    ASSERT_EQ((instance.features[1])->messageHandler, messageReceivedHandler1);
    ASSERT_EQ((instance.features[1])->tickHandler, millisecondTickHandler1);
}

TEST(diypinball_featureRouter_test, deinit_zeros_structure_and_functions) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    diypinball_featureDecoderInstance feature;
    feature.featureNum = 1;
    feature.routerInstance = &instance;
    feature.messageHandler = messageReceivedHandler1;
    feature.tickHandler = millisecondTickHandler1;

    diypinball_result_t featureResult;

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature);

    diypinball_featureRouter_deinit(&instance);

    for(uint8_t i=0; i<16; i++) {
        ASSERT_EQ(NULL, instance.features[i]);
    }

    ASSERT_EQ(instance.boardAddress, 0);
    ASSERT_EQ(NULL, instance.canSendHandler);
}

TEST(diypinball_featureRouter_test, addfeature_with_invalid_featurenum_adds_nothing) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    diypinball_featureDecoderInstance feature;
    feature.featureNum = 17;
    feature.routerInstance = &instance;
    feature.messageHandler = messageReceivedHandler1;
    feature.tickHandler = millisecondTickHandler1;

    diypinball_result_t featureResult;

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature);
    ASSERT_EQ(RESULT_FAIL_INVALID_PARAMETER, featureResult);

    for(uint8_t i=0; i<16; i++) {
        ASSERT_EQ(NULL, instance.features[i]);
    }
}

TEST(diypinball_featureRouter_test, addfeature_adds_2_features) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    diypinball_featureDecoderInstance feature1;
    feature1.featureNum = 1;
    feature1.routerInstance = &instance;
    feature1.messageHandler = messageReceivedHandler1;
    feature1.tickHandler = millisecondTickHandler1;

    diypinball_featureDecoderInstance feature2;
    feature2.featureNum = 2;
    feature2.routerInstance = &instance;
    feature2.messageHandler = messageReceivedHandler2;
    feature2.tickHandler = millisecondTickHandler2;

    diypinball_result_t featureResult;

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature1);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature2);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    for(uint8_t i=0; i<16; i++) {
        if((i == 1) || (i == 2)) break;
        ASSERT_EQ(NULL, instance.features[i]);
    }

    ASSERT_EQ((instance.features[1])->featureNum, feature1.featureNum);
    ASSERT_EQ((instance.features[1])->messageHandler, messageReceivedHandler1);
    ASSERT_EQ((instance.features[1])->tickHandler, millisecondTickHandler1);

    ASSERT_EQ((instance.features[2])->featureNum, feature2.featureNum);
    ASSERT_EQ((instance.features[2])->messageHandler, messageReceivedHandler2);
    ASSERT_EQ((instance.features[2])->tickHandler, millisecondTickHandler2);
}