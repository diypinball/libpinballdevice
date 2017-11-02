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