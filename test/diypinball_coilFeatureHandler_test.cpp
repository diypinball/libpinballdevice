#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_coilFeatureHandler.h"
#include "canMocks.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class MockCoilFeatureHandlerHandlers {
public:
    virtual ~MockCoilFeatureHandlerHandlers() {}
    MOCK_METHOD2(testCoilChangedHandler, void(uint8_t, diypinball_coilStatus_t));
};

static MockCANSend* CANSendImpl;
static MockCoilFeatureHandlerHandlers* CoilFeatureHandlerHandlersImpl;

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        CANSendImpl->testCanSendHandler(message);
    }

    static void testCoilChangedHandler(uint8_t coilNum, diypinball_coilStatus_t coilStatus) {
        CoilFeatureHandlerHandlersImpl->testCoilChangedHandler(coilNum, coilStatus);
    }
}

MATCHER_P(CoilStatusEqual, status, "") {
    //printf("Arg: %d, %d, %d, %d\n", arg.attackState, arg.attackDuration, arg.sustainState, arg.sustainDuration);
    //printf("Sta: %d, %d, %d, %d\n", status.attackState, status.attackDuration, status.sustainState, status.sustainDuration);
    uint8_t fieldFlag = (arg.attackState == status.attackState) &&
        (arg.attackDuration == status.attackDuration) &&
        (arg.sustainState == status.sustainState) &&
        (arg.sustainDuration == status.sustainDuration);

    return !(!fieldFlag);
}

class diypinball_coilFeatureHandler_test : public testing::Test {
    protected: 

    virtual void SetUp() {
        CANSendImpl = &myCANSend;
        CoilFeatureHandlerHandlersImpl = &myCoilFeatureHandlerHandlers;

        diypinball_featureRouterInit_t routerInit;

        routerInit.boardAddress = 42;
        routerInit.canSendHandler = testCanSendHandler;

        diypinball_featureRouter_init(&router, &routerInit);

        diypinball_coilFeatureHandlerInit_t coilFeatureHandlerInit;

        coilFeatureHandlerInit.numCoils = 15;
        coilFeatureHandlerInit.coilChangedHandler = testCoilChangedHandler;
        coilFeatureHandlerInit.routerInstance = &router;

        diypinball_coilFeatureHandler_init(&coilFeatureHandler, &coilFeatureHandlerInit);
    }

    diypinball_featureRouterInstance_t router;
    diypinball_coilFeatureHandlerInstance_t coilFeatureHandler;
    MockCANSend myCANSend;
    MockCoilFeatureHandlerHandlers myCoilFeatureHandlerHandlers;
};

TEST_F(diypinball_coilFeatureHandler_test, init_zeros_structure)
{
    ASSERT_EQ(3, coilFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, coilFeatureHandler.coils[i].attackState);
        ASSERT_EQ(0, coilFeatureHandler.coils[i].attackDuration);
        ASSERT_EQ(0, coilFeatureHandler.coils[i].sustainState);
        ASSERT_EQ(0, coilFeatureHandler.coils[i].sustainDuration);
    }

    ASSERT_EQ(&router, coilFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(&coilFeatureHandler, coilFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(15, coilFeatureHandler.numCoils);
    ASSERT_TRUE(testCoilChangedHandler == coilFeatureHandler.coilChangedHandler);
    ASSERT_TRUE(diypinball_coilFeatureHandler_millisecondTickHandler == coilFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_coilFeatureHandler_messageReceivedHandler == coilFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_coilFeatureHandler_test, deinit_zeros_structure)
{
    diypinball_coilFeatureHandler_deinit(&coilFeatureHandler);

    ASSERT_EQ(0, coilFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, coilFeatureHandler.coils[i].attackState);
        ASSERT_EQ(0, coilFeatureHandler.coils[i].attackDuration);
        ASSERT_EQ(0, coilFeatureHandler.coils[i].sustainState);
        ASSERT_EQ(0, coilFeatureHandler.coils[i].sustainDuration);
    }

    ASSERT_EQ(NULL, coilFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(NULL, coilFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(0, coilFeatureHandler.numCoils);
    ASSERT_TRUE(NULL == coilFeatureHandler.coilChangedHandler);
    ASSERT_TRUE(NULL == coilFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(NULL == coilFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST(diypinball_coilFeatureHandler_test_other, init_too_many_coils)
{
    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_coilFeatureHandlerInstance_t coilFeatureHandler;
    diypinball_coilFeatureHandlerInit_t coilFeatureHandlerInit;

    coilFeatureHandlerInit.numCoils = 17;
    coilFeatureHandlerInit.coilChangedHandler = testCoilChangedHandler;
    coilFeatureHandlerInit.routerInstance = &router;

    diypinball_coilFeatureHandler_init(&coilFeatureHandler, &coilFeatureHandlerInit);

    ASSERT_EQ(3, coilFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, coilFeatureHandler.coils[i].attackState);
        ASSERT_EQ(0, coilFeatureHandler.coils[i].attackDuration);
        ASSERT_EQ(0, coilFeatureHandler.coils[i].sustainState);
        ASSERT_EQ(0, coilFeatureHandler.coils[i].sustainDuration);
    }

    ASSERT_EQ(&router, coilFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(&coilFeatureHandler, coilFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(16, coilFeatureHandler.numCoils);
    ASSERT_TRUE(testCoilChangedHandler == coilFeatureHandler.coilChangedHandler);
    ASSERT_TRUE(diypinball_coilFeatureHandler_millisecondTickHandler == coilFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_coilFeatureHandler_messageReceivedHandler == coilFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_coilFeatureHandler_test, request_to_function_0_to_invalid_coil_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (15 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_coilFeatureHandler_test, request_to_function_0_to_valid_coil_returns_coil_state)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 4;
    expectedCANMessage.data[0] = 0;
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_coilFeatureHandler_test, message_to_function_0_to_invalid_coil_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (15 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_coilFeatureHandler_test, message_to_function_0_to_valid_coil_and_no_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_coilFeatureHandler_test, message_to_function_0_to_valid_coil_and_not_enough_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 255;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_coilFeatureHandler_test, message_to_function_0_to_valid_coil_and_enough_data_sets_attack_only_and_sets_coil)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 2;
    initiatingCANMessage.data[0] = 255;
    initiatingCANMessage.data[1] = 0;

    diypinball_coilStatus_t expectedCoil;
    expectedCoil.attackState = 255;
    expectedCoil.attackDuration = 0;
    expectedCoil.sustainState = 0;
    expectedCoil.sustainDuration = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(0, CoilStatusEqual(expectedCoil))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_coilFeatureHandler_test, message_to_function_0_to_valid_coil_and_more_than_enough_data_sets_attack_only_and_sets_coil)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 3;
    initiatingCANMessage.data[0] = 255;
    initiatingCANMessage.data[1] = 50;
    initiatingCANMessage.data[2] = 127;

    diypinball_coilStatus_t expectedCoil;
    expectedCoil.attackState = 255;
    expectedCoil.attackDuration = 50;
    expectedCoil.sustainState = 0;
    expectedCoil.sustainDuration = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(0, CoilStatusEqual(expectedCoil))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_coilFeatureHandler_test, message_to_function_0_to_valid_coil_and_enough_data_sets_attack_and_sustain_and_sets_coil_then_retrieve)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 4;
    initiatingCANMessage.data[0] = 255;
    initiatingCANMessage.data[1] = 50;
    initiatingCANMessage.data[2] = 127;
    initiatingCANMessage.data[3] = 0;

    diypinball_coilStatus_t expectedCoil;
    expectedCoil.attackState = 255;
    expectedCoil.attackDuration = 50;
    expectedCoil.sustainState = 127;
    expectedCoil.sustainDuration = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(0, CoilStatusEqual(expectedCoil))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 4;
    expectedCANMessage.data[0] = 255;
    expectedCANMessage.data[1] = 50;
    expectedCANMessage.data[2] = 127;
    expectedCANMessage.data[3] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_coilFeatureHandler_test, message_to_function_0_to_valid_coil_then_update_then_retrieve)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 4;
    initiatingCANMessage.data[0] = 255;
    initiatingCANMessage.data[1] = 50;
    initiatingCANMessage.data[2] = 127;
    initiatingCANMessage.data[3] = 0;

    diypinball_coilStatus_t expectedCoil;
    expectedCoil.attackState = 255;
    expectedCoil.attackDuration = 50;
    expectedCoil.sustainState = 127;
    expectedCoil.sustainDuration = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(0, CoilStatusEqual(expectedCoil))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    diypinball_coilStatus_t coilUpdate;
    coilUpdate.attackState = 127;
    coilUpdate.attackDuration = 0;
    coilUpdate.sustainState = 0;
    coilUpdate.sustainDuration = 0;

    diypinball_coilFeatureHandler_registerCoilState(&coilFeatureHandler, 0, coilUpdate);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 4;
    expectedCANMessage.data[0] = 127;
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_coilFeatureHandler_test, request_to_function_1_through_15_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 1; i < 16; i++) {
        for(uint8_t j = 0; j < 16; j++) {
            initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (j << 8) | (i << 4) | 0;
            initiatingCANMessage.rtr = 1;
            initiatingCANMessage.dlc = 0;

            EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
            EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(_, _)).Times(0);

            diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
        }
    }
}

TEST_F(diypinball_coilFeatureHandler_test, message_to_function_1_through_15_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 7; i < 16; i++) {
        for(uint8_t j = 0; j < 16; j++) {
            initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (3 << 12) | (j << 8) | (i << 4) | 0;
            initiatingCANMessage.rtr = 0;
            initiatingCANMessage.dlc = 2;
            initiatingCANMessage.data[0] = 0x42;
            initiatingCANMessage.data[1] = 0x24;

            EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
            EXPECT_CALL(myCoilFeatureHandlerHandlers, testCoilChangedHandler(_, _)).Times(0);

            diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
        }
    }
}
