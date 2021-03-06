#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_switchFeatureHandler.h"
#include "canMocks.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class MockSwitchFeatureHandlerHandlers {
public:
    virtual ~MockSwitchFeatureHandlerHandlers() {}
    MOCK_METHOD2(testReadStateHandler, void(uint8_t*, uint8_t));
    MOCK_METHOD2(testDebounceChangedHandler, void(uint8_t, uint8_t));
};

static MockCANSend* CANSendImpl;
static MockSwitchFeatureHandlerHandlers* SwitchFeatureHandlerHandlersImpl;

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        CANSendImpl->testCanSendHandler(message);
    }

    static void testDebounceChangedHandler(uint8_t switchNum, uint8_t debounceLimit) {
        SwitchFeatureHandlerHandlersImpl->testDebounceChangedHandler(switchNum, debounceLimit);
    }

    static void testReadStateHandler(uint8_t *state, uint8_t switchNum) {
        SwitchFeatureHandlerHandlersImpl->testReadStateHandler(state, switchNum);
        if(switchNum == 0) {
            *state = 1;
        } else {
            *state = 0;
        }
    }

    static void testReadStateHandlerZero(uint8_t *state, uint8_t switchNum) {
        SwitchFeatureHandlerHandlersImpl->testReadStateHandler(state, switchNum);
        *state = 0;
    }

    static void testReadStateHandlerAll(uint8_t *state, uint8_t switchNum) {
        SwitchFeatureHandlerHandlersImpl->testReadStateHandler(state, switchNum);
        if(switchNum % 2 == 0) {
            *state = 0;
        } else {
            *state = 1;
        }
    }
}

class diypinball_switchFeatureHandler_test : public testing::Test {
    protected: 

    virtual void SetUp() {
        CANSendImpl = &myCANSend;
        SwitchFeatureHandlerHandlersImpl = &mySwitchFeatureHandlerHandlers;

        diypinball_featureRouterInit_t routerInit;

        routerInit.boardAddress = 42;
        routerInit.canSendHandler = testCanSendHandler;

        diypinball_featureRouter_init(&router, &routerInit);

        diypinball_switchFeatureHandlerInit_t switchFeatureHandlerInit;

        switchFeatureHandlerInit.numSwitches = 15;
        switchFeatureHandlerInit.debounceChangedHandler = testDebounceChangedHandler;
        switchFeatureHandlerInit.readStateHandler = testReadStateHandler;
        switchFeatureHandlerInit.routerInstance = &router;

        diypinball_switchFeatureHandler_init(&switchFeatureHandler, &switchFeatureHandlerInit);
    }

    diypinball_featureRouterInstance_t router;
    diypinball_switchFeatureHandlerInstance_t switchFeatureHandler;
    MockCANSend myCANSend;
    MockSwitchFeatureHandlerHandlers mySwitchFeatureHandlerHandlers;
};

TEST_F(diypinball_switchFeatureHandler_test, init_zeros_structure)
{
    ASSERT_EQ(1, switchFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, switchFeatureHandler.switches[i].lastState);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].messageTriggerMask);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].lastTick);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].pollingInterval);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].debounceLimit);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].ruleMask);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.boardAddress);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.solenoidNum);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.attackStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.attackDuration);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.sustainStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.sustainDuration);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.boardAddress);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.solenoidNum);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.attackStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.attackDuration);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.sustainStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.sustainDuration);
    }

    ASSERT_EQ(&router, switchFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(&switchFeatureHandler, switchFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(15, switchFeatureHandler.numSwitches);
    ASSERT_TRUE(testReadStateHandler == switchFeatureHandler.readStateHandler);
    ASSERT_TRUE(testDebounceChangedHandler == switchFeatureHandler.debounceChangedHandler);
    ASSERT_TRUE(diypinball_switchFeatureHandler_millisecondTickHandler == switchFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_switchFeatureHandler_messageReceivedHandler == switchFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_switchFeatureHandler_test, deinit_zeros_structure)
{
    diypinball_switchFeatureHandler_deinit(&switchFeatureHandler);

    ASSERT_EQ(0, switchFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, switchFeatureHandler.switches[i].lastState);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].messageTriggerMask);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].lastTick);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].pollingInterval);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].debounceLimit);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].ruleMask);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.boardAddress);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.solenoidNum);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.attackStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.attackDuration);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.sustainStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.sustainDuration);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.boardAddress);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.solenoidNum);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.attackStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.attackDuration);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.sustainStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.sustainDuration);
    }

    ASSERT_EQ(NULL, switchFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(NULL, switchFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(0, switchFeatureHandler.numSwitches);
    ASSERT_TRUE(NULL == switchFeatureHandler.readStateHandler);
    ASSERT_TRUE(NULL == switchFeatureHandler.debounceChangedHandler);
    ASSERT_TRUE(NULL == switchFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(NULL == switchFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST(diypinball_switchFeatureHandler_test_other, init_too_many_switches)
{
    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchFeatureHandlerInstance_t switchFeatureHandler;
    diypinball_switchFeatureHandlerInit_t switchFeatureHandlerInit;

    switchFeatureHandlerInit.numSwitches = 17;
    switchFeatureHandlerInit.debounceChangedHandler = testDebounceChangedHandler;
    switchFeatureHandlerInit.readStateHandler = testReadStateHandler;
    switchFeatureHandlerInit.routerInstance = &router;

    diypinball_switchFeatureHandler_init(&switchFeatureHandler, &switchFeatureHandlerInit);

    ASSERT_EQ(1, switchFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, switchFeatureHandler.switches[i].lastState);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].messageTriggerMask);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].lastTick);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].pollingInterval);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].debounceLimit);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].ruleMask);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.boardAddress);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.solenoidNum);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.attackStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.attackDuration);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.sustainStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].closeRule.sustainDuration);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.boardAddress);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.solenoidNum);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.attackStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.attackDuration);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.sustainStatus);
        ASSERT_EQ(0, switchFeatureHandler.switches[i].openRule.sustainDuration);
    }

    ASSERT_EQ(&router, switchFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(&switchFeatureHandler, switchFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(16, switchFeatureHandler.numSwitches);
    ASSERT_TRUE(testReadStateHandler == switchFeatureHandler.readStateHandler);
    ASSERT_TRUE(testDebounceChangedHandler == switchFeatureHandler.debounceChangedHandler);
    ASSERT_TRUE(diypinball_switchFeatureHandler_millisecondTickHandler == switchFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_switchFeatureHandler_messageReceivedHandler == switchFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_0_to_any_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 0; i < 16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (i << 8) | (0 << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 1;
        initiatingCANMessage.data[0] = 0;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_0_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_0_to_valid_switch_gives_state)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 1; // Switch 0 is hard-coded to state = 1 for this case
    expectedCANMessage.data[1] = 1; // Previous state is a 0, so that's a rising edge -> 1 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 0; // Switch 1 is hard-coded to state = 0 for this case
    expectedCANMessage.data[1] = 0; // Previous state is a 0, so that's no edge -> 0 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_1_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_1_to_valid_switch_gives_polling_info)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0; // Switch 0 is initialized to 0 for polling interval
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_1_to_valid_switch_sets_polling_info)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 21;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(21, switchFeatureHandler.switches[0].pollingInterval);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_1_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 21;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchFeatureHandler.switches[15].pollingInterval);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_1_to_valid_switch_with_no_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 21;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(21, switchFeatureHandler.switches[0].pollingInterval);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(21, switchFeatureHandler.switches[0].pollingInterval);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_1_to_valid_switch_then_request_gets_polling_info)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 21;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(21, switchFeatureHandler.switches[0].pollingInterval);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 21;
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, switch_status_polling)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 10;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    diypinball_featureRouter_millisecondTick(&router, 0);
    diypinball_featureRouter_millisecondTick(&router, 1);
    diypinball_featureRouter_millisecondTick(&router, 2);
    diypinball_featureRouter_millisecondTick(&router, 3);
    diypinball_featureRouter_millisecondTick(&router, 4);
    diypinball_featureRouter_millisecondTick(&router, 5);
    diypinball_featureRouter_millisecondTick(&router, 6);
    diypinball_featureRouter_millisecondTick(&router, 7);
    diypinball_featureRouter_millisecondTick(&router, 8);
    diypinball_featureRouter_millisecondTick(&router, 9);

    expectedCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 1; // Switch 0 is hard-coded to state = 1 for this case
    expectedCANMessage.data[1] = 1; // Previous state is a 0, so that's a rising edge -> 1 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 10);
}

TEST_F(diypinball_switchFeatureHandler_test, switch_status_polling_edges)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 10;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    diypinball_featureRouter_millisecondTick(&router, 0);
    diypinball_featureRouter_millisecondTick(&router, 1);
    diypinball_featureRouter_millisecondTick(&router, 2);
    diypinball_featureRouter_millisecondTick(&router, 3);
    diypinball_featureRouter_millisecondTick(&router, 4);
    diypinball_featureRouter_millisecondTick(&router, 5);
    diypinball_featureRouter_millisecondTick(&router, 6);
    diypinball_featureRouter_millisecondTick(&router, 7);
    diypinball_featureRouter_millisecondTick(&router, 8);
    diypinball_featureRouter_millisecondTick(&router, 9);

    expectedCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 1; // Switch 0 is hard-coded to state = 1 for this case
    expectedCANMessage.data[1] = 1; // Previous state is a 0, so that's a rising edge -> 1 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 10);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_millisecondTick(&router, 11);
    diypinball_featureRouter_millisecondTick(&router, 12);
    diypinball_featureRouter_millisecondTick(&router, 13);
    diypinball_featureRouter_millisecondTick(&router, 14);
    diypinball_featureRouter_millisecondTick(&router, 15);
    diypinball_featureRouter_millisecondTick(&router, 16);
    diypinball_featureRouter_millisecondTick(&router, 17);
    diypinball_featureRouter_millisecondTick(&router, 18);
    diypinball_featureRouter_millisecondTick(&router, 19);

    expectedCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 1; // Switch 0 is hard-coded to state = 1 for this case
    expectedCANMessage.data[1] = 0; // Previous state is a 1, so that's a stable edge -> 0 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 20);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_millisecondTick(&router, 21);
    diypinball_featureRouter_millisecondTick(&router, 22);
    diypinball_featureRouter_millisecondTick(&router, 23);
    diypinball_featureRouter_millisecondTick(&router, 24);
    diypinball_featureRouter_millisecondTick(&router, 25);
    diypinball_featureRouter_millisecondTick(&router, 26);
    diypinball_featureRouter_millisecondTick(&router, 27);
    diypinball_featureRouter_millisecondTick(&router, 28);
    diypinball_featureRouter_millisecondTick(&router, 29);

    // swap out the handler
    switchFeatureHandler.readStateHandler = testReadStateHandlerZero;

    expectedCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 0; // Switch 0 is hard-coded to state = 0 for this case
    expectedCANMessage.data[1] = 2; // Previous state is a 1, so that's a falling edge -> 2 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 30);


    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_millisecondTick(&router, 31);
    diypinball_featureRouter_millisecondTick(&router, 32);
    diypinball_featureRouter_millisecondTick(&router, 33);
    diypinball_featureRouter_millisecondTick(&router, 34);
    diypinball_featureRouter_millisecondTick(&router, 35);
    diypinball_featureRouter_millisecondTick(&router, 36);
    diypinball_featureRouter_millisecondTick(&router, 37);
    diypinball_featureRouter_millisecondTick(&router, 38);
    diypinball_featureRouter_millisecondTick(&router, 39);

    expectedCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 0; // Switch 0 is hard-coded to state = 0 for this case
    expectedCANMessage.data[1] = 0; // Previous state is a 0, so that's a stable edge -> 0 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 40);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_0_edges)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 1; // Switch 0 is hard-coded to state = 1 for this case
    expectedCANMessage.data[1] = 1; // Previous state is a 0, so that's a rising edge -> 1 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 1; // Switch 0 is hard-coded to state = 1 for this case
    expectedCANMessage.data[1] = 0; // Previous state is a 1, so that's no edge -> 0 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    // swap out the handler
    switchFeatureHandler.readStateHandler = testReadStateHandlerZero;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 0; // Switch 0 is hard-coded to state = 0 for this case
    expectedCANMessage.data[1] = 2; // Previous state is a 1, so that's a falling edge -> 2 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 0; // Switch 0 is hard-coded to state = 0 for this case
    expectedCANMessage.data[1] = 0; // Previous state is a 1, so that's no edge -> 0 in this field
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_2_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_2_to_valid_switch_gives_triggering_info)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0; // Switch 0 is initialized to 0 for trigger
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_2_to_valid_switch_sets_trigger_mask)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchFeatureHandler.switches[0].messageTriggerMask);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_2_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchFeatureHandler.switches[15].messageTriggerMask);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_2_to_valid_switch_with_no_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchFeatureHandler.switches[0].messageTriggerMask);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchFeatureHandler.switches[0].messageTriggerMask);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_2_to_valid_switch_only_sets_valid_trigger_mask)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0xFF;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchFeatureHandler.switches[0].messageTriggerMask);
}

TEST_F(diypinball_switchFeatureHandler_test, set_rising_edge_trigger_and_register_switch_status)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x01;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x01, switchFeatureHandler.switches[0].messageTriggerMask);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);

    expectedCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 1;
    expectedCANMessage.data[1] = 1;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);
}

TEST_F(diypinball_switchFeatureHandler_test, set_falling_edge_trigger_and_register_switch_status)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x02;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x02, switchFeatureHandler.switches[0].messageTriggerMask);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);

    expectedCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 0;
    expectedCANMessage.data[1] = 2;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);
}

TEST_F(diypinball_switchFeatureHandler_test, set_both_edge_trigger_and_register_switch_status)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchFeatureHandler.switches[0].messageTriggerMask);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);

    expectedCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 1;
    expectedCANMessage.data[1] = 1;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);

    expectedCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 0;
    expectedCANMessage.data[1] = 2;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);
}

TEST_F(diypinball_switchFeatureHandler_test, set_no_edge_trigger_and_register_switch_status)
{
    ASSERT_EQ(0x00, switchFeatureHandler.switches[0].messageTriggerMask);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);
}

TEST_F(diypinball_switchFeatureHandler_test, set_both_edge_trigger_and_register_different_switch_status)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchFeatureHandler.switches[0].messageTriggerMask);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 1, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 1, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 1, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 1, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 1, 0);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_2_to_valid_switch_then_request_gets_trigger_info)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchFeatureHandler.switches[0].messageTriggerMask);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0x03;
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_3_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_3_to_valid_switch_gives_debouncing_info)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 0; // Switch 0 is initialized to 0 for trigger
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_3_to_valid_switch_sets_trigger_mask)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 120;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(0, 120)).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(120, switchFeatureHandler.switches[0].debounceLimit);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_3_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 120;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchFeatureHandler.switches[15].debounceLimit);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_3_to_valid_switch_with_no_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 120;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(0, 120)).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(120, switchFeatureHandler.switches[0].debounceLimit);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(120, switchFeatureHandler.switches[0].debounceLimit);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_3_to_valid_switch_then_request_gets_debounce_info)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 121;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(0, 121)).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(121, switchFeatureHandler.switches[0].debounceLimit);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 1;
    expectedCANMessage.data[0] = 121;
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_7_through_15_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 7; i < 16; i++) {
        for(uint8_t j = 0; j < 16; j++) {
            initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (j << 8) | (i << 4) | 0;
            initiatingCANMessage.rtr = 1;
            initiatingCANMessage.dlc = 0;

            EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
            EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
            EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

            diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
        }
    }
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_7_through_15_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 7; i < 16; i++) {
        for(uint8_t j = 0; j < 16; j++) {
            initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (j << 8) | (i << 4) | 0;
            initiatingCANMessage.rtr = 0;
            initiatingCANMessage.dlc = 2;
            initiatingCANMessage.data[0] = 0x42;
            initiatingCANMessage.data[1] = 0x24;

            EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
            EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
            EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

            diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
        }
    }
}

TEST(diypinball_switchFeatureHandler_test_other, request_to_function_6_gives_all_switch_statuses)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchFeatureHandlerHandlers mySwitchFeatureHandlerHandlers;
    SwitchFeatureHandlerHandlersImpl = &mySwitchFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchFeatureHandlerInstance_t switchFeatureHandler;
    diypinball_switchFeatureHandlerInit_t switchFeatureHandlerInit;

    switchFeatureHandlerInit.numSwitches = 15;
    switchFeatureHandlerInit.debounceChangedHandler = testDebounceChangedHandler;
    switchFeatureHandlerInit.readStateHandler = testReadStateHandlerAll;
    switchFeatureHandlerInit.routerInstance = &router;

    diypinball_switchFeatureHandler_init(&switchFeatureHandler, &switchFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (6 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (6 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 2;
    expectedCANMessage.data[0] = 0xAA; // Even switches are on, odd are off
    expectedCANMessage.data[1] = 0x2A;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(15);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_6_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (6 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x42;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_4_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_4_to_valid_switch_gives_rule_data)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 7;
    expectedCANMessage.data[0] = 0; // Switch 0 is initialized to 0 for trigger
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_4_to_valid_switch_sets_rule_data)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x01, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(43, switchFeatureHandler.switches[0].openRule.boardAddress);
    ASSERT_EQ(1, switchFeatureHandler.switches[0].openRule.solenoidNum);
    ASSERT_EQ(255, switchFeatureHandler.switches[0].openRule.attackStatus);
    ASSERT_EQ(25, switchFeatureHandler.switches[0].openRule.attackDuration);
    ASSERT_EQ(127, switchFeatureHandler.switches[0].openRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.sustainDuration);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_4_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.boardAddress);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.solenoidNum);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.attackStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.attackDuration);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.sustainDuration);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_4_to_valid_switch_with_no_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.boardAddress);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.solenoidNum);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.attackStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.attackDuration);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.sustainDuration);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_4_to_valid_switch_with_not_enough_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x01;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.boardAddress);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.solenoidNum);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.attackStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.attackDuration);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.sustainDuration);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_4_to_valid_switch_with_not_enough_data_for_sustain_only_sets_attack)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 6;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x01, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(43, switchFeatureHandler.switches[0].openRule.boardAddress);
    ASSERT_EQ(1, switchFeatureHandler.switches[0].openRule.solenoidNum);
    ASSERT_EQ(255, switchFeatureHandler.switches[0].openRule.attackStatus);
    ASSERT_EQ(25, switchFeatureHandler.switches[0].openRule.attackDuration);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.sustainDuration);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_4_to_valid_switch_and_request_back)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x01, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(43, switchFeatureHandler.switches[0].openRule.boardAddress);
    ASSERT_EQ(1, switchFeatureHandler.switches[0].openRule.solenoidNum);
    ASSERT_EQ(255, switchFeatureHandler.switches[0].openRule.attackStatus);
    ASSERT_EQ(25, switchFeatureHandler.switches[0].openRule.attackDuration);
    ASSERT_EQ(127, switchFeatureHandler.switches[0].openRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].openRule.sustainDuration);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;
 
    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 7;
    expectedCANMessage.data[0] = 0x01; // Switch 0 is initialized to 0 for trigger
    expectedCANMessage.data[1] = 43;
    expectedCANMessage.data[2] = 1;
    expectedCANMessage.data[3] = 255;
    expectedCANMessage.data[4] = 25;
    expectedCANMessage.data[5] = 127;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_5_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, request_to_function_5_to_valid_switch_gives_rule_data)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 7;
    expectedCANMessage.data[0] = 0; // Switch 0 is initialized to 0 for trigger
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;
    expectedCANMessage.data[3] = 0;
    expectedCANMessage.data[4] = 0;
    expectedCANMessage.data[5] = 0;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_5_to_valid_switch_sets_rule_data)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x02, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(43, switchFeatureHandler.switches[0].closeRule.boardAddress);
    ASSERT_EQ(1, switchFeatureHandler.switches[0].closeRule.solenoidNum);
    ASSERT_EQ(255, switchFeatureHandler.switches[0].closeRule.attackStatus);
    ASSERT_EQ(25, switchFeatureHandler.switches[0].closeRule.attackDuration);
    ASSERT_EQ(127, switchFeatureHandler.switches[0].closeRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.sustainDuration);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_5_to_invalid_switch_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.boardAddress);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.solenoidNum);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.attackStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.attackDuration);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.sustainDuration);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_5_to_valid_switch_with_no_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.boardAddress);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.solenoidNum);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.attackStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.attackDuration);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.sustainDuration);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_5_to_valid_switch_with_not_enough_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x01;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.boardAddress);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.solenoidNum);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.attackStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.attackDuration);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.sustainDuration);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_5_to_valid_switch_with_not_enough_data_for_sustain_only_sets_attack)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 6;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x02, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(43, switchFeatureHandler.switches[0].closeRule.boardAddress);
    ASSERT_EQ(1, switchFeatureHandler.switches[0].closeRule.solenoidNum);
    ASSERT_EQ(255, switchFeatureHandler.switches[0].closeRule.attackStatus);
    ASSERT_EQ(25, switchFeatureHandler.switches[0].closeRule.attackDuration);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.sustainDuration);
}

TEST_F(diypinball_switchFeatureHandler_test, message_to_function_5_to_valid_switch_and_request_back)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x02, switchFeatureHandler.switches[0].ruleMask);
    ASSERT_EQ(43, switchFeatureHandler.switches[0].closeRule.boardAddress);
    ASSERT_EQ(1, switchFeatureHandler.switches[0].closeRule.solenoidNum);
    ASSERT_EQ(255, switchFeatureHandler.switches[0].closeRule.attackStatus);
    ASSERT_EQ(25, switchFeatureHandler.switches[0].closeRule.attackDuration);
    ASSERT_EQ(127, switchFeatureHandler.switches[0].closeRule.sustainStatus);
    ASSERT_EQ(0, switchFeatureHandler.switches[0].closeRule.sustainDuration);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;
 
    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 7;
    expectedCANMessage.data[0] = 0x01; // Switch 0 is initialized to 0 for trigger
    expectedCANMessage.data[1] = 43;
    expectedCANMessage.data[2] = 1;
    expectedCANMessage.data[3] = 255;
    expectedCANMessage.data[4] = 25;
    expectedCANMessage.data[5] = 127;
    expectedCANMessage.data[6] = 0;
    expectedCANMessage.data[7] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, test_closed_rule_fires_with_switch_triggering)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage1, expectedCANMessage2;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0; // enable close rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0; // enable triggering
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x01;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);    
 
    expectedCANMessage1.id = (0x01 << 25) | (1 << 24) | (43 << 16) | (3 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 4;
    expectedCANMessage1.data[0] = 255;
    expectedCANMessage1.data[1] = 25;
    expectedCANMessage1.data[2] = 127;
    expectedCANMessage1.data[3] = 0;

    expectedCANMessage2.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage2.rtr = 0;
    expectedCANMessage2.dlc = 2;
    expectedCANMessage2.data[0] = 0x01;
    expectedCANMessage2.data[1] = 0x01;

    InSequence dummy;
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage2))).Times(1);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);
}

TEST_F(diypinball_switchFeatureHandler_test, test_open_rule_fires_with_switch_triggering)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage1, expectedCANMessage2;

    initiatingCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0; // enable open rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0; // enable triggering
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x02;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);
 
    expectedCANMessage1.id = (0x01 << 25) | (1 << 24) | (43 << 16) | (3 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 4;
    expectedCANMessage1.data[0] = 255;
    expectedCANMessage1.data[1] = 25;
    expectedCANMessage1.data[2] = 127;
    expectedCANMessage1.data[3] = 0;

    expectedCANMessage2.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage2.rtr = 0;
    expectedCANMessage2.dlc = 2;
    expectedCANMessage2.data[0] = 0x00;
    expectedCANMessage2.data[1] = 0x02;

    InSequence dummy;
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage2))).Times(1);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);
}

TEST_F(diypinball_switchFeatureHandler_test, test_closed_rule_fires_without_switch_triggering)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage1;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0; // enable close rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);    
 
    expectedCANMessage1.id = (0x01 << 25) | (1 << 24) | (43 << 16) | (3 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 4;
    expectedCANMessage1.data[0] = 255;
    expectedCANMessage1.data[1] = 25;
    expectedCANMessage1.data[2] = 127;
    expectedCANMessage1.data[3] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);
}

TEST_F(diypinball_switchFeatureHandler_test, test_open_rule_fires_without_switch_triggering)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage1;

    initiatingCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0; // enable open rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);
 
    expectedCANMessage1.id = (0x01 << 25) | (1 << 24) | (43 << 16) | (3 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 4;
    expectedCANMessage1.data[0] = 255;
    expectedCANMessage1.data[1] = 25;
    expectedCANMessage1.data[2] = 127;
    expectedCANMessage1.data[3] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);
}

TEST_F(diypinball_switchFeatureHandler_test, test_closed_rule_enabled_disabled_sends_solenoid_off)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage1;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0; // enable close rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0; // disable close rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x00;

    expectedCANMessage1.id = (0x01 << 25) | (1 << 24) | (43 << 16) | (3 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 1;
    expectedCANMessage1.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, test_open_rule_enabled_disabled_sends_solenoid_off)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage1;

    initiatingCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0; // enable open rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0; // disable close rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x00;

    expectedCANMessage1.id = (0x01 << 25) | (1 << 24) | (43 << 16) | (3 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 1;
    expectedCANMessage1.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_switchFeatureHandler_test, test_closed_rule_enabled_disabled_sends_solenoid_off_with_triggering)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage1;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0; // enable close rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0; // enable triggering
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x01;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (5 << 4) | 0; // disable close rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x00;

    expectedCANMessage1.id = (0x01 << 25) | (1 << 24) | (43 << 16) | (3 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 1;
    expectedCANMessage1.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);

    expectedCANMessage1.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 2;
    expectedCANMessage1.data[0] = 0x01;
    expectedCANMessage1.data[1] = 0x01;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);
}

TEST_F(diypinball_switchFeatureHandler_test, test_open_rule_enabled_disabled_sends_solenoid_off_with_triggering)
{
    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage1;

    initiatingCANMessage.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0; // enable open rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 7;
    initiatingCANMessage.data[0] = 0x01;
    initiatingCANMessage.data[1] = 43;
    initiatingCANMessage.data[2] = 1;
    initiatingCANMessage.data[3] = 255;
    initiatingCANMessage.data[4] = 25;
    initiatingCANMessage.data[5] = 127;
    initiatingCANMessage.data[6] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0; // enable triggering
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x02;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (4 << 4) | 0; // disable close rule
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x00;

    expectedCANMessage1.id = (0x01 << 25) | (1 << 24) | (43 << 16) | (3 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 1;
    expectedCANMessage1.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 1);

    expectedCANMessage1.id = (0x01 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage1.rtr = 0;
    expectedCANMessage1.dlc = 2;
    expectedCANMessage1.data[0] = 0x00;
    expectedCANMessage1.data[1] = 0x02;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage1))).Times(1);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchFeatureHandlerHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchFeatureHandler_registerSwitchState(&switchFeatureHandler, 0, 0);
}
