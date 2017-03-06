#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "canMocks.h"

using ::testing::Return;
using ::testing::_; // Matcher for parameters

class MockHandlers {
public:
    virtual ~MockHandlers() {}
    MOCK_METHOD2(testMessageReceivedHandler, void(void*, diypinball_pinballMessage_t*));
    MOCK_METHOD2(testMillisecondReceivedHandler, void(void*, uint32_t));
};

static MockCANSend* CANSendImpl;
static MockHandlers* Handler1;
static MockHandlers* Handler2;

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        CANSendImpl->testCanSendHandler(message);
    }

    static void messageReceivedHandler1(void *featureDecoderInstance, diypinball_pinballMessage_t *message) {
        Handler1->testMessageReceivedHandler(featureDecoderInstance, message);
    }

    static void millisecondTickHandler1(void *featureDecoderInstance, uint32_t tickNum) {
        Handler1->testMillisecondReceivedHandler(featureDecoderInstance, tickNum);
    }

    static void messageReceivedHandler2(void *featureDecoderInstance, diypinball_pinballMessage_t *message) {
        Handler2->testMessageReceivedHandler(featureDecoderInstance, message);;
    }

    static void millisecondTickHandler2(void *featureDecoderInstance, uint32_t tickNum) {
        Handler2->testMillisecondReceivedHandler(featureDecoderInstance, tickNum);
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
    ASSERT_TRUE(instance.canSendHandler == testCanSendHandler);
}

TEST(diypinball_featureRouter_test, addfeature_adds_feature) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    diypinball_featureDecoderInstance feature;
    feature.featureType = 1;
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

    ASSERT_EQ((instance.features[1])->featureType, feature.featureType);
    ASSERT_TRUE((instance.features[1])->messageHandler == messageReceivedHandler1);
    ASSERT_TRUE((instance.features[1])->tickHandler == millisecondTickHandler1);
}

TEST(diypinball_featureRouter_test, deinit_zeros_structure_and_functions) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    diypinball_featureDecoderInstance feature;
    feature.featureType = 1;
    feature.routerInstance = &instance;
    feature.messageHandler = messageReceivedHandler1;
    feature.tickHandler = millisecondTickHandler1;

    diypinball_featureRouter_addFeature(&instance, &feature);

    diypinball_featureRouter_deinit(&instance);

    for(uint8_t i=0; i<16; i++) {
        ASSERT_EQ(NULL, instance.features[i]);
    }

    ASSERT_EQ(instance.boardAddress, 0);
    ASSERT_EQ(NULL, instance.canSendHandler);
}

TEST(diypinball_featureRouter_test, addfeature_with_invalid_featureType_adds_nothing) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    diypinball_featureDecoderInstance feature;
    feature.featureType = 17;
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

    uint32_t dummyContext1, dummyContext2;

    diypinball_featureDecoderInstance feature1;
    feature1.featureType = 1;
    feature1.concreteFeatureDecoderInstance = (void*) &dummyContext1;
    feature1.routerInstance = &instance;
    feature1.messageHandler = messageReceivedHandler1;
    feature1.tickHandler = millisecondTickHandler1;

    diypinball_featureDecoderInstance feature2;
    feature2.featureType = 2;
    feature2.concreteFeatureDecoderInstance = (void*) &dummyContext2;
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

    ASSERT_EQ((instance.features[1])->featureType, feature1.featureType);
    ASSERT_TRUE((instance.features[1])->messageHandler == messageReceivedHandler1);
    ASSERT_TRUE((instance.features[1])->tickHandler == millisecondTickHandler1);
    ASSERT_EQ(&dummyContext1, (instance.features[1])->concreteFeatureDecoderInstance);

    ASSERT_EQ((instance.features[2])->featureType, feature2.featureType);
    ASSERT_TRUE((instance.features[2])->messageHandler == messageReceivedHandler2);
    ASSERT_TRUE((instance.features[2])->tickHandler == millisecondTickHandler2);
    ASSERT_EQ(&dummyContext2, (instance.features[2])->concreteFeatureDecoderInstance);
}

TEST(diypinball_featureRouter_test, two_features_incoming_can_message_routed_properly_and_decoded) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;
    MockHandlers myHandler1, myHandler2;
    Handler1 = &myHandler1;
    Handler2 = &myHandler2;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    uint32_t dummyContext1, dummyContext2;

    diypinball_featureDecoderInstance feature1;
    feature1.featureType = 1;
    feature1.concreteFeatureDecoderInstance = (void*) &dummyContext1;
    feature1.routerInstance = &instance;
    feature1.messageHandler = messageReceivedHandler1;
    feature1.tickHandler = millisecondTickHandler1;

    diypinball_featureDecoderInstance feature2;
    feature2.featureType = 2;
    feature2.concreteFeatureDecoderInstance = (void*) &dummyContext2;
    feature2.routerInstance = &instance;
    feature2.messageHandler = messageReceivedHandler2;
    feature2.tickHandler = millisecondTickHandler2;

    diypinball_result_t featureResult;

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature1);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature2);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    diypinball_canMessage_t message;
    message.id = (3 << 25) | (1 << 24) | (init.boardAddress << 16) | (2 << 12) | (5 << 8) | (6 << 4) | 0;
    message.rtr = 0;
    message.dlc = 8;
    message.data[0] = 255;
    message.data[1] = 127;
    message.data[2] = 63;
    message.data[3] = 31;
    message.data[4] = 15;
    message.data[5] = 7;
    message.data[6] = 3;
    message.data[7] = 1;

    diypinball_pinballMessage_t pinballMessage;
    pinballMessage.priority = 3;
    pinballMessage.unitSpecific = 1;
    pinballMessage.boardAddress = 42;
    pinballMessage.featureType = 2;
    pinballMessage.featureNum = 5;
    pinballMessage.function = 6;
    pinballMessage.reserved = 0;
    pinballMessage.messageType = MESSAGE_COMMAND;
    pinballMessage.dataLength = 8;
    pinballMessage.data[0] = 255;
    pinballMessage.data[1] = 127;
    pinballMessage.data[2] = 63;
    pinballMessage.data[3] = 31;
    pinballMessage.data[4] = 15;
    pinballMessage.data[5] = 7;
    pinballMessage.data[6] = 3;
    pinballMessage.data[7] = 1;

    EXPECT_CALL(myHandler1, testMessageReceivedHandler(_, _)).Times(0);
    EXPECT_CALL(myHandler2, testMessageReceivedHandler(_, PinballMessageEqual(pinballMessage))).Times(1);

    diypinball_featureRouter_receiveCAN(&instance, &message);

    Handler1 = NULL;
    Handler2 = NULL;
}

TEST(diypinball_featureRouter_test, two_features_incoming_can_message_with_bad_feature_not_routed) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;
    MockHandlers myHandler1, myHandler2;
    Handler1 = &myHandler1;
    Handler2 = &myHandler2;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    uint32_t dummyContext1, dummyContext2;

    diypinball_featureDecoderInstance feature1;
    feature1.featureType = 1;
    feature1.concreteFeatureDecoderInstance = (void*) &dummyContext1;
    feature1.routerInstance = &instance;
    feature1.messageHandler = messageReceivedHandler1;
    feature1.tickHandler = millisecondTickHandler1;

    diypinball_featureDecoderInstance feature2;
    feature2.featureType = 2;
    feature2.concreteFeatureDecoderInstance = (void*) &dummyContext2;
    feature2.routerInstance = &instance;
    feature2.messageHandler = messageReceivedHandler2;
    feature2.tickHandler = millisecondTickHandler2;

    diypinball_result_t featureResult;

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature1);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature2);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    diypinball_canMessage_t message;
    message.id = (init.boardAddress << 16) | (8 << 12);
    message.rtr = 0;
    message.dlc = 8;
    message.data[0] = 255;
    message.data[1] = 127;
    message.data[2] = 63;
    message.data[3] = 31;
    message.data[4] = 15;
    message.data[5] = 7;
    message.data[6] = 3;
    message.data[7] = 1;

    EXPECT_CALL(myHandler1, testMessageReceivedHandler(_, _)).Times(0);
    EXPECT_CALL(myHandler2, testMessageReceivedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&instance, &message);

    Handler1 = NULL;
    Handler2 = NULL;
}

TEST(diypinball_featureRouter_test, two_features_bitmap) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    uint32_t dummyContext1, dummyContext2;

    diypinball_featureDecoderInstance feature1;
    feature1.featureType = 1;
    feature1.concreteFeatureDecoderInstance = (void*) &dummyContext1;
    feature1.routerInstance = &instance;
    feature1.messageHandler = messageReceivedHandler1;
    feature1.tickHandler = millisecondTickHandler1;

    diypinball_featureDecoderInstance feature2;
    feature2.featureType = 2;
    feature2.concreteFeatureDecoderInstance = (void*) &dummyContext2;
    feature2.routerInstance = &instance;
    feature2.messageHandler = messageReceivedHandler2;
    feature2.tickHandler = millisecondTickHandler2;

    diypinball_result_t featureResult;

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature1);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature2);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    uint16_t featureBitmap;
    diypinball_featureRouter_getFeatureBitmap(&instance, &featureBitmap);
    ASSERT_EQ(0b0000000000000110, featureBitmap);
}

TEST(diypinball_featureRouter_test, two_features_millisecond_tick) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;
    MockHandlers myHandler1, myHandler2;
    Handler1 = &myHandler1;
    Handler2 = &myHandler2;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    uint32_t dummyContext1, dummyContext2;

    diypinball_featureDecoderInstance feature1;
    feature1.featureType = 1;
    feature1.concreteFeatureDecoderInstance = (void*) &dummyContext1;
    feature1.routerInstance = &instance;
    feature1.messageHandler = messageReceivedHandler1;
    feature1.tickHandler = millisecondTickHandler1;

    diypinball_featureDecoderInstance feature2;
    feature2.featureType = 2;
    feature2.concreteFeatureDecoderInstance = (void*) &dummyContext2;
    feature2.routerInstance = &instance;
    feature2.messageHandler = messageReceivedHandler2;
    feature2.tickHandler = millisecondTickHandler2;

    diypinball_result_t featureResult;

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature1);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    featureResult = diypinball_featureRouter_addFeature(&instance, &feature2);
    ASSERT_EQ(RESULT_SUCCESS, featureResult);

    EXPECT_CALL(myHandler1, testMillisecondReceivedHandler((void*)&dummyContext1, 2)).Times(1);
    EXPECT_CALL(myHandler2, testMillisecondReceivedHandler((void*)&dummyContext2, 2)).Times(1);

    diypinball_featureRouter_millisecondTick(&instance, 2);

    Handler1 = NULL;
    Handler2 = NULL;
}

TEST(diypinball_featureRouter_test, send_message_response) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    diypinball_pinballMessage_t pinballMessage;
    pinballMessage.priority = 3;
    pinballMessage.unitSpecific = 1;
    pinballMessage.boardAddress = 43;
    pinballMessage.featureType = 2;
    pinballMessage.featureNum = 5;
    pinballMessage.function = 6;
    pinballMessage.reserved = 0;
    pinballMessage.messageType = MESSAGE_RESPONSE;
    pinballMessage.dataLength = 8;
    pinballMessage.data[0] = 255;
    pinballMessage.data[1] = 127;
    pinballMessage.data[2] = 63;
    pinballMessage.data[3] = 31;
    pinballMessage.data[4] = 15;
    pinballMessage.data[5] = 7;
    pinballMessage.data[6] = 3;
    pinballMessage.data[7] = 1;

    diypinball_canMessage_t expectedCANMessage;
    expectedCANMessage.id = (3 << 25) | (1 << 24) | (init.boardAddress << 16) | (2 << 12) | (5 << 8) | (6 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 8;
    expectedCANMessage.data[0] = 255;
    expectedCANMessage.data[1] = 127;
    expectedCANMessage.data[2] = 63;
    expectedCANMessage.data[3] = 31;
    expectedCANMessage.data[4] = 15;
    expectedCANMessage.data[5] = 7;
    expectedCANMessage.data[6] = 3;
    expectedCANMessage.data[7] = 1;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_sendPinballMessage(&instance, &pinballMessage);
}

TEST(diypinball_featureRouter_test, send_message_command) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    diypinball_pinballMessage_t pinballMessage;
    pinballMessage.priority = 3;
    pinballMessage.unitSpecific = 1;
    pinballMessage.boardAddress = 43;
    pinballMessage.featureType = 2;
    pinballMessage.featureNum = 5;
    pinballMessage.function = 6;
    pinballMessage.reserved = 0;
    pinballMessage.messageType = MESSAGE_COMMAND;
    pinballMessage.dataLength = 8;
    pinballMessage.data[0] = 255;
    pinballMessage.data[1] = 127;
    pinballMessage.data[2] = 63;
    pinballMessage.data[3] = 31;
    pinballMessage.data[4] = 15;
    pinballMessage.data[5] = 7;
    pinballMessage.data[6] = 3;
    pinballMessage.data[7] = 1;

    diypinball_canMessage_t expectedCANMessage;
    expectedCANMessage.id = (3 << 25) | (1 << 24) | (43 << 16) | (2 << 12) | (5 << 8) | (6 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 8;
    expectedCANMessage.data[0] = 255;
    expectedCANMessage.data[1] = 127;
    expectedCANMessage.data[2] = 63;
    expectedCANMessage.data[3] = 31;
    expectedCANMessage.data[4] = 15;
    expectedCANMessage.data[5] = 7;
    expectedCANMessage.data[6] = 3;
    expectedCANMessage.data[7] = 1;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_sendPinballMessage(&instance, &pinballMessage);
}

TEST(diypinball_featureRouter_test, send_message_request) {
    diypinball_featureRouterInstance instance;
    diypinball_featureRouterInit init;

    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;

    init.boardAddress = 42;
    init.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&instance, &init);

    diypinball_pinballMessage_t pinballMessage;
    pinballMessage.priority = 3;
    pinballMessage.unitSpecific = 1;
    pinballMessage.boardAddress = 43;
    pinballMessage.featureType = 2;
    pinballMessage.featureNum = 5;
    pinballMessage.function = 6;
    pinballMessage.reserved = 0;
    pinballMessage.messageType = MESSAGE_REQUEST;
    pinballMessage.dataLength = 8;
    pinballMessage.data[0] = 255;
    pinballMessage.data[1] = 127;
    pinballMessage.data[2] = 63;
    pinballMessage.data[3] = 31;
    pinballMessage.data[4] = 15;
    pinballMessage.data[5] = 7;
    pinballMessage.data[6] = 3;
    pinballMessage.data[7] = 1;

    diypinball_canMessage_t expectedCANMessage;
    expectedCANMessage.id = (3 << 25) | (1 << 24) | (43 << 16) | (2 << 12) | (5 << 8) | (6 << 4) | 0;
    expectedCANMessage.rtr = 1;
    expectedCANMessage.dlc = 8;
    expectedCANMessage.data[0] = 255;
    expectedCANMessage.data[1] = 127;
    expectedCANMessage.data[2] = 63;
    expectedCANMessage.data[3] = 31;
    expectedCANMessage.data[4] = 15;
    expectedCANMessage.data[5] = 7;
    expectedCANMessage.data[6] = 3;
    expectedCANMessage.data[7] = 1;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_sendPinballMessage(&instance, &pinballMessage);
}