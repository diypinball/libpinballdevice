#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_rgbFeatureHandler.h"
#include "canMocks.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class MockRGBFeatureHandlerHandlers {
public:
    virtual ~MockRGBFeatureHandlerHandlers() {}
    MOCK_METHOD2(testRGBChangedHandler, void(uint8_t, diypinball_rgbStatus_t));
};

static MockCANSend* CANSendImpl;
static MockRGBFeatureHandlerHandlers* RGBFeatureHandlerHandlersImpl;

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        CANSendImpl->testCanSendHandler(message);
    }

    static void testRGBChangedHandler(uint8_t rgbNum, diypinball_rgbStatus_t rgbStatus) {
        RGBFeatureHandlerHandlersImpl->testRGBChangedHandler(rgbNum, rgbStatus);
    }
}

MATCHER_P(RGBStatusEqual, status, "") {
    //printf("Arg: %d, %d, %d, %d, %d, %d, %d\n", arg.state1, arg.state1Duration, arg.state2, arg.state2Duration, arg.state3, arg.state3Duration, arg.numStates);
    //printf("Sta: %d, %d, %d, %d, %d, %d, %d\n", status.state1, status.state1Duration, status.state2, status.state2Duration, status.state3, status.state3Duration, status.numStates);
    uint8_t fieldFlag = (arg.red == status.red) &&
        (arg.green == status.green) &&
        (arg.blue == status.blue);

    return !(!fieldFlag);
}

TEST(diypinball_rgbFeatureHandler_test, init_zeros_structure)
{
    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 16;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    ASSERT_EQ(4, rgbFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, rgbFeatureHandler.rgbs[i].red);
        ASSERT_EQ(0, rgbFeatureHandler.rgbs[i].green);
        ASSERT_EQ(0, rgbFeatureHandler.rgbs[i].blue);
    }

    ASSERT_EQ(&router, rgbFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(&rgbFeatureHandler, rgbFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(16, rgbFeatureHandler.numRGBs);
    ASSERT_TRUE(testRGBChangedHandler == rgbFeatureHandler.rgbChangedHandler);
    ASSERT_TRUE(diypinball_rgbFeatureHandler_millisecondTickHandler == rgbFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_rgbFeatureHandler_messageReceivedHandler == rgbFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST(diypinball_rgbFeatureHandler_test, deinit_zeros_structure)
{
    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 16;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_rgbFeatureHandler_deinit(&rgbFeatureHandler);

    ASSERT_EQ(0, rgbFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, rgbFeatureHandler.rgbs[i].red);
        ASSERT_EQ(0, rgbFeatureHandler.rgbs[i].green);
        ASSERT_EQ(0, rgbFeatureHandler.rgbs[i].blue);
    }

    ASSERT_EQ(NULL, rgbFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(NULL, rgbFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(0, rgbFeatureHandler.numRGBs);
    ASSERT_TRUE(NULL == rgbFeatureHandler.rgbChangedHandler);
    ASSERT_TRUE(NULL == rgbFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(NULL == rgbFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST(diypinball_rgbFeatureHandler_test, init_too_many_rgbs)
{
    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 17;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    ASSERT_EQ(4, rgbFeatureHandler.featureHandlerInstance.featureType);

    for(uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(0, rgbFeatureHandler.rgbs[i].red);
        ASSERT_EQ(0, rgbFeatureHandler.rgbs[i].green);
        ASSERT_EQ(0, rgbFeatureHandler.rgbs[i].blue);
    }

    ASSERT_EQ(&router, rgbFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(&rgbFeatureHandler, rgbFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(16, rgbFeatureHandler.numRGBs);
    ASSERT_TRUE(testRGBChangedHandler == rgbFeatureHandler.rgbChangedHandler);
    ASSERT_TRUE(diypinball_rgbFeatureHandler_millisecondTickHandler == rgbFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_rgbFeatureHandler_messageReceivedHandler == rgbFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST(diypinball_rgbFeatureHandler_test, request_to_function_0_to_invalid_rgb_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 15;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (15 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_rgbFeatureHandler_test, request_to_function_0_to_valid_rgb_returns_rgb_state)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 15;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 3;
    expectedCANMessage.data[0] = 0;
    expectedCANMessage.data[1] = 0;
    expectedCANMessage.data[2] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_rgbFeatureHandler_test, message_to_function_0_to_invalid_rgb_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 15;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (15 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_rgbFeatureHandler_test, message_to_function_0_to_valid_rgb_and_no_data_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 15;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_rgbFeatureHandler_test, message_to_function_0_to_valid_rgb_with_not_enough_data_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 15;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 126;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_rgbFeatureHandler_test, message_to_function_0_to_valid_rgb_with_one_and_enough_data_sets_rgb)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 15;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 3;
    initiatingCANMessage.data[0] = 126;
    initiatingCANMessage.data[1] = 20;
    initiatingCANMessage.data[2] = 255;

    diypinball_rgbStatus_t expectedRGB;
    expectedRGB.red = 126;
    expectedRGB.green = 20;
    expectedRGB.blue = 255;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(1, RGBStatusEqual(expectedRGB))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_rgbFeatureHandler_test, message_to_function_0_to_valid_rgb_with_all_data_and_read_back)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 15;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage, expectedCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 6;
    initiatingCANMessage.data[0] = 126;
    initiatingCANMessage.data[1] = 20;
    initiatingCANMessage.data[2] = 255;

    diypinball_rgbStatus_t expectedRGB;
    expectedRGB.red = 126;
    expectedRGB.green = 20;
    expectedRGB.blue = 255;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(1, RGBStatusEqual(expectedRGB))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (1 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;
    initiatingCANMessage.data[1] = 0;
    initiatingCANMessage.data[2] = 0;
    initiatingCANMessage.data[3] = 0;
    initiatingCANMessage.data[4] = 0;
    initiatingCANMessage.data[5] = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (1 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 3;

    expectedCANMessage.data[0] = 126;
    expectedCANMessage.data[1] = 20;
    expectedCANMessage.data[2] = 255;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

/*TEST(diypinball_rgbFeatureHandler_test, request_to_function_1_to_any_set_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 15;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 0; i < 16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (1 << 4) | 0;
        initiatingCANMessage.rtr = 1;
        initiatingCANMessage.dlc = 0;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(_, _)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST(diypinball_rgbFeatureHandler_test, message_to_function_1_to_low_set_changes_rgbs)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 16;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (0 << 8) | (1 << 4) | 0;
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

    diypinball_rgbStatus_t expectedRGB0, expectedRGB1, expectedRGB2, expectedRGB3, expectedRGB4, expectedRGB5, expectedRGB6, expectedRGB7;
    expectedRGB0.state1 = 16;
    expectedRGB0.state1Duration = 0;
    expectedRGB0.state2 = 0;
    expectedRGB0.state2Duration = 0;
    expectedRGB0.state3 = 0;
    expectedRGB0.state3Duration = 0;
    expectedRGB0.numStates = 1;
    expectedRGB1.state1 = 32;
    expectedRGB1.state1Duration = 0;
    expectedRGB1.state2 = 0;
    expectedRGB1.state2Duration = 0;
    expectedRGB1.state3 = 0;
    expectedRGB1.state3Duration = 0;
    expectedRGB1.numStates = 1;
    expectedRGB2.state1 = 64;
    expectedRGB2.state1Duration = 0;
    expectedRGB2.state2 = 0;
    expectedRGB2.state2Duration = 0;
    expectedRGB2.state3 = 0;
    expectedRGB2.state3Duration = 0;
    expectedRGB2.numStates = 1;
    expectedRGB3.state1 = 128;
    expectedRGB3.state1Duration = 0;
    expectedRGB3.state2 = 0;
    expectedRGB3.state2Duration = 0;
    expectedRGB3.state3 = 0;
    expectedRGB3.state3Duration = 0;
    expectedRGB3.numStates = 1;
    expectedRGB4.state1 = 8;
    expectedRGB4.state1Duration = 0;
    expectedRGB4.state2 = 0;
    expectedRGB4.state2Duration = 0;
    expectedRGB4.state3 = 0;
    expectedRGB4.state3Duration = 0;
    expectedRGB4.numStates = 1;
    expectedRGB5.state1 = 4;
    expectedRGB5.state1Duration = 0;
    expectedRGB5.state2 = 0;
    expectedRGB5.state2Duration = 0;
    expectedRGB5.state3 = 0;
    expectedRGB5.state3Duration = 0;
    expectedRGB5.numStates = 1;
    expectedRGB6.state1 = 2;
    expectedRGB6.state1Duration = 0;
    expectedRGB6.state2 = 0;
    expectedRGB6.state2Duration = 0;
    expectedRGB6.state3 = 0;
    expectedRGB6.state3Duration = 0;
    expectedRGB6.numStates = 1;
    expectedRGB7.state1 = 1;
    expectedRGB7.state1Duration = 0;
    expectedRGB7.state2 = 0;
    expectedRGB7.state2Duration = 0;
    expectedRGB7.state3 = 0;
    expectedRGB7.state3Duration = 0;
    expectedRGB7.numStates = 1;

    InSequence dummy;
    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(0, RGBStatusEqual(expectedRGB0))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(1, RGBStatusEqual(expectedRGB1))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(2, RGBStatusEqual(expectedRGB2))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(3, RGBStatusEqual(expectedRGB3))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(4, RGBStatusEqual(expectedRGB4))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(5, RGBStatusEqual(expectedRGB5))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(6, RGBStatusEqual(expectedRGB6))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(7, RGBStatusEqual(expectedRGB7))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_rgbFeatureHandler_test, message_to_function_1_to_high_set_changes_rgbs)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 16;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (1 << 8) | (1 << 4) | 0;
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

    diypinball_rgbStatus_t expectedRGB0, expectedRGB1, expectedRGB2, expectedRGB3, expectedRGB4, expectedRGB5, expectedRGB6, expectedRGB7;
    expectedRGB0.state1 = 16;
    expectedRGB0.state1Duration = 0;
    expectedRGB0.state2 = 0;
    expectedRGB0.state2Duration = 0;
    expectedRGB0.state3 = 0;
    expectedRGB0.state3Duration = 0;
    expectedRGB0.numStates = 1;
    expectedRGB1.state1 = 32;
    expectedRGB1.state1Duration = 0;
    expectedRGB1.state2 = 0;
    expectedRGB1.state2Duration = 0;
    expectedRGB1.state3 = 0;
    expectedRGB1.state3Duration = 0;
    expectedRGB1.numStates = 1;
    expectedRGB2.state1 = 64;
    expectedRGB2.state1Duration = 0;
    expectedRGB2.state2 = 0;
    expectedRGB2.state2Duration = 0;
    expectedRGB2.state3 = 0;
    expectedRGB2.state3Duration = 0;
    expectedRGB2.numStates = 1;
    expectedRGB3.state1 = 128;
    expectedRGB3.state1Duration = 0;
    expectedRGB3.state2 = 0;
    expectedRGB3.state2Duration = 0;
    expectedRGB3.state3 = 0;
    expectedRGB3.state3Duration = 0;
    expectedRGB3.numStates = 1;
    expectedRGB4.state1 = 8;
    expectedRGB4.state1Duration = 0;
    expectedRGB4.state2 = 0;
    expectedRGB4.state2Duration = 0;
    expectedRGB4.state3 = 0;
    expectedRGB4.state3Duration = 0;
    expectedRGB4.numStates = 1;
    expectedRGB5.state1 = 4;
    expectedRGB5.state1Duration = 0;
    expectedRGB5.state2 = 0;
    expectedRGB5.state2Duration = 0;
    expectedRGB5.state3 = 0;
    expectedRGB5.state3Duration = 0;
    expectedRGB5.numStates = 1;
    expectedRGB6.state1 = 2;
    expectedRGB6.state1Duration = 0;
    expectedRGB6.state2 = 0;
    expectedRGB6.state2Duration = 0;
    expectedRGB6.state3 = 0;
    expectedRGB6.state3Duration = 0;
    expectedRGB6.numStates = 1;
    expectedRGB7.state1 = 1;
    expectedRGB7.state1Duration = 0;
    expectedRGB7.state2 = 0;
    expectedRGB7.state2Duration = 0;
    expectedRGB7.state3 = 0;
    expectedRGB7.state3Duration = 0;
    expectedRGB7.numStates = 1;

    InSequence dummy;
    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(8, RGBStatusEqual(expectedRGB0))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(9, RGBStatusEqual(expectedRGB1))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(10, RGBStatusEqual(expectedRGB2))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(11, RGBStatusEqual(expectedRGB3))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(12, RGBStatusEqual(expectedRGB4))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(13, RGBStatusEqual(expectedRGB5))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(14, RGBStatusEqual(expectedRGB6))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(15, RGBStatusEqual(expectedRGB7))).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_rgbFeatureHandler_test, message_to_function_1_to_low_set_with_no_data_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 16;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(_, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST(diypinball_rgbFeatureHandler_test, message_to_function_1_to_invalid_set_does_nothing)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 16;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 2; i < 16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (i << 8) | (1 << 4) | 0;
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
        EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(_, _)).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST(diypinball_rgbFeatureHandler_test, message_to_function_1_to_incomplete_high_set_changes_rgbs)
{
    MockCANSend myCANSend;
    CANSendImpl = &myCANSend;
    MockRGBFeatureHandlerHandlers myRGBFeatureHandlerHandlers;
    RGBFeatureHandlerHandlersImpl = &myRGBFeatureHandlerHandlers;

    diypinball_featureRouterInstance_t router;
    diypinball_featureRouterInit_t routerInit;

    routerInit.boardAddress = 42;
    routerInit.canSendHandler = testCanSendHandler;

    diypinball_featureRouter_init(&router, &routerInit);

    diypinball_rgbFeatureHandlerInstance_t rgbFeatureHandler;
    diypinball_rgbFeatureHandlerInit_t rgbFeatureHandlerInit;

    rgbFeatureHandlerInit.numRGBs = 14;
    rgbFeatureHandlerInit.rgbChangedHandler = testRGBChangedHandler;
    rgbFeatureHandlerInit.routerInstance = &router;

    diypinball_rgbFeatureHandler_init(&rgbFeatureHandler, &rgbFeatureHandlerInit);

    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (4 << 12) | (1 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 6;
    initiatingCANMessage.data[0] = 16;
    initiatingCANMessage.data[1] = 32;
    initiatingCANMessage.data[2] = 64;
    initiatingCANMessage.data[3] = 128;
    initiatingCANMessage.data[4] = 8;
    initiatingCANMessage.data[5] = 4;

    diypinball_rgbStatus_t expectedRGB0, expectedRGB1, expectedRGB2, expectedRGB3, expectedRGB4, expectedRGB5;
    expectedRGB0.state1 = 16;
    expectedRGB0.state1Duration = 0;
    expectedRGB0.state2 = 0;
    expectedRGB0.state2Duration = 0;
    expectedRGB0.state3 = 0;
    expectedRGB0.state3Duration = 0;
    expectedRGB0.numStates = 1;
    expectedRGB1.state1 = 32;
    expectedRGB1.state1Duration = 0;
    expectedRGB1.state2 = 0;
    expectedRGB1.state2Duration = 0;
    expectedRGB1.state3 = 0;
    expectedRGB1.state3Duration = 0;
    expectedRGB1.numStates = 1;
    expectedRGB2.state1 = 64;
    expectedRGB2.state1Duration = 0;
    expectedRGB2.state2 = 0;
    expectedRGB2.state2Duration = 0;
    expectedRGB2.state3 = 0;
    expectedRGB2.state3Duration = 0;
    expectedRGB2.numStates = 1;
    expectedRGB3.state1 = 128;
    expectedRGB3.state1Duration = 0;
    expectedRGB3.state2 = 0;
    expectedRGB3.state2Duration = 0;
    expectedRGB3.state3 = 0;
    expectedRGB3.state3Duration = 0;
    expectedRGB3.numStates = 1;
    expectedRGB4.state1 = 8;
    expectedRGB4.state1Duration = 0;
    expectedRGB4.state2 = 0;
    expectedRGB4.state2Duration = 0;
    expectedRGB4.state3 = 0;
    expectedRGB4.state3Duration = 0;
    expectedRGB4.numStates = 1;
    expectedRGB5.state1 = 4;
    expectedRGB5.state1Duration = 0;
    expectedRGB5.state2 = 0;
    expectedRGB5.state2Duration = 0;
    expectedRGB5.state3 = 0;
    expectedRGB5.state3Duration = 0;
    expectedRGB5.numStates = 1;

    InSequence dummy;
    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(8, RGBStatusEqual(expectedRGB0))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(9, RGBStatusEqual(expectedRGB1))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(10, RGBStatusEqual(expectedRGB2))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(11, RGBStatusEqual(expectedRGB3))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(12, RGBStatusEqual(expectedRGB4))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(13, RGBStatusEqual(expectedRGB5))).Times(1);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(14, _)).Times(0);
    EXPECT_CALL(myRGBFeatureHandlerHandlers, testRGBChangedHandler(15, _)).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}
*/