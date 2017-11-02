#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_switchMatrix.h"
#include "canMocks.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class MockSwitchMatrixHandlers {
public:
    virtual ~MockSwitchMatrixHandlers() {}
    MOCK_METHOD2(testReadStateHandler, void(uint8_t*, uint8_t));
    MOCK_METHOD2(testDebounceChangedHandler, void(uint8_t, uint8_t));
};

static MockCANSend* CANSendImpl;
static MockSwitchMatrixHandlers* SwitchMatrixHandlersImpl;

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        CANSendImpl->testCanSendHandler(message);
    }

    static void testDebounceChangedHandler(uint8_t switchNum, uint8_t debounceLimit) {
        SwitchMatrixHandlersImpl->testDebounceChangedHandler(switchNum, debounceLimit);
    }

    static void testReadStateHandler(uint8_t *state, uint8_t switchNum) {
        SwitchMatrixHandlersImpl->testReadStateHandler(state, switchNum);
        if(switchNum == 0) {
            *state = 1;
        } else {
            *state = 0;
        }
    }

    static void testReadStateHandlerZero(uint8_t *state, uint8_t switchNum) {
        SwitchMatrixHandlersImpl->testReadStateHandler(state, switchNum);
        *state = 0;
    }
}

TEST(diypinball_switchMatrix_test, init_zeros_structure)
{
    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 16;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    ASSERT_EQ(1, switchMatrix.featureDecoderInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, switchMatrix.switches[i].lastState);
        ASSERT_EQ(0, switchMatrix.switches[i].messageTriggerMask);
        ASSERT_EQ(0, switchMatrix.switches[i].lastTick);
        ASSERT_EQ(0, switchMatrix.switches[i].pollingInterval);
        ASSERT_EQ(0, switchMatrix.switches[i].debounceLimit);
        ASSERT_EQ(0, switchMatrix.switches[i].ruleMask);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.boardAddress);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.solenoidNum);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.attackStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.attackDuration);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.sustainStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.sustainDuration);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.boardAddress);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.solenoidNum);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.attackStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.attackDuration);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.sustainStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.sustainDuration);
    }

    ASSERT_EQ(&router, switchMatrix.featureDecoderInstance.routerInstance);
    ASSERT_EQ(&switchMatrix, switchMatrix.featureDecoderInstance.concreteFeatureDecoderInstance);
    ASSERT_EQ(16, switchMatrix.numSwitches);
    ASSERT_TRUE(testReadStateHandler == switchMatrix.readStateHandler);
    ASSERT_TRUE(testDebounceChangedHandler == switchMatrix.debounceChangedHandler);
    ASSERT_TRUE(diypinball_switchMatrix_millisecondTickHandler == switchMatrix.featureDecoderInstance.tickHandler);
    ASSERT_TRUE(diypinball_switchMatrix_messageReceivedHandler == switchMatrix.featureDecoderInstance.messageHandler);
}

TEST(diypinball_switchMatrix_test, deinit_zeros_structure)
{
    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 16;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_switchMatrix_deinit(&switchMatrix);

    ASSERT_EQ(0, switchMatrix.featureDecoderInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, switchMatrix.switches[i].lastState);
        ASSERT_EQ(0, switchMatrix.switches[i].messageTriggerMask);
        ASSERT_EQ(0, switchMatrix.switches[i].lastTick);
        ASSERT_EQ(0, switchMatrix.switches[i].pollingInterval);
        ASSERT_EQ(0, switchMatrix.switches[i].debounceLimit);
        ASSERT_EQ(0, switchMatrix.switches[i].ruleMask);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.boardAddress);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.solenoidNum);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.attackStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.attackDuration);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.sustainStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.sustainDuration);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.boardAddress);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.solenoidNum);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.attackStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.attackDuration);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.sustainStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.sustainDuration);
    }

    ASSERT_EQ(NULL, switchMatrix.featureDecoderInstance.routerInstance);
    ASSERT_EQ(NULL, switchMatrix.featureDecoderInstance.concreteFeatureDecoderInstance);
    ASSERT_EQ(0, switchMatrix.numSwitches);
    ASSERT_TRUE(NULL == switchMatrix.readStateHandler);
    ASSERT_TRUE(NULL == switchMatrix.debounceChangedHandler);
    ASSERT_TRUE(NULL == switchMatrix.featureDecoderInstance.tickHandler);
    ASSERT_TRUE(NULL == switchMatrix.featureDecoderInstance.messageHandler);
}

TEST(diypinball_switchMatrix_test, init_too_many_switches)
{
    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 17;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    ASSERT_EQ(1, switchMatrix.featureDecoderInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, switchMatrix.switches[i].lastState);
        ASSERT_EQ(0, switchMatrix.switches[i].messageTriggerMask);
        ASSERT_EQ(0, switchMatrix.switches[i].lastTick);
        ASSERT_EQ(0, switchMatrix.switches[i].pollingInterval);
        ASSERT_EQ(0, switchMatrix.switches[i].debounceLimit);
        ASSERT_EQ(0, switchMatrix.switches[i].ruleMask);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.boardAddress);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.solenoidNum);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.attackStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.attackDuration);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.sustainStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].closeRule.sustainDuration);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.boardAddress);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.solenoidNum);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.attackStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.attackDuration);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.sustainStatus);
        ASSERT_EQ(0, switchMatrix.switches[i].openRule.sustainDuration);
    }

    ASSERT_EQ(&router, switchMatrix.featureDecoderInstance.routerInstance);
    ASSERT_EQ(&switchMatrix, switchMatrix.featureDecoderInstance.concreteFeatureDecoderInstance);
    ASSERT_EQ(16, switchMatrix.numSwitches);
    ASSERT_TRUE(testReadStateHandler == switchMatrix.readStateHandler);
    ASSERT_TRUE(testDebounceChangedHandler == switchMatrix.debounceChangedHandler);
    ASSERT_TRUE(diypinball_switchMatrix_millisecondTickHandler == switchMatrix.featureDecoderInstance.tickHandler);
    ASSERT_TRUE(diypinball_switchMatrix_messageReceivedHandler == switchMatrix.featureDecoderInstance.messageHandler);
}

TEST(diypinball_switchMatrix_test, message_to_function_0_to_any_switch_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 0; i < 16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (i << 8) | (0 << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 1;
        initiatingCANMessage.data[0] = 0;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
        EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST(diypinball_switchMatrix_test, request_to_function_0_to_invalid_switch_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, request_to_function_0_to_valid_switch_gives_state)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, request_to_function_1_to_invalid_switch_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, request_to_function_1_to_valid_switch_gives_polling_info)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, message_to_function_1_to_valid_switch_sets_polling_info)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 21;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(21, switchMatrix.switches[0].pollingInterval);
}

TEST(diypinball_switchMatrix_test, message_to_function_1_to_invalid_switch_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 21;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchMatrix.switches[15].pollingInterval);
}

TEST(diypinball_switchMatrix_test, message_to_function_1_to_valid_switch_with_no_data_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 21;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(21, switchMatrix.switches[0].pollingInterval);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(21, switchMatrix.switches[0].pollingInterval);
}

TEST(diypinball_switchMatrix_test, message_to_function_1_to_valid_switch_then_request_gets_polling_info)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 21;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(21, switchMatrix.switches[0].pollingInterval);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, switch_status_polling)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 10;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

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

    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 10);
}

TEST(diypinball_switchMatrix_test, switch_status_polling_edges)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 10;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

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

    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 10);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

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

    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 20);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

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
    switchMatrix.readStateHandler = testReadStateHandlerZero;

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

    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 30);


    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

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

    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);
    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);

    diypinball_featureRouter_millisecondTick(&router, 40);
}

TEST(diypinball_switchMatrix_test, request_to_function_0_edges)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    // swap out the handler
    switchMatrix.readStateHandler = testReadStateHandlerZero;

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(1);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, request_to_function_2_to_invalid_switch_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, request_to_function_2_to_valid_switch_gives_triggering_info)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, message_to_function_2_to_valid_switch_sets_trigger_mask)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchMatrix.switches[0].messageTriggerMask);
}

TEST(diypinball_switchMatrix_test, message_to_function_2_to_invalid_switch_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchMatrix.switches[15].messageTriggerMask);
}

TEST(diypinball_switchMatrix_test, message_to_function_2_to_valid_switch_with_no_data_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchMatrix.switches[0].messageTriggerMask);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchMatrix.switches[0].messageTriggerMask);
}

TEST(diypinball_switchMatrix_test, message_to_function_2_to_valid_switch_only_sets_valid_trigger_mask)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0xFF;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchMatrix.switches[0].messageTriggerMask);
}

TEST(diypinball_switchMatrix_test, set_rising_edge_trigger_and_register_switch_status)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x01;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x01, switchMatrix.switches[0].messageTriggerMask);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);
}

TEST(diypinball_switchMatrix_test, set_falling_edge_trigger_and_register_switch_status)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x02;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x02, switchMatrix.switches[0].messageTriggerMask);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 1);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);
}

TEST(diypinball_switchMatrix_test, set_both_edge_trigger_and_register_switch_status)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchMatrix.switches[0].messageTriggerMask);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 1);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);
}

TEST(diypinball_switchMatrix_test, set_no_edge_trigger_and_register_switch_status)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    ASSERT_EQ(0x00, switchMatrix.switches[0].messageTriggerMask);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 0, 0);
}

TEST(diypinball_switchMatrix_test, set_both_edge_trigger_and_register_different_switch_status)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchMatrix.switches[0].messageTriggerMask);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 1, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 1, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 1, 1);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 1, 0);

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_switchMatrix_registerSwitchState(&switchMatrix, 1, 0);
}

TEST(diypinball_switchMatrix_test, message_to_function_2_to_valid_switch_then_request_gets_trigger_info)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (2 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x03;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0x03, switchMatrix.switches[0].messageTriggerMask);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, request_to_function_3_to_invalid_switch_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, request_to_function_3_to_valid_switch_gives_debouncing_info)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, message_to_function_3_to_valid_switch_sets_trigger_mask)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 120;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(0, 120)).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(120, switchMatrix.switches[0].debounceLimit);
}

TEST(diypinball_switchMatrix_test, message_to_function_3_to_invalid_switch_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (15 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 120;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(0, switchMatrix.switches[15].debounceLimit);
}

TEST(diypinball_switchMatrix_test, message_to_function_3_to_valid_switch_with_no_data_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 120;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(0, 120)).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(120, switchMatrix.switches[0].debounceLimit);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(120, switchMatrix.switches[0].debounceLimit);
}

TEST(diypinball_switchMatrix_test, message_to_function_3_to_valid_switch_then_request_gets_debounce_info)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (0 << 8) | (3 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 121;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(0, 121)).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    ASSERT_EQ(121, switchMatrix.switches[0].debounceLimit);

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
    EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
    EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_switchMatrix_test, request_to_function_4_through_15_does_nothing)
{
    diypinball_featureRouterInstance router;
    diypinball_featureRouterInit routerInit;

    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 4; i < 16; i++) {
        for(uint8_t j = 0; j < 16; j++) {
            initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (j << 8) | (i << 4) | 0;
            initiatingCANMessage.rtr = 1;
            initiatingCANMessage.dlc = 0;

            EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
            EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
            EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

            diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
        }
    }
}

TEST(diypinball_switchMatrix_test, message_to_function_4_through_15_does_nothing)
{
    diypinball_featureRouterInstance router;
    diypinball_featureRouterInit routerInit;

    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockSwitchMatrixHandlers mySwitchMatrixHandlers;
    SwitchMatrixHandlersImpl = &mySwitchMatrixHandlers;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_switchMatrixInstance_t switchMatrix;
    diypinball_switchMatrixInit_t switchMatrixInit;

    switchMatrixInit.numSwitches = 15;
    switchMatrixInit.debounceChangedHandler = testDebounceChangedHandler;
    switchMatrixInit.readStateHandler = testReadStateHandler;
    switchMatrixInit.routerInstance = &router;

    diypinball_switchMatrix_init(&switchMatrix, &switchMatrixInit);

    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 4; i < 16; i++) {
        for(uint8_t j = 0; j < 16; j++) {
            initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (1 << 12) | (j << 8) | (i << 4) | 0;
            initiatingCANMessage.rtr = 0;
            initiatingCANMessage.dlc = 2;
            initiatingCANMessage.data[0] = 0x42;
            initiatingCANMessage.data[1] = 0x24;

            EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
            EXPECT_CALL(mySwitchMatrixHandlers, testReadStateHandler(_, _)).Times(0);
            EXPECT_CALL(mySwitchMatrixHandlers, testDebounceChangedHandler(_, _)).Times(0);

            diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
        }
    }
}