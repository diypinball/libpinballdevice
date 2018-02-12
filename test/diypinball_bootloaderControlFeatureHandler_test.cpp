#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <stdint.h>
#include <inttypes.h>

#include "diypinball.h"
#include "diypinball_featureRouter.h"
#include "diypinball_bootloaderControlFeatureHandler.h"
#include "canMocks.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class MockBootloaderControlHandlers {
public:
    virtual ~MockBootloaderControlHandlers() {}
    MOCK_METHOD0(testRebootHandler, void(void));
};

static MockCANSend* CANSendImpl;
static MockBootloaderControlHandlers* BootloaderControlHandlersImpl;

extern "C" {
    static void testCanSendHandler(diypinball_canMessage_t *message) {
        CANSendImpl->testCanSendHandler(message);
    }

    static void testRebootHandler(void) {
        BootloaderControlHandlersImpl->testRebootHandler();
    }
}

class diypinball_bootloaderControlFeatureHandler_test : public testing::Test {
    protected: 

    virtual void SetUp() {
        CANSendImpl = &myCANSend;
        BootloaderControlHandlersImpl = &myBootloaderControlHandlers;

        diypinball_featureRouterInit_t routerInit;

        routerInit.boardAddress = 42;
        routerInit.canSendHandler = testCanSendHandler;

        diypinball_featureRouter_init(&router, &routerInit);

        diypinball_bootloaderControlFeatureHandlerInit bootloaderControlFeatureHandlerInit;

        bootloaderControlFeatureHandlerInit.bootloaderVersionMajor = 1;
        bootloaderControlFeatureHandlerInit.bootloaderVersionMinor = 2;
        bootloaderControlFeatureHandlerInit.bootloaderVersionPatch = 3;
        bootloaderControlFeatureHandlerInit.rebootHandler = testRebootHandler;
        bootloaderControlFeatureHandlerInit.routerInstance = &router;

        diypinball_bootloaderControlFeatureHandler_init(&bootloaderControlFeatureHandler, &bootloaderControlFeatureHandlerInit);
    }

    diypinball_featureRouterInstance_t router;
    diypinball_bootloaderControlFeatureHandlerInstance bootloaderControlFeatureHandler;
    MockCANSend myCANSend;
    MockBootloaderControlHandlers myBootloaderControlHandlers;
};

TEST_F(diypinball_bootloaderControlFeatureHandler_test, init_zeros_structure)
{
    ASSERT_EQ(6, bootloaderControlFeatureHandler.featureHandlerInstance.featureType);
    ASSERT_EQ(1, bootloaderControlFeatureHandler.bootloaderVersionMajor);
    ASSERT_EQ(2, bootloaderControlFeatureHandler.bootloaderVersionMinor);
    ASSERT_EQ(3, bootloaderControlFeatureHandler.bootloaderVersionPatch);

    ASSERT_EQ(&router, bootloaderControlFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_TRUE(testRebootHandler == bootloaderControlFeatureHandler.rebootHandler);
    ASSERT_TRUE(diypinball_bootloaderControlFeatureHandler_millisecondTickHandler == bootloaderControlFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(diypinball_bootloaderControlFeatureHandler_messageReceivedHandler == bootloaderControlFeatureHandler.featureHandlerInstance.messageHandler);
}

TEST_F(diypinball_bootloaderControlFeatureHandler_test, request_to_function_0_sends_id_response)
{
    diypinball_canMessage_t expectedCANMessage, initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (6 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    expectedCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (6 << 12) | (0 << 8) | (0 << 4) | 0;
    expectedCANMessage.rtr = 0;
    expectedCANMessage.dlc = 3;
    expectedCANMessage.data[0] = 1;
    expectedCANMessage.data[1] = 2;
    expectedCANMessage.data[2] = 3;

    EXPECT_CALL(myCANSend, testCanSendHandler(CanMessageEqual(expectedCANMessage))).Times(1);
    EXPECT_CALL(myBootloaderControlHandlers, testRebootHandler()).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderControlFeatureHandler_test, message_to_function_0_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (6 << 12) | (0 << 8) | (0 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderControlHandlers, testRebootHandler()).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderControlFeatureHandler_test, request_to_function_1_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (6 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 1;
    initiatingCANMessage.dlc = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderControlHandlers, testRebootHandler()).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderControlFeatureHandler_test, message_to_function_1_with_no_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (6 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 0;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderControlHandlers, testRebootHandler()).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderControlFeatureHandler_test, message_to_function_1_with_wrong_data_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (6 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderControlHandlers, testRebootHandler()).Times(0);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderControlFeatureHandler_test, message_to_function_1_with_right_data_reboots)
{
    diypinball_canMessage_t initiatingCANMessage;

    initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (6 << 12) | (0 << 8) | (1 << 4) | 0;
    initiatingCANMessage.rtr = 0;
    initiatingCANMessage.dlc = 1;
    initiatingCANMessage.data[0] = 0x42;

    EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
    EXPECT_CALL(myBootloaderControlHandlers, testRebootHandler()).Times(1);

    diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
}

TEST_F(diypinball_bootloaderControlFeatureHandler_test, request_to_function_2_through_15_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 2; i < 16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (6 << 12) | (0 << 8) | (i << 4) | 0;
        initiatingCANMessage.rtr = 1;
        initiatingCANMessage.dlc = 0;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderControlHandlers, testRebootHandler()).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);

    }
}

TEST_F(diypinball_bootloaderControlFeatureHandler_test, message_to_function_2_through_15_does_nothing)
{
    diypinball_canMessage_t initiatingCANMessage;

    for(uint8_t i = 2; i < 16; i++) {
        initiatingCANMessage.id = (0x00 << 25) | (1 << 24) | (42 << 16) | (6 << 12) | (0 << 8) | (i << 4) | 0;
        initiatingCANMessage.rtr = 0;
        initiatingCANMessage.dlc = 1;
        initiatingCANMessage.data[0] = 0;

        EXPECT_CALL(myCANSend, testCanSendHandler(_)).Times(0);
        EXPECT_CALL(myBootloaderControlHandlers, testRebootHandler()).Times(0);

        diypinball_featureRouter_receiveCAN(&router, &initiatingCANMessage);
    }
}

TEST_F(diypinball_bootloaderControlFeatureHandler_test, deinit_zeros_structure)
{
    diypinball_bootloaderControlFeatureHandler_deinit(&bootloaderControlFeatureHandler);

    ASSERT_EQ(0, bootloaderControlFeatureHandler.bootloaderVersionMajor);
    ASSERT_EQ(0, bootloaderControlFeatureHandler.bootloaderVersionMinor);
    ASSERT_EQ(0, bootloaderControlFeatureHandler.bootloaderVersionPatch);

    ASSERT_EQ(NULL, bootloaderControlFeatureHandler.featureHandlerInstance.routerInstance);
    ASSERT_EQ(NULL, bootloaderControlFeatureHandler.featureHandlerInstance.concreteFeatureHandlerInstance);
    ASSERT_EQ(0, bootloaderControlFeatureHandler.featureHandlerInstance.featureType);
    ASSERT_TRUE(NULL == bootloaderControlFeatureHandler.rebootHandler);
    ASSERT_TRUE(NULL == bootloaderControlFeatureHandler.featureHandlerInstance.tickHandler);
    ASSERT_TRUE(NULL == bootloaderControlFeatureHandler.featureHandlerInstance.messageHandler);
}
