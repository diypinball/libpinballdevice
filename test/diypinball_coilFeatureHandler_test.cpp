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
    uint8_t fieldFlag = (arg.attackState == status.attackState) &&
        (arg.attackDuration == status.attackDuration) &&
        (arg.sustainState == status.sustainState) &&
        (arg.sustainDuration == status.sustainDuration);

    return !(!fieldFlag);
}

TEST(diypinball_coilFeatureHandler_test, init_zeros_structure)
{
    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_coilFeatureHandlerInstance_t coilFeatureHandler;
    diypinball_coilFeatureHandlerInit_t coilFeatureHandlerInit;

    coilFeatureHandlerInit.numCoils = 16;
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
