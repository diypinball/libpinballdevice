#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_lampFeatureHandler.h"
#include "canMocks.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class MockLampFeatureHandlerHandlers {
public:
    virtual ~MockLampFeatureHandlerHandlers() {}
    MOCK_METHOD2(testLampChangedHandler, void(uint8_t, diypinball_lampStatus_t));
};

static MockCANSend* CANSendImpl;
static MockLampFeatureHandlerHandlers* LampFeatureHandlerHandlersImpl;

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        CANSendImpl->testCanSendHandler(message);
    }

    static void testLampChangedHandler(uint8_t lampNum, diypinball_lampStatus_t lampStatus) {
        LampFeatureHandlerHandlersImpl->testLampChangedHandler(lampNum, lampStatus);
    }
}

MATCHER_P(LampStatusEqual, status, "") {
    uint8_t fieldFlag = (arg.state1 == status.state1) &&
        (arg.state1Duration == status.state1Duration) &&
        (arg.state2 == status.state2) &&
        (arg.state2Duration == status.state2Duration) &&
        (arg.state3 == status.state3) &&
        (arg.state3Duration == status.state3Duration) &&
        (arg.numStates == status.numStates);

    return !(!fieldFlag);
}

class diypinball_lampFeatureHandler_test : public testing::Test {
    protected: 

    virtual void SetUp() {
        CANSendImpl = &myCANSend;
        LampFeatureHandlerHandlersImpl = &myLampFeatureHandlerHandlers;

        diypinball_featureRouterInit_t routerInit;

        routerInit.boardAddress = 42;
        routerInit.canSendHandler = testCanSendHandler;

        diypinball_featureRouter_init(&router, &routerInit);

        diypinball_lampFeatureHandlerInit_t lampFeatureHandlerInit;

        lampFeatureHandlerInit.numLamps = 15;
        lampFeatureHandlerInit.lampChangedHandler = testLampChangedHandler;
        lampFeatureHandlerInit.routerInstance = &router;

        diypinball_lampFeatureHandler_init(&lampFeatureHandler, &lampFeatureHandlerInit);
    }

    diypinball_featureRouterInstance_t router;
    diypinball_lampFeatureHandlerInstance_t lampFeatureHandler;
    MockCANSend myCANSend;
    MockLampFeatureHandlerHandlers myLampFeatureHandlerHandlers;
};

TEST_F(diypinball_lampFeatureHandler_test, init_zeros_structure)
{
    ASSERT_EQ(2, lampFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state1);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state1Duration);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state2);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state2Duration);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state3);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state3Duration);
        ASSERT_EQ(1, lampFeatureHandler.lamps[i].numStates);
    }

    ASSERT_EQ(&router, lampFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(&lampFeatureHandler, lampFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(15, lampFeatureHandler.numLamps);
    ASSERT_TRUE(testLampChangedHandler == lampFeatureHandler.lampChangedHandler);
    ASSERT_TRUE(diypinball_lampFeatureHandler_millisecondTickHandler == lampFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_lampFeatureHandler_messageReceivedHandler == lampFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_lampFeatureHandler_test, deinit_zeros_structure)
{
    diypinball_lampFeatureHandler_deinit(&lampFeatureHandler);

    ASSERT_EQ(0, lampFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state1);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state1Duration);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state2);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state2Duration);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state3);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state3Duration);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].numStates);
    }

    ASSERT_EQ(NULL, lampFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(NULL, lampFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(0, lampFeatureHandler.numLamps);
    ASSERT_TRUE(NULL == lampFeatureHandler.lampChangedHandler);
    ASSERT_TRUE(NULL == lampFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(NULL == lampFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST(diypinball_lampFeatureHandler_test_other, init_too_many_lamps)
{
    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_lampFeatureHandlerInstance_t lampFeatureHandler;
    diypinball_lampFeatureHandlerInit_t lampFeatureHandlerInit;

    lampFeatureHandlerInit.numLamps = 17;
    lampFeatureHandlerInit.lampChangedHandler = testLampChangedHandler;
    lampFeatureHandlerInit.routerInstance = &router;

    diypinball_lampFeatureHandler_init(&lampFeatureHandler, &lampFeatureHandlerInit);

    ASSERT_EQ(2, lampFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state1);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state1Duration);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state2);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state2Duration);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state3);
        ASSERT_EQ(0, lampFeatureHandler.lamps[i].state3Duration);
        ASSERT_EQ(1, lampFeatureHandler.lamps[i].numStates);
    }

    ASSERT_EQ(&router, lampFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(&lampFeatureHandler, lampFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(16, lampFeatureHandler.numLamps);
    ASSERT_TRUE(testLampChangedHandler == lampFeatureHandler.lampChangedHandler);
    ASSERT_TRUE(diypinball_lampFeatureHandler_millisecondTickHandler == lampFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_lampFeatureHandler_messageReceivedHandler == lampFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_lampFeatureHandler_test, request_to_function_0_to_invalid_lamp_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (15 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, request_to_function_0_to_valid_lamp_returns_lamp_state)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 0;
    expectedCANMessage.data[1] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_0_to_invalid_lamp_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (15 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_0_to_valid_lamp_and_no_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_0_to_valid_lamp_with_only_brightness_sets_lamp)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 126;

    diypinball_lampStatus_t expectedLamp;
    expectedLamp.state1 = 126;
    expectedLamp.state1Duration = 0;
    expectedLamp.state2 = 0;
    expectedLamp.state2Duration = 0;
    expectedLamp.state3 = 0;
    expectedLamp.state3Duration = 0;
    expectedLamp.numStates = 1;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(1, LampStatusEqual(expectedLamp))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_0_to_valid_lamp_with_brightness_and_duration_sets_lamp)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 2;
    initiatingCANMessage.data[0] = 126;
    initiatingCANMessage.data[1] = 20;

    diypinball_lampStatus_t expectedLamp;
    expectedLamp.state1 = 126;
    expectedLamp.state1Duration = 0;
    expectedLamp.state2 = 0;
    expectedLamp.state2Duration = 0;
    expectedLamp.state3 = 0;
    expectedLamp.state3Duration = 0;
    expectedLamp.numStates = 1;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(1, LampStatusEqual(expectedLamp))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_0_to_valid_lamp_with_one_and_a_half_state_sets_lamp_solid)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 3;
    initiatingCANMessage.data[0] = 126;
    initiatingCANMessage.data[1] = 20;
    initiatingCANMessage.data[2] = 255;

    diypinball_lampStatus_t expectedLamp;
    expectedLamp.state1 = 126;
    expectedLamp.state1Duration = 0;
    expectedLamp.state2 = 0;
    expectedLamp.state2Duration = 0;
    expectedLamp.state3 = 0;
    expectedLamp.state3Duration = 0;
    expectedLamp.numStates = 1;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(1, LampStatusEqual(expectedLamp))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_0_to_valid_lamp_with_two_states_sets_lamp_blinking)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 4;
    initiatingCANMessage.data[0] = 126;
    initiatingCANMessage.data[1] = 20;
    initiatingCANMessage.data[2] = 255;
    initiatingCANMessage.data[3] = 40;

    diypinball_lampStatus_t expectedLamp;
    expectedLamp.state1 = 126;
    expectedLamp.state1Duration = 20;
    expectedLamp.state2 = 255;
    expectedLamp.state2Duration = 40;
    expectedLamp.state3 = 0;
    expectedLamp.state3Duration = 0;
    expectedLamp.numStates = 2;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(1, LampStatusEqual(expectedLamp))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_0_to_valid_lamp_with_two_and_a_half_states_sets_lamp_blinking)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 5;
    initiatingCANMessage.data[0] = 126;
    initiatingCANMessage.data[1] = 20;
    initiatingCANMessage.data[2] = 255;
    initiatingCANMessage.data[3] = 40;
    initiatingCANMessage.data[4] = 0;

    diypinball_lampStatus_t expectedLamp;
    expectedLamp.state1 = 126;
    expectedLamp.state1Duration = 20;
    expectedLamp.state2 = 255;
    expectedLamp.state2Duration = 40;
    expectedLamp.state3 = 0;
    expectedLamp.state3Duration = 0;
    expectedLamp.numStates = 2;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(1, LampStatusEqual(expectedLamp))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_0_to_valid_lamp_with_three_states_sets_lamp_blinking)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 6;
    initiatingCANMessage.data[0] = 126;
    initiatingCANMessage.data[1] = 20;
    initiatingCANMessage.data[2] = 255;
    initiatingCANMessage.data[3] = 40;
    initiatingCANMessage.data[4] = 0;
    initiatingCANMessage.data[5] = 200;

    diypinball_lampStatus_t expectedLamp;
    expectedLamp.state1 = 126;
    expectedLamp.state1Duration = 20;
    expectedLamp.state2 = 255;
    expectedLamp.state2Duration = 40;
    expectedLamp.state3 = 0;
    expectedLamp.state3Duration = 200;
    expectedLamp.numStates = 3;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(1, LampStatusEqual(expectedLamp))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_0_to_valid_lamp_with_three_states_and_read_back)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 6;
    initiatingCANMessage.data[0] = 126;
    initiatingCANMessage.data[1] = 20;
    initiatingCANMessage.data[2] = 255;
    initiatingCANMessage.data[3] = 40;
    initiatingCANMessage.data[4] = 0;
    initiatingCANMessage.data[5] = 200;

    diypinball_lampStatus_t expectedLamp;
    expectedLamp.state1 = 126;
    expectedLamp.state1Duration = 20;
    expectedLamp.state2 = 255;
    expectedLamp.state2Duration = 40;
    expectedLamp.state3 = 0;
    expectedLamp.state3Duration = 200;
    expectedLamp.numStates = 3;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(1, LampStatusEqual(expectedLamp))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;
    initiatingCANMessage.data[1] = 0;
    initiatingCANMessage.data[2] = 0;
    initiatingCANMessage.data[3] = 0;
    initiatingCANMessage.data[4] = 0;
    initiatingCANMessage.data[5] = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 6;

    expectedCANMessage.data[0] = 126;
    expectedCANMessage.data[1] = 20;
    expectedCANMessage.data[2] = 255;
    expectedCANMessage.data[3] = 40;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 200;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, request_to_function_1_to_any_set_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 0; i < 16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (i << 8) | (1 << 4) | 0;
        initiatingCANMessage.rtr = 1;
        initiatingCANMessage.dlc = 0;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(_, _)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_1_to_low_set_changes_lamps)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 8;
    initiatingCANMessage.data[0] = 16;
    initiatingCANMessage.data[1] = 32;
    initiatingCANMessage.data[2] = 64;
    initiatingCANMessage.data[3] = 128;
    initiatingCANMessage.data[4] = 8;
    initiatingCANMessage.data[5] = 4;
    initiatingCANMessage.data[6] = 2;
    initiatingCANMessage.data[7] = 1;

    diypinball_lampStatus_t expectedLamp0, expectedLamp1, expectedLamp2, expectedLamp3, expectedLamp4, expectedLamp5, expectedLamp6, expectedLamp7;
    expectedLamp0.state1 = 16;
    expectedLamp0.state1Duration = 0;
    expectedLamp0.state2 = 0;
    expectedLamp0.state2Duration = 0;
    expectedLamp0.state3 = 0;
    expectedLamp0.state3Duration = 0;
    expectedLamp0.numStates = 1;
    expectedLamp1.state1 = 32;
    expectedLamp1.state1Duration = 0;
    expectedLamp1.state2 = 0;
    expectedLamp1.state2Duration = 0;
    expectedLamp1.state3 = 0;
    expectedLamp1.state3Duration = 0;
    expectedLamp1.numStates = 1;
    expectedLamp2.state1 = 64;
    expectedLamp2.state1Duration = 0;
    expectedLamp2.state2 = 0;
    expectedLamp2.state2Duration = 0;
    expectedLamp2.state3 = 0;
    expectedLamp2.state3Duration = 0;
    expectedLamp2.numStates = 1;
    expectedLamp3.state1 = 128;
    expectedLamp3.state1Duration = 0;
    expectedLamp3.state2 = 0;
    expectedLamp3.state2Duration = 0;
    expectedLamp3.state3 = 0;
    expectedLamp3.state3Duration = 0;
    expectedLamp3.numStates = 1;
    expectedLamp4.state1 = 8;
    expectedLamp4.state1Duration = 0;
    expectedLamp4.state2 = 0;
    expectedLamp4.state2Duration = 0;
    expectedLamp4.state3 = 0;
    expectedLamp4.state3Duration = 0;
    expectedLamp4.numStates = 1;
    expectedLamp5.state1 = 4;
    expectedLamp5.state1Duration = 0;
    expectedLamp5.state2 = 0;
    expectedLamp5.state2Duration = 0;
    expectedLamp5.state3 = 0;
    expectedLamp5.state3Duration = 0;
    expectedLamp5.numStates = 1;
    expectedLamp6.state1 = 2;
    expectedLamp6.state1Duration = 0;
    expectedLamp6.state2 = 0;
    expectedLamp6.state2Duration = 0;
    expectedLamp6.state3 = 0;
    expectedLamp6.state3Duration = 0;
    expectedLamp6.numStates = 1;
    expectedLamp7.state1 = 1;
    expectedLamp7.state1Duration = 0;
    expectedLamp7.state2 = 0;
    expectedLamp7.state2Duration = 0;
    expectedLamp7.state3 = 0;
    expectedLamp7.state3Duration = 0;
    expectedLamp7.numStates = 1;

    InSequence dummy;
    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(0, LampStatusEqual(expectedLamp0))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(1, LampStatusEqual(expectedLamp1))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(2, LampStatusEqual(expectedLamp2))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(3, LampStatusEqual(expectedLamp3))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(4, LampStatusEqual(expectedLamp4))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(5, LampStatusEqual(expectedLamp5))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(6, LampStatusEqual(expectedLamp6))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(7, LampStatusEqual(expectedLamp7))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_1_to_high_set_changes_lamps)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 8;
    initiatingCANMessage.data[0] = 16;
    initiatingCANMessage.data[1] = 32;
    initiatingCANMessage.data[2] = 64;
    initiatingCANMessage.data[3] = 128;
    initiatingCANMessage.data[4] = 8;
    initiatingCANMessage.data[5] = 4;
    initiatingCANMessage.data[6] = 2;
    initiatingCANMessage.data[7] = 1;

    diypinball_lampStatus_t expectedLamp0, expectedLamp1, expectedLamp2, expectedLamp3, expectedLamp4, expectedLamp5, expectedLamp6, expectedLamp7;
    expectedLamp0.state1 = 16;
    expectedLamp0.state1Duration = 0;
    expectedLamp0.state2 = 0;
    expectedLamp0.state2Duration = 0;
    expectedLamp0.state3 = 0;
    expectedLamp0.state3Duration = 0;
    expectedLamp0.numStates = 1;
    expectedLamp1.state1 = 32;
    expectedLamp1.state1Duration = 0;
    expectedLamp1.state2 = 0;
    expectedLamp1.state2Duration = 0;
    expectedLamp1.state3 = 0;
    expectedLamp1.state3Duration = 0;
    expectedLamp1.numStates = 1;
    expectedLamp2.state1 = 64;
    expectedLamp2.state1Duration = 0;
    expectedLamp2.state2 = 0;
    expectedLamp2.state2Duration = 0;
    expectedLamp2.state3 = 0;
    expectedLamp2.state3Duration = 0;
    expectedLamp2.numStates = 1;
    expectedLamp3.state1 = 128;
    expectedLamp3.state1Duration = 0;
    expectedLamp3.state2 = 0;
    expectedLamp3.state2Duration = 0;
    expectedLamp3.state3 = 0;
    expectedLamp3.state3Duration = 0;
    expectedLamp3.numStates = 1;
    expectedLamp4.state1 = 8;
    expectedLamp4.state1Duration = 0;
    expectedLamp4.state2 = 0;
    expectedLamp4.state2Duration = 0;
    expectedLamp4.state3 = 0;
    expectedLamp4.state3Duration = 0;
    expectedLamp4.numStates = 1;
    expectedLamp5.state1 = 4;
    expectedLamp5.state1Duration = 0;
    expectedLamp5.state2 = 0;
    expectedLamp5.state2Duration = 0;
    expectedLamp5.state3 = 0;
    expectedLamp5.state3Duration = 0;
    expectedLamp5.numStates = 1;
    expectedLamp6.state1 = 2;
    expectedLamp6.state1Duration = 0;
    expectedLamp6.state2 = 0;
    expectedLamp6.state2Duration = 0;
    expectedLamp6.state3 = 0;
    expectedLamp6.state3Duration = 0;
    expectedLamp6.numStates = 1;
    expectedLamp7.state1 = 1;
    expectedLamp7.state1Duration = 0;
    expectedLamp7.state2 = 0;
    expectedLamp7.state2Duration = 0;
    expectedLamp7.state3 = 0;
    expectedLamp7.state3Duration = 0;
    expectedLamp7.numStates = 1;

    InSequence dummy;
    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(8, LampStatusEqual(expectedLamp0))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(9, LampStatusEqual(expectedLamp1))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(10, LampStatusEqual(expectedLamp2))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(11, LampStatusEqual(expectedLamp3))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(12, LampStatusEqual(expectedLamp4))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(13, LampStatusEqual(expectedLamp5))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(14, LampStatusEqual(expectedLamp6))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(15, LampStatusEqual(expectedLamp7))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_1_to_low_set_with_no_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_lampFeatureHandler_test, message_to_function_1_to_invalid_set_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 2; i < 16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (i << 8) | (1 << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 8;
        initiatingCANMessage.data[0] = 16;
        initiatingCANMessage.data[1] = 32;
        initiatingCANMessage.data[2] = 64;
        initiatingCANMessage.data[3] = 128;
        initiatingCANMessage.data[4] = 8;
        initiatingCANMessage.data[5] = 4;
        initiatingCANMessage.data[6] = 2;
        initiatingCANMessage.data[7] = 1;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(_, _)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST(diypinball_lampFeatureHandler_test_other, message_to_function_1_to_incomplete_high_set_changes_lamps)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockLampFeatureHandlerHandlers myLampFeatureHandlerHandlers;
    LampFeatureHandlerHandlersImpl = &myLampFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_lampFeatureHandlerInstance_t lampFeatureHandler;
    diypinball_lampFeatureHandlerInit_t lampFeatureHandlerInit;

    lampFeatureHandlerInit.numLamps = 14;
    lampFeatureHandlerInit.lampChangedHandler = testLampChangedHandler;
    lampFeatureHandlerInit.routerInstance = &router;

    diypinball_lampFeatureHandler_init(&lampFeatureHandler, &lampFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (2 << 12) | (1 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 6;
    initiatingCANMessage.data[0] = 16;
    initiatingCANMessage.data[1] = 32;
    initiatingCANMessage.data[2] = 64;
    initiatingCANMessage.data[3] = 128;
    initiatingCANMessage.data[4] = 8;
    initiatingCANMessage.data[5] = 4;

    diypinball_lampStatus_t expectedLamp0, expectedLamp1, expectedLamp2, expectedLamp3, expectedLamp4, expectedLamp5;
    expectedLamp0.state1 = 16;
    expectedLamp0.state1Duration = 0;
    expectedLamp0.state2 = 0;
    expectedLamp0.state2Duration = 0;
    expectedLamp0.state3 = 0;
    expectedLamp0.state3Duration = 0;
    expectedLamp0.numStates = 1;
    expectedLamp1.state1 = 32;
    expectedLamp1.state1Duration = 0;
    expectedLamp1.state2 = 0;
    expectedLamp1.state2Duration = 0;
    expectedLamp1.state3 = 0;
    expectedLamp1.state3Duration = 0;
    expectedLamp1.numStates = 1;
    expectedLamp2.state1 = 64;
    expectedLamp2.state1Duration = 0;
    expectedLamp2.state2 = 0;
    expectedLamp2.state2Duration = 0;
    expectedLamp2.state3 = 0;
    expectedLamp2.state3Duration = 0;
    expectedLamp2.numStates = 1;
    expectedLamp3.state1 = 128;
    expectedLamp3.state1Duration = 0;
    expectedLamp3.state2 = 0;
    expectedLamp3.state2Duration = 0;
    expectedLamp3.state3 = 0;
    expectedLamp3.state3Duration = 0;
    expectedLamp3.numStates = 1;
    expectedLamp4.state1 = 8;
    expectedLamp4.state1Duration = 0;
    expectedLamp4.state2 = 0;
    expectedLamp4.state2Duration = 0;
    expectedLamp4.state3 = 0;
    expectedLamp4.state3Duration = 0;
    expectedLamp4.numStates = 1;
    expectedLamp5.state1 = 4;
    expectedLamp5.state1Duration = 0;
    expectedLamp5.state2 = 0;
    expectedLamp5.state2Duration = 0;
    expectedLamp5.state3 = 0;
    expectedLamp5.state3Duration = 0;
    expectedLamp5.numStates = 1;

    InSequence dummy;
    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(8, LampStatusEqual(expectedLamp0))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(9, LampStatusEqual(expectedLamp1))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(10, LampStatusEqual(expectedLamp2))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(11, LampStatusEqual(expectedLamp3))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(12, LampStatusEqual(expectedLamp4))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(13, LampStatusEqual(expectedLamp5))).Times(1);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(14, _)).Times(0);
    EXPECT_CALL(myLampFeatureHandlerHandlers, testLampChangedHandler(15, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}
